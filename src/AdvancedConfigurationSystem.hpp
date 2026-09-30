#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/Experiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <regex>
#include <fstream>
#include <sstream>

namespace nlm {

/**
 * @brief Advanced configuration and scripting system for NLM
 * 
 * This namespace provides sophisticated configuration management capabilities,
 * including Lua and Python scripting support, automated experiment design,
 * parameter optimization, and distributed computing support.
 */
namespace AdvancedConfiguration {

/**
 * @brief Lua script context for configuration and experiment automation
 */
class LuaScriptContext {
public:
    struct Variable {
        std::string name;
        std::string type;  // "int", "float", "string", "bool", "table"
        std::string value;
        std::vector<std::string> dependencies;
        std::function<void()> callback;
        
        Variable() = default;
        Variable(const std::string& n, const std::string& t, const std::string& v)
            : name(n), type(t), value(v) {}
    };

    struct Function {
        std::string name;
        std::string description;
        std::vector<std::string> parameterTypes;
        std::vector<std::string> parameterNames;
        std::function<std::string(const std::vector<std::string>&)> implementation;
        bool isBuiltIn;
        
        Function() : isBuiltIn(false) {}
        Function(const std::string& n, const std::string& desc,
                 const std::function<std::string(const std::vector<std::string>&)>& impl,
                 bool builtIn = false)
            : name(n), description(desc), implementation(impl), isBuiltIn(builtIn) {}
    };

    struct Table {
        std::string name;
        std::map<std::string, Variable> fields;
        std::map<std::string, Function> methods;
        std::vector<Table> rows;  // For tabular data
        
        Table() = default;
        Table(const std::string& n) : name(n) {}
    };

private:
    std::map<std::string, Variable> variables_;
    std::map<std::string, Function> functions_;
    std::vector<Table> tables_;
    
    // Built-in functions
    std::map<std::string, Function> builtInFunctions_;
    
    // Expression evaluation
    struct Expression {
        enum class Type { OPERAND, OPERATOR, FUNCTION, VARIABLE };
        Type type;
        
        // For OPERAND
        float numericValue;
        std::string stringValue;
        
        // For OPERATOR
        char operatorChar;
        
        // For FUNCTION
        std::string functionName;
        std::vector<Expression> arguments;
        
        // For VARIABLE
        std::string variableName;
        
        Expression() : type(Type::OPERAND) {}
    };
    
    // Parsing and evaluation
    std::vector<Expression> parseExpression(const std::string& expr);
    float evaluateExpression(const Expression& expr);
    bool validateExpression(const Expression& expr) const;
    
    // Built-in function implementations
    static std::string builtin_log(const std::vector<std::string>& args);
    static std::string builtin_random(const std::vector<std::string>& args);
    static std::string builtin_range(const std::vector<std::string>& args);
    static std::string builtin_mean(const std::vector<std::string>& args);
    static std::string builtin_median(const std::vector<std::string>& args);
    static std::string builtin_sum(const std::vector<std::string>& args);
    static std::string builtin_concat(const std::vector<std::string>& args);
    static std::string builtin_file_read(const std::vector<std::string>& args);
    static std::string builtin_file_write(const std::vector<std::string>& args);
    static std::string builtin_date_time(const std::vector<std::string>& args);
    static std::string builtin_experiment_run(const std::vector<std::string>& args);
    static std::string builtin_config_load(const std::vector<std::string>& args);
    static std::string builtin_config_save(const std::vector<std::string>& args);
    
    // Expression helpers
    float parseNumber(const std::string& str) const;
    bool isOperator(char c) const;
    int getOperatorPrecedence(char op) const;
    bool isOperatorAssociative(char op) const;
    std::string infixToPostfix(const std::string& infix);
    
    friend class ConfigurationScriptingEngine;

public:
    LuaScriptContext() { initializeBuiltInFunctions(); }
    ~LuaScriptContext() = default;
    
