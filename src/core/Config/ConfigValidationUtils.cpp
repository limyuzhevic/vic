#include "ConfigValidationUtils.hpp"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// ConfigDependencyAnalyzer implementation

std::map<std::string, ConfigGraphNode> ConfigDependencyAnalyzer::buildDependencyGraph(const ConfigSchema& schema) const {
    std::map<std::string, ConfigGraphNode> graph;
    std::map<std::string, ParameterInfo> paramMap;
    
    // Create nodes from schema parameters
    for (const auto& param : schema.getParameters()) {
        paramMap[param.name] = param;
        graph[param.name] = ConfigGraphNode{param.name, {}, {}};
    }
    
    // Build dependency relationships
    for (const auto& paramPair : paramMap) {
        const auto& param = paramPair.second;
        
        if (param.dependency) {
            if (paramMap.find(param.dependency->name) != paramMap.end()) {
                graph[param.name].dependencies.push_back(param.dependency->name);
                graph[param.dependency->name].dependents.push_back(param.name);
            }
        }
    }
    
    return graph;
}

std::vector<std::vector<std::string>> ConfigDependencyAnalyzer::findCycles(const std::map<std::string, ConfigGraphNode>& graph) const {
    std::vector<std::vector<std::string>> cycles;
    std::vector<std::string> path;
    
    for (const auto& pair : graph) {
        const std::string& node = pair.first;
        
        if (!pair.second.visited) {
            dfs(node, graph, path, cycles);
        }
    }
    
    return cycles;
}

void ConfigDependencyAnalyzer::dfs(const std::string& node, const std::map<std::string, ConfigGraphNode>& graph,
                                   std::vector<std::string>& path, std::vector<std::vector<std::string>>& cycles) const {
    auto it = graph.find(node);
    if (it == graph.end()) return;
    
    const ConfigGraphNode& graphNode = it->second;
    
    if (graphNode.visited) return;
    
    graphNode.visited = true;
    graphNode.inStack = true;
    path.push_back(node);
    
    for (const std::string& dep : graphNode.dependencies) {
        if (graph.find(dep) == graph.end()) continue;
        
        const ConfigGraphNode& depNode = graph.at(dep);
        
        if (!depNode.visited) {
            dfs(dep, graph, path, cycles);
        } else if (depNode.inStack) {
            // Found a cycle
            std::vector<std::string> cycle;
            auto depIt = path.begin();
            while (depIt != path.end() && *depIt != dep) {
                ++depIt;
            }
            
            if (depIt != path.end()) {
                for (auto it2 = depIt; it2 != path.end(); ++it2) {
                    cycle.push_back(*it2);
                }
                cycle.push_back(dep);
                cycles.push_back(cycle);
            }
        }
    }
    
    path.pop_back();
    graphNode.inStack = false;
}

std::vector<std::string> ConfigDependencyAnalyzer::getTopologicalOrder(const std::map<std::string, ConfigGraphNode>& graph) const {
    std::vector<std::string> order;
    std::unordered_set<std::string> visited;
    
    for (const auto& pair : graph) {
        const std::string& node = pair.first;
        if (visited.find(node) == visited.end()) {
            topologicalDfs(node, graph, visited, order);
        }
    }
    
    return order;
}

void ConfigDependencyAnalyzer::topologicalDfs(const std::string& node, const std::map<std::string, ConfigGraphNode>& graph,
                                               std::unordered_set<std::string>& visited, std::vector<std::string>& order) const {
    auto it = graph.find(node);
    if (it == graph.end()) return;
    
    const ConfigGraphNode& graphNode = it->second;
    
    if (visited.find(node) != visited.end()) return;
    
    visited.insert(node);
    
    for (const std::string& dep : graphNode.dependencies) {
        topologicalDfs(dep, graph, visited, order);
    }
    
    order.push_back(node);
}

