// Implementation of the advanced configuration validation system
#include "Validator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <cctype>

namespace nlm {

// ===============================================================
// ADVANCED VALIDATOR IMPLEMENTATION
// ===============================================================

struct AdvancedValidator::Impl {
    std::vector<ValidationRule> rules;
    std::vector<ConfigurationSchema> schemas;
    ValidationContext context;
    
    // Statistics
    size_t totalValidations = 0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    double totalValidationTime = 0.0;
    
    // Configuration
    ValidationLevel validationLevel = ValidationLevel::Strict;
    bool autoRepairEnabled = false;
    
    // Rule engine
    std::unordered_map<std::string, std::vector<ValidationRule>> rulesByType;
    std::unordered_map<ConfigurationScope, std::vector<ValidationRule>> rulesByScope;
    
    // Schema engine
    std::unordered_map<std::string, ConfigurationSchema> schemasByName;
    std::unordered_map<ConfigurationScope, std::vector<ConfigurationSchema>> schemasByScope;
    
    // Session management
    std::unordered_map<std::string, ValidationResult> sessionResults;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> sessionTimes;
};

AdvancedValidator::AdvancedValidator() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultRules();
    initializeSchemaValidations();
    setupRuleEngine();
}

AdvancedValidator::~AdvancedValidator() = default;

// Core validation methods
ValidationResult AdvancedValidator::validate(const Config& config) {
    return validate(config, pImpl->context);
}

ValidationResult AdvancedValidator::validate(const Config& config, const ValidationContext& context) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ValidationResult result;
    result.sessionId = context.sessionId;
    result.timestamp = std::chrono::system_clock::now();
    
    pImpl->context = context;
    pImpl->totalValidations++;
    
    // Validate against all schemas
    for (const auto& schema : pImpl->schemas) {
        if (schema.scope == ConfigurationScope::Global || 
            (context.activeScopes.count(ConfigurationScope::Global) && context.activeScopes.at(ConfigurationScope::Global))) {
            auto schemaResult = validateAgainstSchema(config, schema);
            result.issues.insert(result.issues.end(), schemaResult.issues.begin(), schemaResult.issues.end());
        }
    }
    
    // Validate against all rules
    for (const auto& rule : pImpl->rules) {
        if (rule.scope == ConfigurationScope::Global || 
            (context.activeScopes.count(rule.scope) && context.activeScopes.at(rule.scope))) {
            auto ruleResult = validateRule(rule, config);
            result.issues.insert(result.issues.end(), ruleResult.issues.begin(), ruleResult.issues.end());
        }
    }
    
    // Classify issues
    classifyIssues(result);
    
    // Check if validation passes based on level
    if (pImpl->validationLevel == ValidationLevel::Strict) {
        result.isValid = result.issues.empty() || 
                        std::none_of(result.issues.begin(), result.issues.end(),
                                   [](const ValidationIssue& issue) { return issue.severity == ValidationSeverity::Error; });
    } else if (pImpl->validationLevel == ValidationLevel::Warning) {
        result.isValid = std::none_of(result.issues.begin(), result.issues.end(),
                                   [](const ValidationIssue& issue) { return issue.severity == ValidationSeverity::Error; });
    } else { // Lenient
        result.isValid = true; // Only critical errors matter
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalValidationTime += result.validationTime;
    
    // Store session result
    pImpl->sessionResults[result.sessionId] = result;
    pImpl->sessionTimes[result.sessionId] = result.timestamp;
    
    return result;
}

ValidationResult AdvancedValidator::validate(const std::string& configJson) {
    Config config;
    if (validateConfigJson(configJson, config)) {
        return validate(config);
    }
    
    ValidationResult result;
    result.isValid = false;
    result.issues.push_back(ValidationIssue("JSON_VALIDATION", "Invalid JSON configuration", ValidationSeverity::Error));
    return result;
}