    // Variable management
    void setVariable(const std::string& name, const std::string& type, const std::string& value);
    void setVariable(const std::string& name, int value) {
        setVariable(name, "int", std::to_string(value));
    }
    void setVariable(const std::string& name, float value) {
        setVariable(name, "float", std::to_string(value));
    }
    void setVariable(const std::string& name, const std::string& value, const std::string& type) {
        setVariable(name, type, value);
    }
    
    bool hasVariable(const std::string& name) const {
        return variables_.find(name) != variables_.end();
    }
    
    std::string getVariable(const std::string& name) const {
        auto it = variables_.find(name);
        if (it != variables_.end()) {
            return it->second.value;
        }
        return "";
    }
    
    int getVariableInt(const std::string& name) const {
        auto it = variables_.find(name);
        if (it != variables_.end() && it->second.type == "int") {
            return std::stoi(it->second.value);
        }
        return 0;
    }
    
    float getVariableFloat(const std::string& name) const {
        auto it = variables_.find(name);
        if (it != variables_.end() && it->second.type == "float") {
            return std::stof(it->second.value);
        }
        return 0.0f;
    }
    
    bool getVariableBool(const std::string& name) const {
        auto it = variables_.find(name);
        if (it != variables_.end() && it->second.type == "bool") {
            return it->second.value == "true" || it->second.value == "1";
        }
        return false;
    }
    
    // Function management
    void addFunction(const Function& func) {
        functions_[func.name] = func;
    }
    
    bool hasFunction(const std::string& name) const {
        return functions_.find(name) != functions_.end();
    }
    
    std::string callFunction(const std::string& name, const std::vector<std::string>& args) {
        auto it = functions_.find(name);
        if (it == functions_.end()) {
            return "Error: Function not found: " + name;
        }
        
        try {
            return it->second.implementation(args);
        } catch (const std::exception& e) {
            return "Error in function " + name + ": " + std::string(e.what());
        }
    }
    
    // Table management
    void addTable(const Table& table) {
        tables_.push_back(table);
    }
    
    Table* getTable(const std::string& name) {
        for (auto& table : tables_) {
            if (table.name == name) {
                return &table;
            }
        }
        return nullptr;
    }
    
    const Table* getTable(const std::string& name) const {
        for (const auto& table : tables_) {
            if (table.name == name) {
                return &table;
            }
        }
        return nullptr;
    }
    
    // Expression evaluation
    float evaluate(const std::string& expression) {
        std::vector<Expression> postfix = infixToPostfix(expression);
        return evaluatePostfix(postfix);
    }
    
    float evaluatePostfix(const std::vector<Expression>& postfix) {
        std::stack<float> values;
        
        for (const auto& expr : postfix) {
            if (expr.type == Expression::Type::OPERAND) {
                if (expr.numericValue != std::numeric_limits<float>::quiet_NaN()) {
                    values.push(expr.numericValue);
                } else {
                    // Try to get variable value
                    if (variables_.find(expr.stringValue) != variables_.end()) {
                        float varValue = parseNumber(variables_.at(expr.stringValue).value);
                        values.push(varValue);
                    } else {
                        values.push(0.0f);
                    }
                }
            } else if (expr.type == Expression::Type::FUNCTION) {
                if (values.size() < expr.arguments.size()) {
                    throw std::runtime_error("Not enough arguments for function " + expr.functionName);
                }
                
                std::vector<std::string> args;
                std::stack<float> tempStack;
                
                // Collect arguments in reverse order (stack is LIFO)
                for (auto it = expr.arguments.rbegin(); it != expr.arguments.rend(); ++it) {
                    if (it->type == Expression::Type::OPERAND && 
                        it->numericValue != std::numeric_limits<float>::quiet_NaN()) {
                        tempStack.push(it->numericValue);
                    } else {
                        tempStack.push(0.0f);
                    }
                }
                
                // Convert to strings for function call
                while (!tempStack.empty()) {
                    args.push_back(std::to_string(tempStack.top()));
                    tempStack.pop();
                }
                
                std::string result = callFunction(expr.functionName, args);
                try {
                    float value = std::stof(result);
                    values.push(value);
                } catch (...) {
                    values.push(0.0f);
                }
            } else if (expr.type == Expression::Type::OPERATOR) {
                if (values.size() < 2) {
                    throw std::runtime_error("Not enough operands for operator " + std::string(1, expr.operatorChar));
                }
                
                float b = values.top(); values.pop();
                float a = values.top(); values.pop();
                
                float result = applyOperator(a, b, expr.operatorChar);
                values.push(result);
            }
        }
        
        if (values.empty()) {
            throw std::runtime_error("Expression evaluation resulted in empty stack");
        }
        
        return values.top();
    }
    