std::vector<std::string> ConfigDependencyAnalyzer::findParameterDependencies(const ParameterInfo& param,
                                                                              const std::map<std::string, ParameterInfo>& paramMap) const {
    std::vector<std::string> dependencies;
    
    if (param.dependency) {
        dependencies.push_back(param.dependency->name);
        
        // Recursively find nested dependencies
        auto it = paramMap.find(param.dependency->name);
        if (it != paramMap.end()) {
            std::vector<std::string> nestedDeps = findParameterDependencies(it->second, paramMap);
            dependencies.insert(dependencies.end(), nestedDeps.begin(), nestedDeps.end());
        }
    }
    
    return dependencies;
}

// ConfigDocumentation implementation

std::string ConfigDocumentation::generateDocumentation(const ConfigSchema& schema) const {
    std::ostringstream oss;
    oss << "NLM Configuration System Documentation\n";
    oss << "===================================\n\n";
    
    oss << generateSchemaSummary(schema) << "\n";
    oss << "\n" << generateExamples(schema) << "\n";
    oss << "\n" << generateParameterDocumentation(schema) << "\n";
    
    return oss.str();
}

std::string ConfigDocumentation::generateExamples(const ConfigSchema& schema) const {
    std::ostringstream oss;
    oss << "Examples:\n";
    oss << "--------\n\n";
    
    oss << "1. Quick Preset Configuration:\n";
    oss << "```\n";
    oss << "mode: quick\n";
    oss << "performance: 95\n";
    oss << "memory: 2048\n";
    oss << "threads: 8\n";
    oss << "enable_cuda: false\n";
    oss << "```\n\n";
    
    oss << "2. Balanced Preset Configuration:\n";
    oss << "```\n";
    oss << "mode: balanced\n";
    oss << "performance: 75\n";
    oss << "memory: 8192\n";
    oss << "threads: 8\n";
    oss << "enable_cuda: false\n";
    oss << "verbose: false\n";
    oss << "```\n\n";
    
    oss << "3. Research Preset Configuration:\n";
    oss << "```\n";
    oss << "mode: research\n";
    oss << "performance: 60\n";
    oss << "memory: 32768\n";
    oss << "threads: 16\n";
    oss << "enable_cuda: true\n";
    oss << "verbose: true\n";
    oss << "checkpoint_interval: 1800\n";
    oss << "```\n\n";
    
    return oss.str();
}

std::string ConfigDocumentation::generateSchemaSummary(const ConfigSchema& schema) const {
    std::ostringstream oss;
    oss << "Schema Summary:\n";
    oss << "------------\n";
    oss << "Total Parameters: " << schema.getParameters().size() << "\n";
    oss << "Required Parameters: " << schema.getRequiredParameters().size() << "\n";
    oss << "Categories:\n";
    
    auto categories = std::set<std::string>{};
    for (const auto& param : schema.getParameters()) {
        if (!param.category.empty()) {
            categories.insert(param.category);
        }
    }
    
    for (const auto& category : categories) {
        auto params = schema.getParametersByCategory(category);
        oss << "  - " << category << ": " << params.size() << " parameters\n";
    }
    
    return oss.str();
}

std::string ConfigDocumentation::generateParameterDocumentation(const ConfigSchema& schema) const {
    std::ostringstream oss;
    oss << "Parameter Details:\n";
    oss << "----------------\n\n";
    
    for (const auto& param : schema.getOrderedParameters()) {
        oss << generateParameterDocumentation(param) << "\n";
    }
    
    return oss.str();
}