// Rule management
void AdvancedValidator::addRule(const ValidationRule& rule) {
    pImpl->rules.push_back(rule);
    
    // Organize rules by type and scope
    pImpl->rulesByType[std::to_string(static_cast<int>(rule.type))].push_back(rule);
    pImpl->rulesByScope[static_cast<int>(rule.scope)].push_back(rule);
}

void AdvancedValidator::removeRule(const std::string& ruleId) {
    auto it = std::remove_if(pImpl->rules.begin(), pImpl->rules.end(),
                           [&ruleId](const ValidationRule& rule) { return rule.id == ruleId; });
    pImpl->rules.erase(it, pImpl->rules.end());
    
    // Remove from organized structures
    for (auto& [type, rules] : pImpl->rulesByType) {
        rules.erase(std::remove_if(rules.begin(), rules.end(),
                                 [&ruleId](const ValidationRule& rule) { return rule.id == ruleId; }),
                   rules.end());
    }
    
    for (auto& [scope, rules] : pImpl->rulesByScope) {
        rules.erase(std::remove_if(rules.begin(), rules.end(),
                                 [&ruleId](const ValidationRule& rule) { return rule.id == ruleId; }),
                   rules.end());
    }
}

void AdvancedValidator::clearRules() {
    pImpl->rules.clear();
    pImpl->rulesByType.clear();
    pImpl->rulesByScope.clear();
}

std::vector<ValidationRule> AdvancedValidator::getRules() const {
    return pImpl->rules;
}

// Schema management
void AdvancedValidator::addSchema(const ConfigurationSchema& schema) {
    pImpl->schemas.push_back(schema);
    pImpl->schemasByName[schema.name] = schema;
    pImpl->schemasByScope[static_cast<int>(schema.scope)].push_back(schema);
}

void AdvancedValidator::removeSchema(const std::string& schemaName) {
    auto it = std::remove_if(pImpl->schemas.begin(), pImpl->schemas.end(),
                           [&schemaName](const ConfigurationSchema& schema) { return schema.name == schemaName; });
    if (it != pImpl->schemas.end()) {
        ConfigurationSchema removed = *it;
        pImpl->schemas.erase(it);
        pImpl->schemasByName.erase(removed.name);
        
        for (auto& [scope, schemas] : pImpl->schemasByScope) {
            schemas.erase(std::remove_if(schemas.begin(), schemas.end(),
                                       [&schemaName](const ConfigurationSchema& schema) { return schema.name == schemaName; }),
                       schemas.end());
        }
    }
}

void AdvancedValidator::clearSchemas() {
    pImpl->schemas.clear();
    pImpl->schemasByName.clear();
    pImpl->schemasByScope.clear();
}

std::vector<ConfigurationSchema> AdvancedValidator::getSchemas() const {
    return pImpl->schemas;
}

// Validation context management
void AdvancedValidator::setContext(const ValidationContext& context) {
    pImpl->context = context;
}

ValidationContext AdvancedValidator::getContext() const {
    return pImpl->context;
}

// Validation options
void AdvancedValidator::setValidationLevel(ValidationLevel level) {
    pImpl->validationLevel = level;
}

ValidationLevel AdvancedValidator::getValidationLevel() const {
    return pImpl->validationLevel;
}

void AdvancedValidator::enableAutoRepair(bool enable) {
    pImpl->autoRepairEnabled = enable;
}

bool AdvancedValidator::isAutoRepairEnabled() const {
    return pImpl->autoRepairEnabled;
}

// Statistics
Size AdvancedValidator::getTotalRules() const {
    return pImpl->rules.size();
}

Size AdvancedValidator::getTotalIssues() const {
    size_t total = 0;
    for (const auto& session : pImpl->sessionResults) {
        total += session.second.issues.size();
    }
    return total;
}

Size AdvancedValidator::getErrorsCount() const {
    size_t count = 0;
    for (const auto& session : pImpl->sessionResults) {
        for (const auto& issue : session.second.issues) {
            if (issue.severity == ValidationSeverity::Error) {
                count++;
            }
        }
    }
    return count;
}