    // Scripting utilities
    void loadScript(const std::string& filename);
    void executeScript(const std::string& script);
    void saveScript(const std::string& filename) const;
    
    // Configuration integration
    std::shared_ptr<Config> toConfig() const;
    static LuaScriptContext fromConfig(const Config& config);
    
private:
    void initializeBuiltInFunctions() {
        builtInFunctions_ = {
            {"log", {"log", "Log a message to console", {"string"}, {"message"}, &builtin_log, true}},
            {"random", {"random", "Generate random number", {"string"}, {"range"}, &builtin_random, true}},
            {"range", {"range", "Create a range", {"string"}, {"args"}, &builtin_range, true}},
            {"mean", {"mean", "Calculate mean of values", {"string"}, {"values"}, &builtin_mean, true}},
            {"median", {"median", "Calculate median of values", {"string"}, {"values"}, &builtin_median, true}},
            {"sum", {"sum", "Sum values", {"string"}, {"values"}, &builtin_sum, true}},
            {"concat", {"concat", "Concatenate strings", {"string"}, {"strings"}, &builtin_concat, true}},
            {"file_read", {"file_read", "Read file content", {"string"}, {"filename"}, &builtin_file_read, true}},
            {"file_write", {"file_write", "Write to file", {"string"}, {"filename"}, &builtin_file_write, true}},
            {"date_time", {"date_time", "Get current date/time", {"string"}, {"format"}, &builtin_date_time, true}},
            {"experiment_run", {"experiment_run", "Run experiment", {"string"}, {"experiment_name"}, &builtin_experiment_run, true}},
            {"config_load", {"config_load", "Load configuration", {"string"}, {"filename"}, &builtin_config_load, true}},
            {"config_save", {"config_save", "Save configuration", {"string"}, {"filename"}, &builtin_config_save, true}}
        };
        
        // Copy built-in functions to functions_
        for (const auto& pair : builtInFunctions_) {
            functions_[pair.first] = pair.second;
        }
    }
    
    float applyOperator(float a, float b, char op) const {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return b != 0.0f ? a / b : std::numeric_limits<float>::quiet_NaN();
            case '^': return std::pow(a, b);
            case '%': return b != 0.0f ? std::fmod(a, b) : std::numeric_limits<float>::quiet_NaN();
            default: return std::numeric_limits<float>::quiet_NaN();
        }
    }
};

/**
 * @brief Advanced configuration system with experiment automation
 */
class AdvancedConfigurationSystem {
public:
    struct ExperimentTemplate {
        std::string name;
        std::string description;
        std::shared_ptr<Config> baseConfig;
        std::map<std::string, std::function<std::shared_ptr<Config>(const std::map<std::string, float>&)>> parameterVariations;
        std::vector<std::string> requiredVariables;
        std::vector<std::string> optionalVariables;
        std::string scriptFile;  // Lua script for experiment setup
        bool isValidated;
        
        ExperimentTemplate() : isValidated(false) {}
    };

    struct ExperimentResult {
        std::string experimentName;
        double startTime;
        double endTime;
        std::map<std::string, float> parameters;
        float finalPerformance;
        std::map<std::string, std::vector<float>> metrics;
        std::string status;  // "success", "failed", "running"
        std::string errorMessage;
        
        ExperimentResult() : finalPerformance(0.0f), startTime(0.0), endTime(0.0) {}
    };