std::string ConfigDocumentation::generateParameterDocumentation(const ParameterInfo& param) const {
    std::ostringstream oss;
    oss << """Parameter: """ << param.name << """
""";
    if (!param.description.empty()) {
        oss << "Description: " << param.description << "\n";
    }
    oss << "Type: " << formatType(param.type) << "\n";
    if (!param.defaultValue.empty()) {
        oss << "Default: " << formatDefault(param.defaultValue, param.type) << "\n";
    }
    if (param.required) {
        oss << "Required: Yes\n";
    }
    if (param.readOnly) {
        oss << "Read-only: Yes\n";
    }
    if (param.range) {
        oss << "Range: " << param.range->min << " to " << param.range->max;
        if (!param.range->inclusive) oss << " (exclusive)";
        oss << "\n";
    }
    if (param.choice) {
        oss << "Choices: ";
        for (size_t i = 0; i < param.choice->choices.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << param.choice->choices[i];
        }
        oss << "\n";
    }
    if (!param.tags.empty()) {
        oss << "Tags: ";
        for (size_t i = 0; i < param.tags.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << param.tags[i];
        }
        oss << "\n";
    }
    if (!param.category.empty()) {
        oss << "Category: " << param.category << "\n";
    }
    if (!param.unit.empty()) {
        oss << "Unit: " << param.unit << "\n";
    }
    
    return oss.str();
}

std::string ConfigDocumentation::formatType(const ParameterType& type) const {
    switch (type) {
        case ParameterType::Int: return "int";
        case ParameterType::Int64: return "int64_t";
        case ParameterType::Float: return "float";
        case ParameterType::Double: return "double";
        case ParameterType::Bool: return "bool";
        case ParameterType::String: return "string";
        case ParameterType::StringList: return "string[]";
        case ParameterType::IntList: return "int[]";
        case ParameterType::FloatList: return "float[]";
        case ParameterType::Choice: return "choice";
        case ParameterType::Range: return "range";
        case ParameterType::Vector2D: return "vector2d";
        case ParameterType::Vector3D: return "vector3d";
        case ParameterType::Matrix: return "matrix";
        case ParameterType::Custom: return "custom";
        default: return "unknown";
    }
}

std::string ConfigDocumentation::formatDefault(const std::string& defaultValue, const ParameterType& type) const {
    if (type == ParameterType::String) {
        return "\"" + defaultValue + "\"";
    }
    return defaultValue;
}

// ConfigValidationUtils implementation

std::any ConfigValidationUtils::sanitizeValue(const std::any& value, const ParameterType& expectedType) const {
    return convertToExactType(value, expectedType);
}

bool ConfigValidationUtils::isValidValue(const std::any& value, const ParameterInfo& param) const {
    if (valueMatchesType(value, param.type)) {
        if (param.range) {
            if (!validateRangeForAny(value, param.range.value())) return false;
        }
        if (param.choice && param.type == ParameterType::String) {
            if (!validateChoiceForAny(value, param.choice.value())) return false;
        }
        return true;
    }
    return false;
}

std::any ConfigValidationUtils::getDefaultValue(const ParameterInfo& param) const {
    return stringToAny(param.defaultValue, param.type);
}

std::any ConfigValidationUtils::stringToAny(const std::string& str, const ParameterType& type) const {
    if (type == ParameterType::String) {
        return str;
    } else if (type == ParameterType::Int) {
        try { return std::stoi(str); } catch (...) { return 0; }
    } else if (type == ParameterType::Double) {
        try { return std::stod(str); } catch (...) { return 0.0; }
    } else if (type == ParameterType::Bool) {
        return (str == "true" || str == "1" || str == "yes");
    } else if (type == ParameterType::Float) {
        try { return static_cast<float>(std::stod(str)); } catch (...) { return 0.0f; }
    }
    
    return str;
}

std::string ConfigValidationUtils::anyToString(const std::any& value) const {
    if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    } else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    } else if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    }
    
    return "";
}

