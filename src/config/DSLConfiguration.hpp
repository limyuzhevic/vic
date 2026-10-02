// Configuration DSL for NLM experiments
// 
// Provides a human-readable configuration language for customizing
// NLM experiments without dealing with complex configuration files.
// Supports variables, expressions, and validation.

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>
#include <optional>
#include <sstream>
#include <regex>
#include <functional>

namespace nlm {

// DSL value types
using DSLValue = std::variant<
    std::monostate,           // null
    bool,                     // boolean
    int64_t,                  // integer
    double,                   // floating point
    std::string,              // string
    std::vector<DSLValue>,    // array
    std::map<std::string, DSLValue>  // object
>;

// Expression evaluator
class DSLEvaluator {
public:
    DSLEvaluator() = default;
    
    // Evaluate an expression string
    DSLValue evaluate(const std::string& expression, 
                      const std::map<std::string, DSLValue>& variables = {});
    
    // Set custom functions
    void registerFunction(const std::string& name, 
                         std::function<DSLValue(const std::vector<DSLValue>&)> func);
    
    // Set custom operators
    void registerOperator(char op, int precedence,
                         std::function<DSLValue(DSLValue, DSLValue)> func);
    
    // Parse a configuration block
    std::map<std::string, DSLValue> parseBlock(const std::string& block,
                                               const std::map<std::string, DSLValue>& variables = {});
    
    // Validate configuration against schema
    bool validate(const std::map<std::string, DSLValue>& config,
                  const std::map<std::string, DSLValue>& schema);
    
    // Convert DSL value to string
    static std::string toString(const DSLValue& value);
    
    // Convert string to DSL value
    static DSLValue fromString(const std::string& str);

private:
    std::map<std::string, std::function<DSLValue(const std::vector<DSLValue>&)>> functions;
    std::map<char, std::pair<int, std::function<DSLValue(DSLValue, DSLValue)>>> operators;
    
    // Helper methods for expression parsing
    std::vector<DSLValue> tokenize(const std::string& expression);
    std::vector<DSLValue> parseExpression(const std::string& expression);
    DSLValue evaluateExpression(const std::vector<DSLValue>& tokens);
    DSLValue evaluateBinaryOperation(DSLValue left, char op, DSLValue right);
};

// Configuration DSL parser
class DSLParser {
public:
    DSLParser() : evaluator(std::make_unique<DSLEvaluator>()) {
        initializeBuiltins();
    }
    
    // Parse a configuration string
    std::map<std::string, DSLValue> parse(const std::string& config);
    
    // Parse configuration with variable substitution
    std::map<std::string, DSLValue> parseWithVariables(
        const std::string& config,
        const std::map<std::string, DSLValue>& variables);
    
    // Load configuration from file
    std::map<std::string, DSLValue> parseFile(const std::string& filepath);
    
    // Validate configuration against NLM schema
    bool validateNLMConfig(const std::map<std::string, DSLValue>& config);
    
    // Convert DSL config to regular NLM config format
    std::map<std::string, std::variant<int64_t, double, std::string>> 
    convertToNLMConfig(const std::map<std::string, DSLValue>& dslConfig);

private:
    std::unique_ptr<DSLEvaluator> evaluator;
    
    void initializeBuiltins();
    std::map<std::string, DSLValue> parseBlock(const std::string& block,
                                               const std::map<std::string, DSLValue>& variables);
    std::vector<std::string> splitLines(const std::string& text) const;
    std::string trim(const std::string& str) const;
    bool isComment(const std::string& line) const;
    std::string extractKeyValue(const std::string& line) const;
    std::string extractExpression(const std::string& line) const;
};

// Configuration DSL compiler (generates optimized runtime config)
class DSLCompiler {
public:
    DSLCompiler() = default;
    
    // Compile DSL configuration for runtime use
    std::map<std::string, std::variant<int64_t, double, std::string>> 
    compile(const std::map<std::string, DSLValue>& config);
    
    // Generate C++ code from DSL configuration
    std::string generateCppCode(const std::map<std::string, DSLValue>& config) const;
    
    // Optimize configuration for specific runtime conditions
    std::map<std::string, std::variant<int64_t, double, std::string>> 
    optimize(const std::map<std::string, DSLValue>& config,
             const std::map<std::string, std::variant<int64_t, double, std::string>>& hardwareInfo);

private:
    // Optimization strategies
    void applyMemoryOptimization(std::map<std::string, std::variant<int64_t, double, std::string>>& config) const;
    void applyPerformanceOptimization(std::map<std::string, std::variant<int64_t, double, std::string>>& config,
                                      const std::map<std::string, std::variant<int64_t, double, std::string>>& hardwareInfo) const;
    void applyAccuracyOptimization(std::map<std::string, std::variant<int64_t, double, std::string>>& config) const;
};

// Configuration DSL utilities
namespace DSLUtils {
    // DSL syntax highlighting
    std::string syntaxHighlight(const std::string& text);
    
    // DSL error reporting
    struct DSLError {
        std::string message;
        size_t line;
        size_t column;
        std::string severity;  // "error", "warning", "info"
    };
    
    std::vector<DSLError> validateSyntax(const std::string& dslCode);
    
    // DSL documentation generator
    std::string generateDocumentation(const std::map<std::string, DSLValue>& config);
    
    // DSL example templates
    std::string getExampleConfig(const std::string& experimentType);
    
    // DSL variable substitution
    std::string substituteVariables(const std::string& template,
                                    const std::map<std::string, std::string>& variables);
}

// Global DSL instances
inline DSLEvaluator& getDSLEvaluator() {
    static DSLEvaluator evaluator;
    return evaluator;
}

inline DSLParser& getDSLParser() {
    static DSLParser parser;
    return parser;
}

inline DSLCompiler& getDSLCompiler() {
    static DSLCompiler compiler;
    return compiler;
}

// Convenience functions for common DSL operations
inline std::map<std::string, DSLValue> parseDSLConfig(const std::string& config) {
    return getDSLParser().parse(config);
}

inline std::map<std::string, std::variant<int64_t, double, std::string>> 
compileDSLConfig(const std::string& config) {
    auto dslConfig = getDSLParser().parse(config);
    return getDSLCompiler().compile(dslConfig);
}

inline std::map<std::string, std::variant<int64_t, double, std::string>> 
compileDSLConfigWithVariables(const std::string& config,
                              const std::map<std::string, DSLValue>& variables) {
    auto dslConfig = getDSLParser().parseWithVariables(config, variables);
    return getDSLCompiler().compile(dslConfig);
}

} // namespace nlm