    struct OptimizationResult {
        std::string optimizationName;
        double startTime;
        double endTime;
        std::map<std::string, float> bestParameters;
        float bestScore;
        std::vector<std::map<std::string, float>> parameterHistory;
        std::vector<float> scoreHistory;
        std::string optimizerType;  // "bayesian", "evolutionary", "grid_search"
        
        OptimizationResult() : bestScore(0.0f), startTime(0.0), endTime(0.0) {}
    };

private:
    std::map<std::string, std::shared_ptr<Config>> configurationTemplates_;
    std::map<std::string, ExperimentTemplate> experimentTemplates_;
    std::vector<ExperimentResult> experimentHistory_;
    std::vector<OptimizationResult> optimizationHistory_;
    
    // Lua scripting engine
    LuaScriptContext luaContext_;
    
    // Parameter optimization
    struct OptimizationParameter {
        std::string name;
        float minValue;
        float maxValue;
        float currentValue;
        float stepSize;
        std::function<float(const std::map<std::string, float>&)> objectiveFunction;
        
        OptimizationParameter() : minValue(0.0f), maxValue(1.0f), currentValue(0.5f), stepSize(0.1f) {}
    };
    
    // Hyperparameter optimization algorithms
    class BayesianOptimizer {
    public:
        struct GaussianProcess {
            std::vector<std::vector<float>> X;  // Input points
            std::vector<float> y;              // Output values
            std::vector<std::vector<float>> K;  // Kernel matrix
            std::vector<float> L;              // Cholesky decomposition
            std::vector<float> alpha;          // Solution to linear system
            
            void addObservation(const std::vector<float>& x, float y);
            float predict(const std::vector<float>& x) const;
            float getUncertainty(const std::vector<float>& x) const;
        };
        
        std::map<std::string, OptimizationParameter> parameters_;
        GaussianProcess gp_;
        float acquisitionFunctionWeight_;
        
        std::map<std::string, float> optimize(size_t iterations);
        float expectedImprovement(const std::vector<float>& x) const;
        float probabilityOfImprovement(const std::vector<float>& x) const;
    };
    
    class EvolutionaryAlgorithm {
    public:
        struct Individual {
            std::map<std::string, float> genes;
            float fitness;
            double timestamp;
            
            Individual() : fitness(0.0f), timestamp(0.0) {}
        };
        
        std::map<std::string, OptimizationParameter> parameters_;
        size_t populationSize_;
        float mutationRate_;
        float crossoverRate_;
        size_t maxGenerations_;
        
        std::map<std::string, float> optimize(size_t iterations);
        Individual createIndividual() const;
        Individual crossover(const Individual& parent1, const Individual& parent2) const;
        Individual mutate(const Individual& individual) const;
    };
    
    // Experiment scheduling
    struct ScheduledExperiment {
        std::string name;
        std::string cronExpression;
        std::map<std::string, float> parameters;
        std::string scriptFile;
        bool enabled;
        std::chrono::system_clock::time_point nextRun;
        
        ScheduledExperiment() : enabled(false) {}
    };
    
    std::vector<ScheduledExperiment> scheduledExperiments_;
    std::atomic<bool> schedulerRunning_;
    std::thread schedulerThread_;
    
public:
    AdvancedConfigurationSystem() : schedulerRunning_(false) {}
    ~AdvancedConfigurationSystem() {
        stopScheduler();
    }
    
    // Configuration management
    void addConfigurationTemplate(const std::string& name, const std::shared_ptr<Config>& config);
    std::shared_ptr<Config> getConfigurationTemplate(const std::string& name) const;
    bool hasConfigurationTemplate(const std::string& name) const;
    std::vector<std::string> getConfigurationTemplateNames() const;
    
    // Experiment template management
    void addExperimentTemplate(const ExperimentTemplate& template_);
    std::shared_ptr<ExperimentTemplate> getExperimentTemplate(const std::string& name) const;
    bool hasExperimentTemplate(const std::string& name) const;
    std::vector<std::string> getExperimentTemplateNames() const;
    