bool ConfigValidationUtils::valueMatchesType(const std::any& value, const ParameterType& type) const {
    switch (type) {
        case ParameterType::Int: return value.type() == typeid(int);
        case ParameterType::Double: return value.type() == typeid(double) || value.type() == typeid(float);
        case ParameterType::Bool: return value.type() == typeid(bool);
        case ParameterType::String: return value.type() == typeid(std::string);
        case ParameterType::StringList: return value.type() == typeid(std::vector<std::string>);
        case ParameterType::IntList: return value.type() == typeid(std::vector<int>);
        case ParameterType::FloatList: return value.type() == typeid(std::vector<float>);
        case ParameterType::Choice: return value.type() == typeid(std::string);
        case ParameterType::Range: return value.type() == typeid(double) || value.type() == typeid(int);
        case ParameterType::Vector2D: return value.type() == typeid(std::vector<double>);
        case ParameterType::Vector3D: return value.type() == typeid(std::vector<double>);
        case ParameterType::Matrix: return value.type() == typeid(std::vector<std::vector<double>>);
        case ParameterType::Custom: return true;
        default: return false;
    }
}

std::any ConfigValidationUtils::convertToExactType(const std::any& value, const ParameterType& type) const {
    if (valueMatchesType(value, type)) return value;
    
    if (value.type() == typeid(int)) {
        int val = std::any_cast<int>(value);
        if (type == ParameterType::Double) return static_cast<double>(val);
        if (type == ParameterType::Int64) return static_cast<int64_t>(val);
        if (type == ParameterType::Float) return static_cast<float>(val);
    } else if (value.type() == typeid(double)) {
        double val = std::any_cast<double>(value);
        if (type == ParameterType::Int) return static_cast<int>(val);
        if (type == ParameterType::Int64) return static_cast<int64_t>(val);
        if (type == ParameterType::Float) return static_cast<float>(val);
    } else if (value.type() == typeid(float)) {
        float val = std::any_cast<float>(value);
        if (type == ParameterType::Double) return static_cast<double>(val);
        if (type == ParameterType::Int) return static_cast<int>(val);
    }
    
    return value;
}

bool ConfigValidationUtils::validateRangeForAny(const std::any& value, const RangeSpec& range) const {
    try {
        double val = getDoubleValue(value);
        double min = parseDouble(range.min);
        double max = parseDouble(range.max);
        
        if (range.inclusive) {
            return val >= min && val <= max;
        } else {
            return val > min && val < max;
        }
    } catch (...) {
        return false;
    }
}

bool ConfigValidationUtils::validateChoiceForAny(const std::any& value, const ChoiceSpec& choice) const {
    if (value.type() != typeid(std::string)) return false;
    
    const std::string& strValue = std::any_cast<const std::string&>(value);
    for (const auto& option : choice.choices) {
        if (strValue == option) return true;
    }
    return false;
}

// ConfigErrorReporter implementation

std::string ConfigErrorReporter::reportError(const ConfigError& error, const ConfigSchema* schema) const {
    std::ostringstream oss;
    oss << "ERROR: " << getErrorTypeName(error.type) << "\n";
    oss << "Key: " << error.key << "\n";
    oss << "Expected: " << error.expected << "\n";
    oss << "Actual: " << error.actual << "\n";
    oss << "Message: " << error.message << "\n";
    
    if (schema) {
        const ParameterInfo* param = schema->getParameter(error.key);
        if (param) {
            oss << "Parameter Info:\n";
            oss << "  Type: " << getParameterTypeName(param->type) << "\n";
            if (param->range) {
                oss << "  Range: " << param->range->min << " to " << param->range->max << "\n";
            }
            if (param->choice) {
                oss << "  Choices: ";
                for (size_t i = 0; i < param->choice->choices.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << param->choice->choices[i];
                }
                oss << "\n";
            }
            oss << "  Description: " << param->description << "\n";
        }
    }
    
    return oss.str();
}

std::string ConfigErrorReporter::reportValidationErrors(const std::vector<ConfigError>& errors, 
                                                         const ConfigSchema* schema) const {
    std::ostringstream oss;
    oss << "Validation Report:\n";
    oss << "==================\n\n";
    
    for (size_t i = 0; i < errors.size(); ++i) {
        oss << "Error " << i + 1 << ":\n";
        oss << reportError(errors[i], schema) << "\n";
        if (i < errors.size() - 1) oss << "\n";
    }
    
    return oss.str();
}

