#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <future>
#include <filesystem>

namespace nlm {

// Script command types
enum class ScriptCommandType {
    Run,
    Set,
    If,
    While,
    For,
    Loop,
    Break,
    Continue,
    Return,
    Print,
    Include,
    Import,
    Export,
    Save,
    Load,
    Sleep,
    Fork,
    Join,
    Parallel,
    Profile,
    Benchmark,
    Custom
};

// Script data types
enum class ScriptDataType {
    None,
    Int,
    Float,
    String,
    Bool,
    Array,
    Object,
    Config,
    Brain,
    Function,
    Variable
};

// Script value (variant type)
class ScriptValue {
public:
    ScriptDataType type;
    
    // Constructors
    ScriptValue() : type(ScriptDataType::None) {}
    ScriptValue(int value) : type(ScriptDataType::Int) { intValue = value; }
    ScriptValue(double value) : type(ScriptDataType::Float) { floatValue = value; }
    ScriptValue(const std::string& value) : type(ScriptDataType::String) { stringValue = value; }
    ScriptValue(bool value) : type(ScriptDataType::Bool) { boolValue = value; }
    ScriptValue(const std::vector<ScriptValue>& value) : type(ScriptDataType::Array) { arrayValue = value; }
    ScriptValue(const std::map<std::string, ScriptValue>& value) : type(ScriptDataType::Object) { objectValue = value; }
    
    // Getters
    int asInt() const { return intValue; }
    double asFloat() const { return floatValue; }
    const std::string& asString() const { return stringValue; }
    bool asBool() const { return boolValue; }
    const std::vector<ScriptValue>& asArray() const { return arrayValue; }
    const std::map<std::string, ScriptValue>& asObject() const { return objectValue; }
    
    // Type checking
    bool isInt() const { return type == ScriptDataType::Int; }
    bool isFloat() const { return type == ScriptDataType::Float; }
    bool isString() const { return type == ScriptDataType::String; }
    bool isBool() const { return type == ScriptDataType::Bool; }
    bool isArray() const { return type == ScriptDataType::Array; }
    bool isObject() const { return type == ScriptDataType::Object; }
    
    // Serialization
    std::string toString() const;
    std::string toJSON() const;
    
private:
    union {
        int intValue;
        double floatValue;
        bool boolValue;
        std::string stringValue;
        std::vector<ScriptValue> arrayValue;
        std::map<std::string, ScriptValue> objectValue;
    };
    
    friend class ScriptInterpreter;
};

// Script command
struct ScriptCommand {
    ScriptCommandType type;
    std::string name;
    std::vector<ScriptValue> arguments;
    std::string file;
    int line;
    
    ScriptCommand(ScriptCommandType t, const std::string& n, 
                  const std::vector<ScriptValue>& args = {},
                  const std::string& f = "", int l = 0)
        : type(t), name(n), arguments(args), file(f), line(l) {}
};

// Script context
class ScriptContext {
public:
    ScriptContext() = default;
    
    // Variable management
    void setVariable(const std::string& name, const ScriptValue& value);
    ScriptValue getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    std::vector<std::string> getVariableNames() const;
    
    // Function management
    void defineFunction(const std::string& name, 
                       const std::function<ScriptValue(const std::vector<ScriptValue>&)>& func);
    ScriptValue callFunction(const std::string& name, const std::vector<ScriptValue>& args);
    bool hasFunction(const std::string& name) const;
    
    // Scope management
    void pushScope();
    void popScope();
    void clearScope();
    
    // Error handling
    void addError(const std::string& message, const std::string& context = "");
    const std::vector<std::string>& getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }
    
    // Execution state
    void setBreakPoint(const std::string& condition);
    bool checkBreakPoint();
    void setStepMode(bool enabled) { step_mode_ = enabled; }
    bool isStepMode() const { return step_mode_; }
    
    // Configuration
    void setConfig(const std::string& key, const ScriptValue& value);
    ScriptValue getConfig(const std::string& key) const;
    
private:
    std::map<std::string, ScriptValue> variables_;
    std::map<std::string, std::function<ScriptValue(const std::vector<ScriptValue>&)>> functions_;
    std::vector<std::map<std::string, ScriptValue>> variable_scopes_;
    std::vector<std::map<std::string, std::function<ScriptValue(const std::vector<ScriptValue>&)>>>
        function_scopes_;
    std::vector<std::string> errors_;
    std::string break_condition_;
    bool step_mode_ = false;
};

// Script interpreter
class ScriptInterpreter {
public:
    ScriptInterpreter() = default;
    