Size AdvancedValidator::getWarningsCount() const {
    size_t count = 0;
    for (const auto& session : pImpl->sessionResults) {
        for (const auto& issue : session.second.issues) {
            if (issue.severity == ValidationSeverity::Warning) {
                count++;
            }
        }
    }
    return count;
}

Size AdvancedValidator::getValidationTime() const {
    return pImpl->totalValidationTime;
}

// Configuration validation methods
ValidationResult AdvancedValidator::validateConfig(const Config& config) const {
    ValidationResult result;
    
    for (const auto& rule : pImpl->rules) {
        auto ruleResult = validateRule(rule, config);
        result.issues.insert(result.issues.end(), ruleResult.issues.begin(), ruleResult.issues.end());
    }
    
    classifyIssues(result);
    return result;
}

ValidationResult AdvancedValidator::validateConfig(const Config& config, const ConfigurationSchema& schema) const {
    return validateAgainstSchema(config, schema);
}

// Advanced validation methods
ValidationResult AdvancedValidator::validateDependencies(const Config& config) const {
    ValidationResult result;
    
    for (const auto& rule : pImpl->rules) {
        if (rule.type == ValidationRuleType::Dependency) {
            auto ruleResult = validateRule(rule, config);
            result.issues.insert(result.issues.end(), ruleResult.issues.begin(), ruleResult.issues.end());
        }
    }
    
    return result;
}

ValidationResult AdvancedValidator::validateTypes(const Config& config) const {
    ValidationResult result;
    
    for (const auto& rule : pImpl->rules) {
        if (rule.type == ValidationRuleType::Type) {
            auto ruleResult = validateRule(rule, config);
            result.issues.insert(result.issues.end(), ruleResult.issues.begin(), ruleResult.issues.end());
        }
    }
    
    return result;
}

ValidationResult AdvancedValidator::validateRanges(const Config& config) const {
    ValidationResult result;
    
    for (const auto& rule : pImpl->rules) {
        if (rule.type == ValidationRuleType::Range) {
            auto ruleResult = validateRule(rule, config);
            result.issues.insert(result.issues.end(), ruleResult.issues.begin(), ruleResult.issues.end());
        }
    }
    
    return result;
}

ValidationResult AdvancedValidator::validatePatterns(const Config& config) const {
    ValidationResult result;
    
    for (const auto& rule : pImpl->rules) {
        if (rule.type == ValidationRuleType::Pattern) {
            auto ruleResult = validateRule(rule, config);
            result.issues.insert(result.issues.end(), ruleResult.issues.begin(), ruleResult.issues.end());
        }
    }
    
    return result;
}

ValidationResult AdvancedValidator::validateSchemas(const Config& config) const {
    ValidationResult result;
    
    for (const auto& schema : pImpl->schemas) {
        auto schemaResult = validateAgainstSchema(config, schema);
        result.issues.insert(result.issues.end(), schemaResult.issues.begin(), schemaResult.issues.end());
    }
    
    return result;
}

// Repair methods
bool AdvancedValidator::attemptAutoRepair(Config& config) const {
    if (!pImpl->autoRepairEnabled) {
        return false;
    }
    
    bool repaired = false;
    
    // Attempt to fix each issue
    for (const auto& rule : pImpl->rules) {
        for (auto& issue : pImpl->context.suggestedFixes) {
            if (attemptAutoFix(issue, config)) {
                repaired = true;
            }
        }
    }
    
    return repaired;
}

std::vector<ValidationIssue> AdvancedValidator::suggestFixes(const Config& config) const {
    std::vector<ValidationIssue> fixes;
    
    for (const auto& rule : pImpl->rules) {
        auto ruleResult = validateRule(rule, config);
        fixes.insert(fixes.end(), ruleResult.issues.begin(), ruleResult.issues.end());
    }
    
    return fixes;
}

// Configuration management
Config AdvancedValidator::applyFixes(const Config& config) const {
    Config fixed = config;
    // Implementation would apply fixes
    return fixed;
}

Config AdvancedValidator::applySchemaFixes(const Config& config, const ConfigurationSchema& schema) const {
    Config fixed = config;
    // Implementation would apply schema fixes
    return fixed;
}