std::string ConfigErrorReporter::formatErrorDetails(const ConfigError& error) const {
    std::ostringstream oss;
    oss << "[" << getErrorTypeName(error.type) << "] " << error.key << ": " << error.message;
    if (!error.expected.empty()) {
        oss << " (expected: " << error.expected << ")";
    }
    if (!error.actual.empty()) {
        oss << " (actual: " << error.actual << ")";
    }
    return oss.str();
}

std::string ConfigErrorReporter::suggestFixes(const ConfigError& error, const ConfigSchema* schema) const {
    std::ostringstream oss;
    oss << "Suggestions:\n";
    
    if (error.type == ConfigErrorType::TypeMismatch) {
        oss << "  - Change the value to the correct type (" << error.expected << ")\n";
        if (schema) {
            const ParameterInfo* param = schema->getParameter(error.key);
            if (param && param->type == ParameterType::Choice) {
                oss << "  - Use one of the valid choices: ";
                for (size_t i = 0; i < param->choice->choices.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << param->choice->choices[i];
                }
                oss << "\n";
            }
        }
    } else if (error.type == ConfigErrorType::RangeViolation) {
        oss << "  - Ensure the value is within the allowed range (" << error.expected << ")\n";
    } else if (error.type == ConfigErrorType::MissingDependency) {
        oss << "  - Add the missing required parameter\n";
    }
    
    return oss.str();
}

std::string ConfigErrorReporter::getErrorTypeName(ConfigErrorType type) const {
    switch (type) {
        case ConfigErrorType::TypeMismatch: return "Type Mismatch";
        case ConfigErrorType::RangeViolation: return "Range Violation";
        case ConfigErrorType::MissingDependency: return "Missing Dependency";
        case ConfigErrorType::DependencyCycle: return "Dependency Cycle";
        case ConfigErrorType::SchemaValidation: return "Schema Validation";
        case ConfigErrorType::MigrationError: return "Migration Error";
        case ConfigErrorType::ImportExportError: return "Import/Export Error";
        default: return "Unknown Error";
    }
}

std::string ConfigErrorReporter::getParameterTypeName(ParameterType type) const {
    switch (type) {
        case ParameterType::Int: return "int";
        case ParameterType::Int64: return "int64_t";
        case ParameterType::Float: return "float";
        case ParameterType::Double: return "double";
        case ParameterType::Bool: return "bool";
        case ParameterType::String: return "string";
        case ParameterType::StringList: return "string[]";
        case ParameterType::IntList: return "int[]";
        case ParameterType::FloatList: return "float[]";
        case ParameterType::Choice: return "choice";
        case ParameterType::Range: return "range";
        case ParameterType::Vector2D: return "vector2d";
        case ParameterType::Vector3D: return "vector3d";
        case ParameterType::Matrix: return "matrix";
        case ParameterType::Custom: return "custom";
        default: return "unknown";
    }
}

// ConfigHelpSystem implementation