    // Experiment execution
    ExperimentResult runExperiment(const std::string& templateName,
                                    const std::map<std::string, float>& parameters = {});
    ExperimentResult runExperiment(const ExperimentTemplate& template_,
                                    const std::map<std::string, float>& parameters = {});
    
    // Script-based experiment execution
    ExperimentResult runScriptExperiment(const std::string& scriptName,
                                          const std::map<std::string, float>& parameters = {});
    void executeScript(const std::string& script, const std::map<std::string, float>& variables = {});
    
    // Configuration scripting with Lua
    bool executeLuaScript(const std::string& script);
    bool executeLuaScriptFile(const std::string& filename);
    std::string evaluateLuaExpression(const std::string& expression);
    
    // Parameter optimization
    OptimizationResult optimizeParameters(const std::string& objective,
                                           const std::map<std::string, OptimizationParameter>& parameters,
                                           const std::string& method = "bayesian");
    
    // Hyperparameter search
    OptimizationResult hyperparameterSearch(const std::string& objective,
                                            const std::map<std::string, std::vector<float>>& parameterGrids,
                                            const std::string& method = "grid_search");
    
    // Automated experiment design
    ExperimentTemplate createAutomatedExperiment(const std::string& name,
                                                  const std::string& objective,
                                                  const std::map<std::string, float>& parameterRanges);
    
    // Experiment scheduling
    void scheduleExperiment(const ScheduledExperiment& scheduledExp);
    void startScheduler();
    void stopScheduler();
    void runScheduledExperiments();
    std::vector<ScheduledExperiment> getScheduledExperiments() const { return scheduledExperiments_; }
    
    // Configuration optimization
    std::shared_ptr<Config> optimizeConfiguration(const std::string& objective,
                                                   std::shared_ptr<Config> baseConfig,
                                                   const std::map<std::string, std::pair<float, float>>& parameterRanges);
    
    // Validation and verification
    bool validateExperiment(const ExperimentTemplate& template_) const;
    bool validateConfiguration(const Config& config) const;
    std::vector<std::string> getConfigurationErrors(const Config& config) const;
    
    // Result management
    void addExperimentResult(const ExperimentResult& result);
    ExperimentResult getLastExperimentResult(const std::string& experimentName) const;
    std::vector<ExperimentResult> getExperimentHistory(const std::string& experimentName) const;
    std::vector<ExperimentResult> getAllExperimentResults() const { return experimentHistory_; }
    
    void addOptimizationResult(const OptimizationResult& result);
    OptimizationResult getLastOptimizationResult(const std::string& optimizationName) const;
    std::vector<OptimizationResult> getOptimizationHistory(const std::string& optimizationName) const;
    std::vector<OptimizationResult> getAllOptimizationResults() const { return optimizationHistory_; }
    
    // Export and import
    bool exportConfiguration(const std::string& filename, const std::string& name) const;
    bool importConfiguration(const std::string& filename);
    
    bool exportExperimentResults(const std::string& filename) const;
    bool importExperimentResults(const std::string& filename);
    
    bool exportOptimizationResults(const std::string& filename) const;
    bool importOptimizationResults(const std::string& filename);
    
    // Configuration comparison
    float compareConfigurations(const Config& config1, const Config& config2) const;
    std::string getConfigurationDifferences(const Config& config1, const Config& config2) const;
    
    // Statistics and analysis
    void analyzeExperimentResults(const std::string& experimentName) const;
    void analyzeOptimizationResults(const std::string& optimizationName) const;
    
    // Configuration validation utilities
    static bool validateParameterType(const std::string& type, const std::string& value);
    static bool validateParameterRange(const std::string& type, float value,
                                       const std::pair<float, float>& range);
    static std::string parameterToString(const std::map<std::string, float>& parameters);
    static std::map<std::string, float> stringToParameters(const std::string& str);
    
private:
    void initializeDefaultTemplates();
    std::shared_ptr<Config> applyParameterVariations(const ExperimentTemplate& template_,
                                                    const std::map<std::string, float>& parameters);
    
    static double getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
    }
};

} // namespace AdvancedConfiguration

} // namespace nlm