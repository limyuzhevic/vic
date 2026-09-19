#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "ConfigSchema.hpp"

namespace nlm {

// Forward declarations
enum class ConfigErrorType;
struct ConfigError;
enum class ParameterType;

// Configuration graph for dependency analysis
struct ConfigGraphNode {
    std::string name;
    std::vector<std::string> dependencies;
    std::vector<std::string> dependents;
    bool visited = false;
    bool inStack = false;
};

// Configuration dependency analyzer
class ConfigDependencyAnalyzer {
public:
    ConfigDependencyAnalyzer() = default;
    
    // Build dependency graph from schema
    std::map<std::string, ConfigGraphNode> buildDependencyGraph(const ConfigSchema& schema) const;
    
    // Check for dependency cycles
    std::vector<std::vector<std::string>> findCycles(const std::map<std::string, ConfigGraphNode>& graph) const;
    
    // Get topological ordering
    std::vector<std::string> getTopologicalOrder(const std::map<std::string, ConfigGraphNode>& graph) const;
    
    // Find parameter dependencies
    static std::vector<std::string> findParameterDependencies(const ParameterInfo& param, 
                                                              const std::map<std::string, ParameterInfo>& paramMap) const;
    
private:
    void dfs(const std::string& node, const std::map<std::string, ConfigGraphNode>& graph,
             std::vector<std::string>& path, std::vector<std::vector<std::string>>& cycles) const;
    void topologicalDfs(const std::string& node, const std::map<std::string, ConfigGraphNode>& graph,
                       std::unordered_set<std::string>& visited, std::vector<std::string>& order) const;
};

// Configuration documentation system
class ConfigDocumentation {
public:
    ConfigDocumentation() = default;
    
    // Generate comprehensive documentation
    std::string generateDocumentation(const ConfigSchema& schema) const;
    
    // Generate usage examples
    std::string generateExamples(const ConfigSchema& schema) const;
    
    // Generate schema summary
    std::string generateSchemaSummary(const ConfigSchema& schema) const;
    
    // Generate parameter documentation
    std::string generateParameterDocumentation(const ParameterInfo& param) const;
    
private:
    std::string formatType(const ParameterType& type) const;
    std::string formatDefault(const std::string& defaultValue, const ParameterType& type) const;
};

// Configuration validation utilities
class ConfigValidationUtils {
public:
    ConfigValidationUtils() = default;
    
    // Sanitize configuration value
    std::any sanitizeValue(const std::any& value, const ParameterType& expectedType) const;
    
    // Validate configuration value
    bool isValidValue(const std::any& value, const ParameterInfo& param) const;
    
    // Get default value for parameter
    std::any getDefaultValue(const ParameterInfo& param) const;
    
    // Convert string to any
    std::any stringToAny(const std::string& str, const ParameterType& type) const;
    
    // Convert any to string
    std::string anyToString(const std::any& value) const;
    
    // Check if value matches type
    bool valueMatchesType(const std::any& value, const ParameterType& type) const;
    
private:
    std::any convertToExactType(const std::any& value, const ParameterType& type) const;
    bool validateRangeForAny(const std::any& value, const RangeSpec& range) const;
    bool validateChoiceForAny(const std::any& value, const ChoiceSpec& choice) const;
    double getDoubleValue(const std::any& value) const;
};

// Enhanced error reporter with context
class ConfigErrorReporter {
public:
    ConfigErrorReporter() = default;
    
    // Report error with context
    std::string reportError(const ConfigError& error, const ConfigSchema* schema = nullptr) const;
    
    // Report validation errors
    std::string reportValidationErrors(const std::vector<ConfigError>& errors, 
                                       const ConfigSchema* schema = nullptr) const;
    
    // Format error details
    std::string formatErrorDetails(const ConfigError& error) const;
    
    // Suggest fixes
    std::string suggestFixes(const ConfigError& error, const ConfigSchema* schema = nullptr) const;
    
private:
    std::string getErrorTypeName(ConfigErrorType type) const;
    std::string getParameterTypeName(ParameterType type) const;
};

// Configuration help system
class ConfigHelpSystem {
public:
    ConfigHelpSystem() = default;
    
    // Get help for parameter
    std::string getParameterHelp(const std::string& parameterName, const ConfigSchema& schema) const;
    
    // Get schema help
    std::string getSchemaHelp(const ConfigSchema& schema) const;
    
    // Get preset help
    std::string getPresetHelp(const std::string& presetName) const;
    
    // List available presets
    std::vector<std::string> listAvailablePresets() const;
    
private:
    std::string getQuickPresetHelp() const;
    std::string getBalancedPresetHelp() const;
    std::string getResearchPresetHelp() const;
    std::string getMinimalPresetHelp() const;
    std::string getMaximumPresetHelp() const;
    std::string getParameterTypeName(ParameterType type) const;
};

} // namespace nlm