std::string ConfigHelpSystem::getParameterHelp(const std::string& parameterName, const ConfigSchema& schema) const {
    const ParameterInfo* param = schema.getParameter(parameterName);
    if (!param) return "Parameter not found in schema.";
    
    std::ostringstream oss;
    oss << "Parameter: " << param->name << "\n";
    oss << "Description: " << param->description << "\n";
    oss << "Type: " << getParameterTypeName(param->type) << "\n";
    
    if (!param->defaultValue.empty()) {
        oss << "Default: " << param->defaultValue << "\n";
    }
    
    if (param->required) {
        oss << "Required: Yes\n";
    }
    
    if (param->range) {
        oss << "Range: " << param->range->min << " to " << param->range->max << "\n";
    }
    
    if (param->choice) {
        oss << "Choices: ";
        for (size_t i = 0; i < param->choice->choices.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << param->choice->choices[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ConfigHelpSystem::getSchemaHelp(const ConfigSchema& schema) const {
    std::ostringstream oss;
    oss << "Configuration Schema Help\n";
    oss << "========================\n\n";
    oss << "Total parameters: " << schema.getParameters().size() << "\n";
    oss << "Required parameters: " << schema.getRequiredParameters().size() << "\n";
    oss << "\nAvailable commands:\n";
    oss << "  - 'config help <parameter>' - Get help for a specific parameter\n";
    oss << "  - 'config presets' - List available presets\n";
    oss << "  - 'config migrate <version>' - Apply configuration migrations\n";
    oss << "  - 'config validate' - Validate current configuration\n";
    
    return oss.str();
}

std::string ConfigHelpSystem::getPresetHelp(const std::string& presetName) const {
    if (presetName == "quick") {
        return getQuickPresetHelp();
    } else if (presetName == "balanced") {
        return getBalancedPresetHelp();
    } else if (presetName == "research") {
        return getResearchPresetHelp();
    } else if (presetName == "minimal") {
        return getMinimalPresetHelp();
    } else if (presetName == "maximum") {
        return getMaximumPresetHelp();
    } else {
        return "Preset not found.";
    }
}

std::vector<std::string> ConfigHelpSystem::listAvailablePresets() const {
    return {"quick", "balanced", "research", "minimal", "maximum"};
}

std::string ConfigHelpSystem::getQuickPresetHelp() const {
    return """Quick Preset:
  - Optimized for maximum speed with minimal resources
  - Performance: 95
  - Memory: 2048 MB
  - Threads: 8
  - CUDA: Disabled
  - Verbose logging: Disabled
  - Cache size: 512 MB
  - Timeout: 300 seconds""";
}

std::string ConfigHelpSystem::getBalancedPresetHelp() const {
    return """Balanced Preset:
  - Balanced performance and resource usage
  - Performance: 75
  - Memory: 8192 MB
  - Threads: 8
  - CUDA: Disabled
  - Verbose logging: Disabled
  - Cache size: 1024 MB
  - Timeout: 600 seconds""";
}

std::string ConfigHelpSystem::getResearchPresetHelp() const {
    return """Research Preset:
  - Optimized for research and analysis
  - Performance: 60
  - Memory: 32768 MB
  - Threads: 16
  - CUDA: Enabled
  - Verbose logging: Enabled
  - Cache size: 4096 MB
  - Timeout: 1800 seconds
  - Checkpoint interval: 1800 seconds""";
}

std::string ConfigHelpSystem::getMinimalPresetHelp() const {
    return """Minimal Preset:
  - Minimal configuration with lowest resource usage
  - Performance: 50
  - Memory: 512 MB
  - Threads: 1
  - CUDA: Disabled
  - Verbose logging: Disabled
  - Cache size: 64 MB
  - Timeout: 120 seconds""";
}

std::string ConfigHelpSystem::getMaximumPresetHelp() const {
    return """Maximum Preset:
  - Maximum performance and resource usage
  - Performance: 100
  - Memory: 65536 MB
  - Threads: 32
  - CUDA: Enabled
  - Verbose logging: Enabled
  - Cache size: 8192 MB
  - Timeout: 7200 seconds
  - Checkpoint interval: 7200 seconds""";
}

std::string ConfigHelpSystem::getParameterTypeName(ParameterType type) const {
    switch (type) {
        case ParameterType::Int: return "int";
        case ParameterType::Int64: return "int64_t";
        case ParameterType::Float: return "float";
        case ParameterType::Double: return "double";
        case ParameterType::Bool: return "bool";
        case ParameterType::String: return "string";
        case ParameterType::StringList: return "string[]";
        case ParameterType::IntList: return "int[]";
        case ParameterType::FloatList: return "float[]";
        case ParameterType::Choice: return "choice";
        case ParameterType::Range: return "range";
        case ParameterType::Vector2D: return "vector2d";
        case ParameterType::Vector3D: return "vector3d";
        case ParameterType::Matrix: return "matrix";
        case ParameterType::Custom: return "custom";
        default: return "unknown";
    }
}

} // namespace nlm