// Export/import
bool AdvancedValidator::exportRules(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonRules;
    for (const auto& rule : pImpl->rules) {
        nlohmann::json jsonRule;
        jsonRule["id"] = rule.id;
        jsonRule["name"] = rule.name;
        jsonRule["type"] = static_cast<int>(rule.type);
        jsonRule["scope"] = static_cast<int>(rule.scope);
        jsonRule["severity"] = static_cast<int>(rule.severity);
        jsonRule["description"] = rule.description;
        jsonRule["isRequired"] = rule.isRequired;
        jsonRule["errorMessage"] = rule.errorMessage;
        jsonRule["warningMessage"] = rule.warningMessage;
        jsonRule["hintMessage"] = rule.hintMessage;
        
        jsonRule["parameters"] = rule.parameters;
        jsonRule["tags"] = rule.tags;
        jsonRule["metadata"] = rule.metadata;
        
        jsonRules.push_back(jsonRule);
    }
    
    file << jsonRules.dump(2);
    return true;
}

bool AdvancedValidator::importRules(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonRules = nlohmann::json::parse(file);
        
        clearRules();
        
        for (const auto& jsonRule : jsonRules) {
            ValidationRule rule;
            rule.id = jsonRule["id"];
            rule.name = jsonRule["name"];
            rule.type = static_cast<ValidationRuleType>(jsonRule["type"]);
            rule.scope = static_cast<ConfigurationScope>(jsonRule["scope"]);
            rule.severity = static_cast<ValidationSeverity>(jsonRule["severity"]);
            rule.description = jsonRule["description"];
            rule.isRequired = jsonRule["isRequired"];
            rule.errorMessage = jsonRule["errorMessage"];
            rule.warningMessage = jsonRule["warningMessage"];
            rule.hintMessage = jsonRule["hintMessage"];
            
            rule.parameters = jsonRule["parameters"].get<std::unordered_map<std::string, std::string>>();
            rule.tags = jsonRule["tags"].get<std::vector<std::string>>();
            rule.metadata = jsonRule["metadata"].get<std::unordered_map<std::string, std::string>>();
            
            addRule(rule);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedValidator::exportSchemas(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonSchemas;
    for (const auto& schema : pImpl->schemas) {
        nlohmann::json jsonSchema;
        jsonSchema["name"] = schema.name;
        jsonSchema["description"] = schema.description;
        jsonSchema["version"] = schema.version;
        jsonSchema["scope"] = static_cast<int>(schema.scope);
        
        // Add rules
        nlohmann::json jsonRules;
        for (const auto& rule : schema.rules) {
            nlohmann::json jsonRule;
            jsonRule["id"] = rule.id;
            jsonRule["name"] = rule.name;
            jsonRule["type"] = static_cast<int>(rule.type);
            jsonRule["severity"] = static_cast<int>(rule.severity);
            jsonRule["description"] = rule.description;
            jsonRule["isRequired"] = rule.isRequired;
            jsonRule["errorMessage"] = rule.errorMessage;
            jsonRule["warningMessage"] = rule.warningMessage;
            jsonRule["hintMessage"] = rule.hintMessage;
            
            jsonRules.push_back(jsonRule);
        }
        jsonSchema["rules"] = jsonRules;
        
        jsonSchemas.push_back(jsonSchema);
    }
    
    file << jsonSchemas.dump(2);
    return true;
}

bool AdvancedValidator::importSchemas(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonSchemas = nlohmann::json::parse(file);
        
        clearSchemas();
        
        for (const auto& jsonSchema : jsonSchemas) {
            ConfigurationSchema schema;
            schema.name = jsonSchema["name"];
            schema.description = jsonSchema["description"];
            schema.version = jsonSchema["version"];
            schema.scope = static_cast<ConfigurationScope>(jsonSchema["scope"]);
            
            // Add rules
            for (const auto& jsonRule : jsonSchema["rules"]) {
                ValidationRule rule;
                rule.id = jsonRule["id"];
                rule.name = jsonRule["name"];
                rule.type = static_cast<ValidationRuleType>(jsonRule["type"]);
                rule.severity = static_cast<ValidationSeverity>(jsonRule["severity"]);
                rule.description = jsonRule["description"];
                rule.isRequired = jsonRule["isRequired"];
                rule.errorMessage = jsonRule["errorMessage"];
                rule.warningMessage = jsonRule["warningMessage"];
                rule.hintMessage = jsonRule["hintMessage"];
                
                schema.rules.push_back(rule);
            }
            
            addSchema(schema);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Schema utilities
ConfigurationSchema AdvancedValidator::createBrainSchema() {
    ConfigurationSchema schema;
    schema.name = "Brain Configuration";
    schema.description = "Schema for brain configuration parameters";
    schema.version = "1.0.0";
    schema.scope = ConfigurationScope::Brain;
    
    // Add essential brain rules
    auto neuronCountRule = createRequiredRule("neuron_count", "Number of neurons in the brain");
    auto regionCountRule = createTypeRule("region_count", "integer", "Number of brain regions");
    auto connectionProbabilityRule = createRangeRule("connection_probability", "0.0", "1.0", "Connection probability for random connectivity");
    
    schema.rules.push_back(neuronCountRule);
    schema.rules.push_back(regionCountRule);
    schema.rules.push_back(connectionProbabilityRule);
    
    return schema;
}

ConfigurationSchema AdvancedValidator::createWorldSchema() {
    ConfigurationSchema schema;
    schema.name = "World Configuration";
    schema.description = "Schema for world configuration parameters";
    schema.version = "1.0.0";
    schema.scope = ConfigurationScope::World;
    
    // Add essential world rules
    auto widthRule = createRequiredRule("width", "World width");
    auto heightRule = createTypeRule("height", "integer", "World height");
    auto visionWidthRule = createRangeRule("vision_width", "0", "100", "Vision width");
    auto visionHeightRule = createRangeRule("vision_height", "0", "100", "Vision height");
    
    schema.rules.push_back(widthRule);
    schema.rules.push_back(heightRule);
    schema.rules.push_back(visionWidthRule);
    schema.rules.push_back(visionHeightRule);
    
    return schema;
}

ConfigurationSchema AdvancedValidator::createAgentSchema() {
    ConfigurationSchema schema;
    schema.name = "Agent Configuration";
    schema.description = "Schema for agent configuration parameters";
    schema.version = "1.0.0";
    schema.scope = ConfigurationScope::Agent;
    
    // Add essential agent rules
    auto rewardModulationRule = createRequiredRule("enable_reward_modulation", "Enable reward modulation");
    auto curiosityRule = createRequiredRule("enable_curiosity", "Enable curiosity-driven behavior");
    auto developmentRule = createRequiredRule("enable_development", "Enable developmental processes");
    
    schema.rules.push_back(rewardModulationRule);
    schema.rules.push_back(curiosityRule);
    schema.rules.push_back(developmentRule);
    
    return schema;
}

ConfigurationSchema AdvancedValidator::createSystemSchema() {
    ConfigurationSchema schema;
    schema.name = "System Configuration";
    schema.description = "Schema for system configuration parameters";
    schema.version = "1.0.0";
    schema.scope = ConfigurationScope::System;
    
    // Add essential system rules
    auto randomSeedRule = createTypeRule("random_seed", "integer", "Random seed for reproducibility");
    auto simulationTimestepRule = createTypeRule("simulation_timestep", "float", "Simulation timestep");
    auto logLevelRule = createTypeRule("log_level", "string", "Log level");
    
    schema.rules.push_back(randomSeedRule);
    schema.rules.push_back(simulationTimestepRule);
    schema.rules.push_back(logLevelRule);
    
    return schema;
}

ConfigurationSchema AdvancedValidator::createPhase6Schema() {
    ConfigurationSchema schema;
    schema.name = "Phase 6 Integration Schema";
    schema.description = "Schema for Phase 6 integrated brain configuration";
    schema.version = "1.0.0";
    schema.scope = ConfigurationScope::Component;
    
    // Add Phase 6-specific rules
    auto neuronCountRule = createRangeRule("neuron_count", "100", "10000", "Number of neurons (100-10000)");
    auto enableCheckpointingRule = createRequiredRule("enable_checkpointing", "Enable checkpointing");
    auto enableReplayRule = createRequiredRule("enable_replay", "Enable memory replay");
    auto enableDevelopmentRule = createRequiredRule("enable_development", "Enable developmental processes");
    auto replayIntervalRule = createRangeRule("replay_interval", "1", "1000", "Replay interval in steps");
    auto consolidationIntervalRule = createRangeRule("consolidation_interval", "100", "10000", "Consolidation interval in steps");
    
    schema.rules.push_back(neuronCountRule);
    schema.rules.push_back(enableCheckpointingRule);
    schema.rules.push_back(enableReplayRule);
    schema.rules.push_back(enableDevelopmentRule);
    schema.rules.push_back(replayIntervalRule);
    schema.rules.push_back(consolidationIntervalRule);
    
    return schema;
}

ConfigurationSchema AdvancedValidator::createDevelopmentSchema() {
    ConfigurationSchema schema;
    schema.name = "Development Configuration";
    schema.description = "Schema for development configuration parameters";
    schema.version = "1.0.0";
    schema.scope = ConfigurationScope::Component;
    
    // Add development-specific rules
    auto initialPlasticityRule = createRangeRule("initial_plasticity", "0.1", "2.0", "Initial plasticity level");
    auto criticalPeriodRule = createRequiredRule("enable_critical_period", "Enable critical period");
    auto maturationRule = createRangeRule("maturation_rate", "0.0", "1.0", "Maturation rate");
    
    schema.rules.push_back(initialPlasticityRule);
    schema.rules.push_back(criticalPeriodRule);
    schema.rules.push_back(maturationRule);
    
    return schema;
}

// Predefined rule sets
void AdvancedValidator::createDefaultRules() {
    // Create default validation rules
    auto requiredRule1 = createRequiredRule("random_seed", "Random seed for reproducible experiments");
    auto typeRule1 = createTypeRule("neuron_count", "integer", "Number of neurons in the brain");
    auto rangeRule1 = createRangeRule("connection_probability", "0.0", "1.0", "Connection probability");
    auto rangeRule2 = createRangeRule("simulation_timestep", "0.0", "1.0", "Simulation timestep");
    
    addRule(requiredRule1);
    addRule(typeRule1);
    addRule(rangeRule1);
    addRule(rangeRule2);
}

void AdvancedValidator::createBrainRules() {
    // Create brain-specific validation rules
    auto brainTypeRule = createTypeRule("brain_type", "string", "Brain type");
    auto neuronTypeRule = createTypeRule("neuron_type", "string", "Neuron type");
    auto connectionRule = createRequiredRule("connections", "Neural connections");
    
    addRule(brainTypeRule);
    addRule(neuronTypeRule);
    addRule(connectionRule);
}

void AdvancedValidator::createWorldRules() {
    // Create world-specific validation rules
    auto worldWidthRule = createTypeRule("width", "integer", "World width");
    auto worldHeightRule = createTypeRule("height", "integer", "World height");
    auto worldVisionRule = createTypeRule("vision_width", "integer", "Vision width");
    
    addRule(worldWidthRule);
    addRule(worldHeightRule);
    addRule(worldVisionRule);
}

void AdvancedValidator::createAgentRules() {
    // Create agent-specific validation rules
    auto agentTypeRule = createTypeRule("agent_type", "string", "Agent type");
    auto agentBehaviorRule = createTypeRule("behavior", "string", "Agent behavior");
    auto agentMemoryRule = createTypeRule("memory_capacity", "integer", "Agent memory capacity");
    
    addRule(agentTypeRule);
    addRule(agentBehaviorRule);
    addRule(agentMemoryRule);
}

void AdvancedValidator::createSystemRules() {
    // Create system-specific validation rules
    auto systemLogRule = createTypeRule("log_level", "string", "Log level");
    auto systemDebugRule = createTypeRule("debug_mode", "boolean", "Debug mode flag");
    auto systemPerformanceRule = createTypeRule("performance_mode", "string", "Performance mode");
    
    addRule(systemLogRule);
    addRule(systemDebugRule);
    addRule(systemPerformanceRule);
}

void AdvancedValidator::createNeuromodulationRules() {
    // Create neuromodulation-specific validation rules
    auto dopamineRule = createRangeRule("dopamine_level", "0.0", "10.0", "Dopamine level");
    auto curiosityRule = createRangeRule("curiosity_level", "0.0", "1.0", "Curiosity level");
    auto noveltyRule = createRangeRule("novelty_level", "0.0", "1.0", "Novelty level");
    
    addRule(dopamineRule);
    addRule(curiosityRule);
    addRule(noveltyRule);
}

void AdvancedValidator::createMemoryRules() {
    // Create memory-specific validation rules
    auto workingMemoryRule = createTypeRule("working_memory_size", "integer", "Working memory size");
    auto episodicMemoryRule = createTypeRule("episodic_memory_capacity", "integer", "Episodic memory capacity");
    auto associativeMemoryRule = createTypeRule("associative_memory_strength", "float", "Associative memory strength");
    
    addRule(workingMemoryRule);
    addRule(episodicMemoryRule);
    addRule(associativeMemoryRule);
}

void AdvancedValidator::createPredictionRules() {
    // Create prediction-specific validation rules
    auto predictionAccuracyRule = createRangeRule("prediction_accuracy", "0.0", "1.0", "Prediction accuracy");
    auto predictionConfidenceRule = createRangeRule("prediction_confidence", "0.0", "1.0", "Prediction confidence");
    auto predictionLatencyRule = createTypeRule("prediction_latency", "float", "Prediction latency");
    
    addRule(predictionAccuracyRule);
    addRule(predictionConfidenceRule);
    addRule(predictionLatencyRule);
}

void AdvancedValidator::createCognitionRules() {
    // Create cognition-specific validation rules
    auto cognitionRule = createTypeRule("cognitive_load", "float", "Cognitive load");
    auto attentionRule = createRangeRule("attention_span", "0.0", "1.0", "Attention span");
    auto planningRule = createTypeRule("planning_depth", "integer", "Planning depth");
    
    addRule(cognitionRule);
    addRule(attentionRule);
    addRule(planningRule);
}

void AdvancedValidator::createDevelopmentRules() {
    // Create development-specific validation rules
    auto developmentRule = createTypeRule("developmental_stage", "string", "Developmental stage");
    auto plasticityRule = createRangeRule("plasticity_rate", "0.0", "1.0", "Plasticity rate");
    auto maturationRule = createTypeRule("maturation_progress", "float", "Maturation progress");
    
    addRule(developmentRule);
    addRule(plasticityRule);
    addRule(maturationRule);
}

// Statistics and analysis
std::unordered_map<std::string, size_t> AdvancedValidator::getRuleStatistics() const {
    std::unordered_map<std::string, size_t> stats;
    
    for (const auto& rule : pImpl->rules) {
        stats[std::to_string(static_cast<int>(rule.type))]++;
    }
    
    return stats;
}

std::unordered_map<std::string, size_t> AdvancedValidator::getSchemaStatistics() const {
    std::unordered_map<std::string, size_t> stats;
    
    for (const auto& schema : pImpl->schemas) {
        stats[std::to_string(static_cast<int>(schema.scope))]++;
    }
    
    return stats;
}

std::string AdvancedValidator::getValidationSummary() const {
    std::ostringstream oss;
    oss << "Validation Summary:" << std::endl;
    oss << "  Total Rules: " << pImpl->rules.size() << std::endl;
    oss << "  Total Schemas: " << pImpl->schemas.size() << std::endl;
    oss << "  Total Validations: " << pImpl->totalValidations << std::endl;
    oss << "  Total Errors: " << pImpl->totalErrors << std::endl;
    oss << "  Total Warnings: " << pImpl->totalWarnings << std::endl;
    oss << "  Average Validation Time: " << (pImpl->totalValidations > 0 ? pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    
    return oss.str();
}

// Report generation
std::string AdvancedValidator::generateValidationReport(const ValidationResult& result) const {
    std::ostringstream oss;
    oss << "=== VALIDATION REPORT ===" << std::endl;
    oss << "Session ID: " << result.sessionId << std::endl;
    oss << "Timestamp: " << LoggerFactory::timestampToString(result.timestamp) << std::endl;
    oss << "Overall Valid: " << (result.isValid ? "YES" : "NO") << std::endl;
    oss << "Total Issues: " << result.issues.size() << std::endl;
    oss << "Validation Time: " << result.validationTime << "s" << std::endl;
    oss << "" << std::endl;
    
    for (const auto& issue : result.issues) {
        oss << "[" << (issue.severity == ValidationSeverity::Error ? "ERROR" : 
                     issue.severity == ValidationSeverity::Warning ? "WARNING" : "INFO") << "]";
        oss << " Rule: " << issue.ruleName << std::endl;
        oss << "  Key: " << issue.key << std::endl;
        oss << "  Message: " << issue.message << std::endl;
        if (!issue.value.empty()) oss << "  Value: " << issue.value << std::endl;
        if (!issue.expected.empty()) oss << "  Expected: " << issue.expected << std::endl;
        oss << "  File: " << issue.file << ":" << issue.line << std::endl;
        if (!issue.function.empty()) oss << "  Function: " << issue.function << std::endl;
        oss << "" << std::endl;
    }
    
    return oss.str();
}

std::string AdvancedValidator::generateSchemaReport(const ConfigurationSchema& schema) const {
    std::ostringstream oss;
    oss << "=== SCHEMA REPORT ===" << std::endl;
    oss << "Name: " << schema.name << std::endl;
    oss << "Description: " << schema.description << std::endl;
    oss << "Version: " << schema.version << std::endl;
    oss << "Scope: " << static_cast<int>(schema.scope) << std::endl;
    oss << "Required Keys: " << schema.requiredKeys.size() << std::endl;
    oss << "Forbidden Keys: " << schema.forbiddenKeys.size() << std::endl;
    oss << "Min Keys: " << schema.minKeys << std::endl;
    oss << "Max Keys: " << schema.maxKeys << std::endl;
    oss << "Validation Level: " << static_cast<int>(schema.validationLevel) << std::endl;
    oss << "Allow Unknown Keys: " << (schema.allowUnknownKeys ? "YES" : "NO") << std::endl;
    oss << "Strict Type Checking: " << (schema.strictTypeChecking ? "YES" : "NO") << std::endl;
    oss << "" << std::endl;
    
    return oss.str();
}

std::string AdvancedValidator::generateRuleReport(const ValidationRule& rule) const {
    std::ostringstream oss;
    oss << "=== RULE REPORT ===" << std::endl;
    oss << "ID: " << rule.id << std::endl;
    oss << "Name: " << rule.name << std::endl;
    oss << "Type: " << static_cast<int>(rule.type) << std::endl;
    oss << "Scope: " << static_cast<int>(rule.scope) << std::endl;
    oss << "Severity: " << static_cast<int>(rule.severity) << std::endl;
    oss << "Description: " << rule.description << std::endl;
    oss << "Is Required: " << (rule.isRequired ? "YES" : "NO") << std::endl;
    oss << "Error Message: " << rule.errorMessage << std::endl;
    oss << "Warning Message: " << rule.warningMessage << std::endl;
    oss << "Hint Message: " << rule.hintMessage << std::endl;
    oss << "Tags: ";
    for (size_t i = 0; i < rule.tags.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << rule.tags[i];
    }
    oss << std::endl;
    
    return oss.str();
}

} // namespace nlm