    // Command execution
    ScriptValue executeCommand(const ScriptCommand& command, ScriptContext& context);
    std::vector<ScriptValue> executeScript(const std::vector<ScriptCommand>& script, 
                                          ScriptContext& context);
    
    // Script parsing
    std::vector<ScriptCommand> parseScript(const std::string& script);
    std::vector<ScriptCommand> parseFile(const std::string& filename);
    
    // Built-in commands
    static ScriptCommand createBuiltInCommand(ScriptCommandType type, 
                                             const std::string& name,
                                             const std::vector<ScriptValue>& args = {});
    
    // Command handlers
    static ScriptValue handleRun(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleSet(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handlePrint(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleIf(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleWhile(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleFor(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleLoop(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleBreak(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleContinue(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleReturn(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleInclude(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleSave(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleLoad(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleSleep(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleParallel(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleProfile(const std::vector<ScriptValue>& args, ScriptContext& context);
    static ScriptValue handleBenchmark(const std::vector<ScriptValue>& args, ScriptContext& context);
    
    // Configuration file handling
    static std::string scriptToConfig(const std::string& script_filename, 
                                    const std::string& config_filename);
    static std::string configToScript(const std::string& config_filename, 
                                    const std::string& script_filename);
    
    // Batch processing
    static void runBatch(const std::vector<std::string>& script_files,
                       const std::map<std::string, std::vector<ScriptValue>>& parameter_sets,
                       const std::string& output_dir = "batch_results");
    
    // Parameter sweeps
    static void runParameterSweep(const std::string& script_file,
                                const std::string& param_file,
                                const std::string& output_dir = "param_sweep_results");
    
    // Workflow automation
    static void runWorkflow(const std::vector<std::string>& steps,
                           const std::map<std::string, ScriptValue>& workflow_config);
    
private:
    std::map<ScriptCommandType, std::function<ScriptValue(const std::vector<ScriptValue>&, ScriptContext&)>>
        command_handlers_;
    
    // Parsing helpers
    static std::vector<std::string> tokenize(const std::string& script);
    static ScriptValue parseValue(const std::string& token);
    static ScriptCommandType getCommandType(const std::string& name);
    
    // Command execution helpers
    static ScriptValue evaluateCondition(const ScriptValue& value);
    static bool compareValues(const ScriptValue& a, const ScriptValue& b, const std::string& op);
    
    // Error handling
    static void addScriptError(ScriptContext& context, const std::string& message,
                             const ScriptCommand& command);
};

// Script execution engine
class ScriptExecutionEngine {
public:
    ScriptExecutionEngine() = default;
    
    // Execute script
    ScriptValue executeScript(const std::string& script_filename,
                             const std::map<std::string, ScriptValue>& initial_context = {});
    
    // Execute script from string
    ScriptValue executeScriptString(const std::string& script,
                                   const std::map<std::string, ScriptValue>& initial_context = {});
    
    // Load and execute script file
    ScriptValue executeScriptFile(const std::string& filename,
                                 const std::map<std::string, ScriptValue>& initial_context = {});
    
    // Execute batch of scripts
    std::vector<ScriptValue> executeBatch(const std::vector<std::string>& script_files,
                                         const std::map<std::string, ScriptValue>& shared_context = {});
    
    // Execute parameter sweep
    std::vector<ScriptValue> executeParameterSweep(const std::string& script_file,
                                                  const std::string& param_file,
                                                  const std::string& output_dir = "param_sweep_results");
    
    // Execute workflow
    ScriptValue executeWorkflow(const std::vector<std::string>& steps,
                               const std::map<std::string, ScriptValue>& workflow_config = {});
    
    // Save execution results
    bool saveResults(const std::string& filename, const ScriptValue& results);
    ScriptValue loadResults(const std::string& filename);
    
    // Profiling integration
    void enableProfiling() { profiling_enabled_ = true; }
    void disableProfiling() { profiling_enabled_ = false; }
    bool isProfilingEnabled() const { return profiling_enabled_; }
    
private:
    ScriptInterpreter interpreter_;
    bool profiling_enabled_ = false;
    std::string current_session_id_;
    
    // Profiling integration
    void recordScriptExecution(const std::string& script_name, double duration);
};

// CLI commands for scripting
class ScriptingCommands {
public:
    static void registerScriptingCommands();
    
    static bool handleScriptingCommand(const std::string& command,
                                     ScriptExecutionEngine& engine,
                                     const class Brain* brain = nullptr);
    
private:
    static void printScriptHelp();
    static void printBatchHelp();
    static void printParameterSweepHelp();
    static void printWorkflowHelp();
};

} // namespace nlm