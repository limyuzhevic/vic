#include "ScriptEngine.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <future>
#include <filesystem>

namespace nlm {

// ScriptValue implementation
std::string ScriptValue::toString() const {
    switch (type) {
        case ScriptDataType::None: return "null";
        case ScriptDataType::Int: return std::to_string(intValue);
        case ScriptDataType::Float: {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << floatValue;
            return oss.str();
        }
        case ScriptDataType::String: return "\"" + stringValue + "\"";
        case ScriptDataType::Bool: return boolValue ? "true" : "false";
        case ScriptDataType::Array: {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arrayValue.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arrayValue[i].toString();
            }
            oss << "]";
            return oss.str();
        }
        case ScriptDataType::Object: {
            std::ostringstream oss;
            oss << "{";
            bool first = true;
            for (const auto& pair : objectValue) {
                if (!first) oss << ", ";
                oss << "\"" << pair.first << "\": " << pair.second.toString();
                first = false;
            }
            oss << "}";
            return oss.str();
        }
        default: return "";
    }
}

std::string ScriptValue::toJSON() const {
    switch (type) {
        case ScriptDataType::None: return "null";
        case ScriptDataType::Int: return std::to_string(intValue);
        case ScriptDataType::Float: {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << floatValue;
            return oss.str();
        }
        case ScriptDataType::String: {
            std::ostringstream oss;
            oss << "\"";
            for (char c : stringValue) {
                switch (c) {
                    case '\\': oss << "\\\\"; break;
                    case '\"': oss << "\\\""; break;
                    case '\b': oss << "\\b"; break;
                    case '\f': oss << "\\f"; break;
                    case '\n': oss << "\\n"; break;
                    case '\r': oss << "\\r"; break;
                    case '\t': oss << "\\t"; break;
                    default: oss << c; break;
                }
            }
            oss << "\"";
            return oss.str();
        }
        case ScriptDataType::Bool: return boolValue ? "true" : "false";
        case ScriptDataType::Array: {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arrayValue.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arrayValue[i].toJSON();
            }
            oss << "]";
            return oss.str();
        }
        case ScriptDataType::Object: {
            std::ostringstream oss;
            oss << "{";
            bool first = true;
            for (const auto& pair : objectValue) {
                if (!first) oss << ", ";
                oss << "\"" << pair.first << "\": " << pair.second.toJSON();
                first = false;
            }
            oss << "}";
            return oss.str();
        }
        default: return "null";
    }
}

// ScriptContext implementation
void ScriptContext::setVariable(const std::string& name, const ScriptValue& value) {
    if (!variable_scopes_.empty()) {
        variable_scopes_.back()[name] = value;
    } else {
        variables_[name] = value;
    }
}

ScriptValue ScriptContext::getVariable(const std::string& name) const {
    // Check local scope first
    for (auto it = variable_scopes_.rbegin(); it != variable_scopes_.rend(); ++it) {
        auto var_it = it->find(name);
        if (var_it != it->end()) {
            return var_it->second;
        }
    }
    
    auto global_it = variables_.find(name);
    if (global_it != variables_.end()) {
        return global_it->second;
    }
    
    return ScriptValue();
}

bool ScriptContext::hasVariable(const std::string& name) const {
    for (const auto& scope : variable_scopes_) {
        if (scope.find(name) != scope.end()) {
            return true;
        }
    }
    return variables_.find(name) != variables_.end();
}

void ScriptContext::removeVariable(const std::string& name) {
    if (!variable_scopes_.empty()) {
        variable_scopes_.back().erase(name);
    } else {
        variables_.erase(name);
    }
}

std::vector<std::string> ScriptContext::getVariableNames() const {
    std::vector<std::string> names;
    for (const auto& scope : variable_scopes_) {
        for (const auto& pair : scope) {
            names.push_back(pair.first);
        }
    }
    for (const auto& pair : variables_) {
        if (std::find(names.begin(), names.end(), pair.first) == names.end()) {
            names.push_back(pair.first);
        }
    }
    return names;
}

void ScriptContext::defineFunction(const std::string& name,
                                   const std::function<ScriptValue(const std::vector<ScriptValue>&)>& func) {
    if (!function_scopes_.empty()) {
        function_scopes_.back()[name] = func;
    } else {
        functions_[name] = func;
    }
}

ScriptValue ScriptContext::callFunction(const std::string& name, const std::vector<ScriptValue>& args) {
    // Check local scope first
    for (auto it = function_scopes_.rbegin(); it != function_scopes_.rend(); ++it) {
        auto func_it = it->find(name);
        if (func_it != it->end()) {
            return func_it->second(args);
        }
    }
    
    auto func_it = functions_.find(name);
    if (func_it != functions_.end()) {
        return func_it->second(args);
    }
    
    return ScriptValue();
}

bool ScriptContext::hasFunction(const std::string& name) const {
    for (const auto& scope : function_scopes_) {
        if (scope.find(name) != scope.end()) {
            return true;
        }
    }
    return functions_.find(name) != functions_.end();
}

void ScriptContext::pushScope() {
    variable_scopes_.emplace_back();
    function_scopes_.emplace_back();
}

void ScriptContext::popScope() {
    if (!variable_scopes_.empty()) {
        variable_scopes_.pop_back();
    }
    if (!function_scopes_.empty()) {
        function_scopes_.pop_back();
    }
}

void ScriptContext::clearScope() {
    variable_scopes_.clear();
    function_scopes_.clear();
}

void ScriptContext::addError(const std::string& message, const std::string& context) {
    errors_.push_back("Error: " + message + (context.empty() ? "" : " (Context: " + context + ")"));
}

void ScriptContext::setConfig(const std::string& key, const ScriptValue& value) {
    // Could be extended to store configuration values
    setVariable("config_" + key, value);
}

ScriptValue ScriptContext::getConfig(const std::string& key) const {
    return getVariable("config_" + key);
}

// ScriptInterpreter implementation
ScriptValue ScriptInterpreter::executeCommand(const ScriptCommand& command, ScriptContext& context) {
    // Find command handler
    ScriptCommandType type = getCommandType(command.name);
    if (type == ScriptCommandType::Custom) {
        // Check for built-in commands
        auto it = command_handlers_.find(command.type);
        if (it != command_handlers_.end()) {
            return it->second(command.arguments, context);
        }
        
        // Check for user-defined functions
        if (context.hasFunction(command.name)) {
            return context.callFunction(command.name, command.arguments);
        }
        
        // Not found
        context.addError("Unknown command: " + command.name);
        return ScriptValue();
    }
    
    // Execute built-in command
    auto it = command_handlers_.find(type);
    if (it != command_handlers_.end()) {
        return it->second(command.arguments, context);
    }
    
    context.addError("Command not implemented: " + command.name);
    return ScriptValue();
}

std::vector<ScriptValue> ScriptInterpreter::executeScript(const std::vector<ScriptCommand>& script, 
                                                        ScriptContext& context) {
    std::vector<ScriptValue> results;
    
    for (const auto& command : script) {
        if (context.hasErrors()) {
            context.addError("Script execution stopped due to errors");
            break;
        }
        
        ScriptValue result = executeCommand(command, context);
        results.push_back(result);
        
        // Check for step mode
        if (context.isStepMode() && !command.name.empty()) {
            std::cout << ">>> Executing: " << command.name << std::endl;
            std::cout << "Current variables:" << std::endl;
            for (const auto& name : context.getVariableNames()) {
                ScriptValue val = context.getVariable(name);
                std::cout << "  " << name << " = " << val.toString() << std::endl;
            }
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
        
        // Check for break condition
        if (context.checkBreakPoint()) {
            context.addError("Break point reached");
            break;
        }
    }
    
    return results;
}

std::vector<ScriptCommand> ScriptInterpreter::parseScript(const std::string& script) {
    std::vector<ScriptCommand> commands;
    std::vector<std::string> tokens = tokenize(script);
    
    std::string current_file = "";
    int current_line = 0;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string token = tokens[i];
        
        // Skip whitespace
        if (token == " " || token == "\t" || token == "\n") {
            continue;
        }
        
        // Check for comments
        if (token.substr(0, 1) == "#") {
            continue;
        }
        
        // Parse command
        ScriptCommandType type = getCommandType(token);
        if (type == ScriptCommandType::Custom) {
            // Try to parse as a built-in command
            ScriptCommandType builtin_type = getCommandType(token);
            if (builtin_type != ScriptCommandType::Custom) {
                std::string name = token;
                std::vector<ScriptValue> args;
                
                // Parse arguments
                i++;
                while (i < tokens.size() && tokens[i] != ";") {
                    args.push_back(parseValue(tokens[i]));
                    i++;
                }
                
                commands.emplace_back(builtin_type, name, args, current_file, current_line);
                continue;
            }
        }
        
        // Handle custom commands
        std::string name = token;
        std::vector<ScriptValue> args;
        
        // Parse arguments
        i++;
        while (i < tokens.size() && tokens[i] != ";") {
            args.push_back(parseValue(tokens[i]));
            i++;
        }
        
        commands.emplace_back(ScriptCommandType::Custom, name, args, current_file, current_line);
    }
    
    return commands;
}

std::vector<ScriptCommand> ScriptInterpreter::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open script file: " + filename);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return parseScript(content);
}

ScriptCommand ScriptInterpreter::createBuiltInCommand(ScriptCommandType type,
                                                     const std::string& name,
                                                     const std::vector<ScriptValue>& args) {
    return ScriptCommand(type, name, args);
}

ScriptValue ScriptInterpreter::handleRun(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 1) {
        context.addError("Run command requires at least one argument (script file)");
        return ScriptValue();
    }
    
    std::string script_file = args[0].asString();
    try {
        auto script = parseFile(script_file);
        return executeScript(script, context).empty() ? ScriptValue() : executeScript(script, context)[0];
    } catch (const std::exception& e) {
        context.addError("Failed to run script " + script_file + ": " + e.what());
        return ScriptValue();
    }
}

ScriptValue ScriptInterpreter::handleSet(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 2) {
        context.addError("Set command requires at least two arguments (variable name, value)");
        return ScriptValue();
    }
    
    std::string var_name = args[0].asString();
    ScriptValue value = args[1];
    
    context.setVariable(var_name, value);
    return value;
}

ScriptValue ScriptInterpreter::handlePrint(const std::vector<ScriptValue>& args, ScriptContext& context) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) std::cout << " ";
        std::cout << args[i].toString();
    }
    std::cout << std::endl;
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleIf(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 2) {
        context.addError("If command requires condition and then branch");
        return ScriptValue();
    }
    
    ScriptValue condition = args[0];
    bool result = evaluateCondition(condition);
    
    if (result) {
        // Execute then branch
        return executeCommand(ScriptCommand(ScriptCommandType::Custom, "then", 
                                          std::vector<ScriptValue>(args.begin() + 1, args.end()), 
                                          "", 0), context);
    }
    
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleWhile(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 1) {
        context.addError("While command requires condition");
        return ScriptValue();
    }
    
    while (evaluateCondition(args[0])) {
        // Execute loop body
        for (size_t i = 1; i < args.size(); ++i) {
            ScriptValue result = executeCommand(ScriptCommand(ScriptCommandType::Custom, "loop", 
                                                            {args[i]}, "", 0), context);
            if (result.isString() && result.asString() == "break") {
                return ScriptValue();
            }
        }
    }
    
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleFor(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 3) {
        context.addError("For command requires variable, start, end, and optional step");
        return ScriptValue();
    }
    
    std::string var_name = args[0].asString();
    double start = args[1].asFloat();
    double end = args[2].asFloat();
    double step = (args.size() > 3) ? args[3].asFloat() : 1.0;
    
    for (double i = start; i <= end; i += step) {
        context.setVariable(var_name, ScriptValue(i));
        
        // Execute loop body
        for (size_t j = 4; j < args.size(); ++j) {
            ScriptValue result = executeCommand(ScriptCommand(ScriptCommandType::Custom, "loop", 
                                                            {args[j]}, "", 0), context);
            if (result.isString() && result.asString() == "break") {
                return ScriptValue();
            }
        }
    }
    
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleLoop(const std::vector<ScriptValue>& args, ScriptContext& context) {
    for (const auto& command : args) {
        ScriptValue result = executeCommand(ScriptCommand(ScriptCommandType::Custom, "loop_item", 
                                                        {command}, "", 0), context);
        if (result.isString() && result.asString() == "break") {
            return ScriptValue();
        }
    }
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleBreak(const std::vector<ScriptValue>& args, ScriptContext& context) {
    context.setVariable("break_flag", ScriptValue(true));
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleContinue(const std::vector<ScriptValue>& args, ScriptContext& context) {
    context.setVariable("continue_flag", ScriptValue(true));
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleReturn(const std::vector<ScriptValue>& args, ScriptContext& context) {
    ScriptValue result = args.empty() ? ScriptValue() : args[0];
    context.setVariable("return_value", result);
    return result;
}

ScriptValue ScriptInterpreter::handleInclude(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 1) {
        context.addError("Include command requires script file");
        return ScriptValue();
    }
    
    std::string script_file = args[0].asString();
    try {
        auto script = parseFile(script_file);
        executeScript(script, context);
        return ScriptValue();
    } catch (const std::exception& e) {
        context.addError("Failed to include script " + script_file + ": " + e.what());
        return ScriptValue();
    }
}

ScriptValue ScriptInterpreter::handleSave(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 2) {
        context.addError("Save command requires filename and data");
        return ScriptValue();
    }
    
    std::string filename = args[0].asString();
    ScriptValue data = args[1];
    
    // For now, just store in variables
    context.setVariable("_saved_" + filename, data);
    return data;
}

ScriptValue ScriptInterpreter::handleLoad(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 1) {
        context.addError("Load command requires filename");
        return ScriptValue();
    }
    
    std::string filename = args[0].asString();
    
    // For now, just retrieve from variables
    return context.getVariable("_saved_" + filename);
}

ScriptValue ScriptInterpreter::handleSleep(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 1) {
        context.addError("Sleep command requires duration");
        return ScriptValue();
    }
    
    double duration = args[0].asFloat();
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(duration * 1000)));
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleParallel(const std::vector<ScriptValue>& args, ScriptContext& context) {
    if (args.size() < 1) {
        context.addError("Parallel command requires function and arguments");
        return ScriptValue();
    }
    
    std::function<ScriptValue()> func = [args, &context]() {
        ScriptContext local_context = context;
        return executeCommand(ScriptCommand(ScriptCommandType::Custom, "parallel_func", args, "", 0), local_context);
    };
    
    // For now, just execute synchronously
    return func();
}

ScriptValue ScriptInterpreter::handleProfile(const std::vector<ScriptValue>& args, ScriptContext& context) {
    // TODO: Implement profiling
    return ScriptValue();
}

ScriptValue ScriptInterpreter::handleBenchmark(const std::vector<ScriptValue>& args, ScriptContext& context) {
    // TODO: Implement benchmarking
    return ScriptValue();
}

std::string ScriptInterpreter::scriptToConfig(const std::string& script_filename,
                                            const std::string& config_filename) {
    // TODO: Implement script to config conversion
    return "";
}

std::string ScriptInterpreter::configToScript(const std::string& config_filename,
                                            const std::string& script_filename) {
    // TODO: Implement config to script conversion
    return "";
}

void ScriptInterpreter::runBatch(const std::vector<std::string>& script_files,
                                const std::map<std::string, std::vector<ScriptValue>>& parameter_sets,
                                const std::string& output_dir) {
    // TODO: Implement batch processing
}

void ScriptInterpreter::runParameterSweep(const std::string& script_file,
                                        const std::string& param_file,
                                        const std::string& output_dir) {
    // TODO: Implement parameter sweep
}

void ScriptInterpreter::runWorkflow(const std::vector<std::string>& steps,
                                   const std::map<std::string, ScriptValue>& workflow_config) {
    // TODO: Implement workflow execution
}

std::vector<std::string> ScriptInterpreter::tokenize(const std::string& script) {
    std::vector<std::string> tokens;
    std::string current_token;
    
    for (char c : script) {
        if (std::isspace(c)) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else if (c == '"' || c == '\'') {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
            
            // Find closing quote
            size_t quote_pos = script.find(c, script.find(c) + 1);
            if (quote_pos != std::string::npos) {
                tokens.push_back(script.substr(script.find(c) + 1, quote_pos - script.find(c) - 1));
                i += quote_pos - script.find(c) + 1;
            } else {
                tokens.push_back(script.substr(script.find(c) + 1));
                break;
            }
        } else {
            current_token += c;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

ScriptValue ScriptInterpreter::parseValue(const std::string& token) {
    if (token == "null" || token == "NULL") {
        return ScriptValue();
    } else if (token == "true" || token == "TRUE") {
        return ScriptValue(true);
    } else if (token == "false" || token == "FALSE") {
        return ScriptValue(false);
    } else if (token[0] == '"' && token[token.size() - 1] == '"') {
        return ScriptValue(token.substr(1, token.size() - 2));
    } else {
        // Try to parse as number
        try {
            size_t pos;
            int int_val = std::stoi(token, &pos);
            if (pos == token.size()) {
                return ScriptValue(int_val);
            }
            
            double float_val = std::stod(token, &pos);
            if (pos == token.size()) {
                return ScriptValue(float_val);
            }
        } catch (...) {
            // Not a number
        }
    }
    
    return ScriptValue(token);
}

ScriptCommandType ScriptInterpreter::getCommandType(const std::string& name) {
    static const std::map<std::string, ScriptCommandType> command_map = {
        {"run", ScriptCommandType::Run},
        {"set", ScriptCommandType::Set},
        {"print", ScriptCommandType::Print},
        {"if", ScriptCommandType::If},
        {"while", ScriptCommandType::While},
        {"for", ScriptCommandType::For},
        {"loop", ScriptCommandType::Loop},
        {"break", ScriptCommandType::Break},
        {"continue", ScriptCommandType::Continue},
        {"return", ScriptCommandType::Return},
        {"include", ScriptCommandType::Include},
        {"import", ScriptCommandType::Import},
        {"export", ScriptCommandType::Export},
        {"save", ScriptCommandType::Save},
        {"load", ScriptCommandType::Load},
        {"sleep", ScriptCommandType::Sleep},
        {"fork", ScriptCommandType::Fork},
        {"join", ScriptCommandType::Join},
        {"parallel", ScriptCommandType::Parallel},
        {"profile", ScriptCommandType::Profile},
        {"benchmark", ScriptCommandType::Benchmark},
        {"custom", ScriptCommandType::Custom}
    };
    
    auto it = command_map.find(name);
    if (it != command_map.end()) {
        return it->second;
    }
    
    return ScriptCommandType::Custom;
}

ScriptValue ScriptInterpreter::evaluateCondition(const ScriptValue& value) {
    if (value.isBool()) {
        return value;
    } else if (value.isInt()) {
        return ScriptValue(value.asInt() != 0);
    } else if (value.isFloat()) {
        return ScriptValue(value.asFloat() != 0.0);
    } else if (value.isString()) {
        return ScriptValue(!value.asString().empty());
    }
    
    return ScriptValue(false);
}

bool ScriptInterpreter::compareValues(const ScriptValue& a, const ScriptValue& b, const std::string& op) {
    if (op == "==") {
        return a.toString() == b.toString();
    } else if (op == "!=") {
        return a.toString() != b.toString();
    } else if (op == "<=") {
        if (a.isFloat() && b.isFloat()) return a.asFloat() <= b.asFloat();
        if (a.isInt() && b.isInt()) return a.asInt() <= b.asInt();
        return false;
    } else if (op == "<") {
        if (a.isFloat() && b.isFloat()) return a.asFloat() < b.asFloat();
        if (a.isInt() && b.isInt()) return a.asInt() < b.asInt();
        return false;
    } else if (op == ">=") {
        if (a.isFloat() && b.isFloat()) return a.asFloat() >= b.asFloat();
        if (a.isInt() && b.isInt()) return a.asInt() >= b.asInt();
        return false;
    } else if (op == ">") {
        if (a.isFloat() && b.isFloat()) return a.asFloat() > b.asFloat();
        if (a.isInt() && b.isInt()) return a.asInt() > b.asInt();
        return false;
    }
    
    return false;
}

void ScriptInterpreter::addScriptError(ScriptContext& context, const std::string& message,
                                      const ScriptCommand& command) {
    context.addError(message + (command.file.empty() ? "" : " in file " + command.file) +
                    (command.line > 0 ? " at line " + std::to_string(command.line) : ""));
}

// ScriptExecutionEngine implementation
ScriptValue ScriptExecutionEngine::executeScript(const std::string& script_filename,
                                               const std::map<std::string, ScriptValue>& initial_context) {
    ScriptContext context;
    
    // Set initial context
    for (const auto& pair : initial_context) {
        context.setVariable(pair.first, pair.second);
    }
    
    try {
        return interpreter_.executeScriptFile(script_filename, context);
    } catch (const std::exception& e) {
        context.addError("Script execution failed: " + std::string(e.what()));
        return ScriptValue();
    }
}

ScriptValue ScriptExecutionEngine::executeScriptString(const std::string& script,
                                                      const std::map<std::string, ScriptValue>& initial_context) {
    ScriptContext context;
    
    // Set initial context
    for (const auto& pair : initial_context) {
        context.setVariable(pair.first, pair.second);
    }
    
    try {
        auto script_commands = interpreter_.parseScript(script);
        auto results = interpreter_.executeScript(script_commands, context);
        return results.empty() ? ScriptValue() : results[0];
    } catch (const std::exception& e) {
        context.addError("Script execution failed: " + std::string(e.what()));
        return ScriptValue();
    }
}

ScriptValue ScriptExecutionEngine::executeScriptFile(const std::string& filename,
                                                   const std::map<std::string, ScriptValue>& initial_context) {
    return executeScript(filename, initial_context);
}

std::vector<ScriptValue> ScriptExecutionEngine::executeBatch(const std::vector<std::string>& script_files,
                                                           const std::map<std::string, ScriptValue>& shared_context) {
    std::vector<std::future<ScriptValue>> futures;
    std::vector<ScriptValue> results;
    
    for (const auto& script_file : script_files) {
        futures.push_back(std::async(std::launch::async, [this, script_file, shared_context]() {
            return executeScript(script_file, shared_context);
        }));
    }
    
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    
    return results;
}

std::vector<ScriptValue> ScriptExecutionEngine::executeParameterSweep(const std::string& script_file,
                                                                    const std::string& param_file,
                                                                    const std::string& output_dir) {
    // TODO: Implement parameter sweep
    return std::vector<ScriptValue>();
}

ScriptValue ScriptExecutionEngine::executeWorkflow(const std::vector<std::string>& steps,
                                                 const std::map<std::string, ScriptValue>& workflow_config) {
    // TODO: Implement workflow execution
    return ScriptValue();
}

bool ScriptExecutionEngine::saveResults(const std::string& filename, const ScriptValue& results) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement serialization
    // For now, just store as JSON
    std::string json = results.toJSON();
    file << json;
    file.close();
    return true;
}

ScriptValue ScriptExecutionEngine::loadResults(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return ScriptValue();
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // TODO: Implement deserialization
    return ScriptValue(content);
}

void ScriptExecutionEngine::recordScriptExecution(const std::string& script_name, double duration) {
    // TODO: Implement script execution recording
}

// CLI commands for scripting
void ScriptingCommands::registerScriptingCommands() {
    // TODO: Register CLI commands for scripting
}

bool ScriptingCommands::handleScriptingCommand(const std::string& command,
                                            ScriptExecutionEngine& engine,
                                            const class Brain* brain) {
    if (command == "script-help") {
        printScriptHelp();
        return true;
    } else if (command == "batch-help") {
        printBatchHelp();
        return true;
    } else if (command == "param-sweep-help") {
        printParameterSweepHelp();
        return true;
    } else if (command == "workflow-help") {
        printWorkflowHelp();
        return true;
    }
    return false;
}

void ScriptingCommands::printScriptHelp() {
    std::cout << "=== Script Language Help ===" << std::endl;
    std::cout << "Basic script commands:" << std::endl;
    std::cout << "  set <var> <value>     Set variable" << std::endl;
    std::cout << "  print <value>...       Print value(s)" << std::endl;
    std::cout << "  if <condition> <then>   Conditional execution" << std::endl;
    std::cout << "  while <condition>       Loop while condition is true" << std::endl;
    std::cout << "  for <var> <start> <end> [step] Loop" << std::endl;
    std::cout << "  loop <items>...        Execute loop" << std::endl;
    std::cout << "  break                   Break from loop" << std::endl;
    std::cout << "  continue                Continue to next iteration" << std::endl;
    std::cout << "  return [value]          Return from function" << std::endl;
    std::cout << "  include <file>         Include script file" << std::endl;
    std::cout << "  save <file> <data>      Save data to variable" << std::endl;
    std::cout << "  load <file>            Load data from variable" << std::endl;
    std::cout << "  sleep <seconds>        Sleep for given time" << std::endl;
    std::cout << "  parallel <func>...      Run function in parallel" << std::endl;
    std::cout << "  profile <command>       Profile command execution" << std::endl;
    std::cout << "  benchmark <command>     Benchmark command execution" << std::endl;
    std::cout << std::endl;
    std::cout << "Variable types: int, float, string, bool, array, object" << std::endl;
    std::cout << "Array syntax: [value1, value2, value3]" << std::endl;
    std::cout << "Object syntax: {key1: value1, key2: value2}" << std::endl;
}

void ScriptingCommands::printBatchHelp() {
    std::cout << "=== Batch Processing Help ===" << std::endl;
    std::cout << "Run multiple scripts in parallel:" << std::endl;
    std::cout << "  run-batch <script1> <script2> ... <output_dir>" << std::endl;
    std::cout << std::endl;
    std::cout << "Parameters can be specified as key=value pairs" << std::endl;
    std::cout << "Results are saved to output directory" << std::endl;
}

void ScriptingCommands::printParameterSweepHelp() {
    std::cout << "=== Parameter Sweep Help ===" << std::endl;
    std::cout << "Vary parameters systematically:" << std::endl;
    std::cout << "  param-sweep <script> <param_file> [output_dir]" << std::endl;
    std::cout << std::endl;
    std::cout << "Parameter file format:" << std::endl;
    std::cout << "  param1: [0.1, 0.2, 0.3]" << std::endl;
    std::cout << "  param2: [1, 2, 3]" << std::endl;
}

void ScriptingCommands::printWorkflowHelp() {
    std::cout << "=== Workflow Automation Help ===" << std::endl;
    std::cout << "Automate complex workflows:" << std::endl;
    std::cout << "  workflow <step1> <step2> ... [config]" << std::endl;
    std::cout << std::endl;
    std::cout << "Each step can be a script file or inline command" << std::endl;
    std::cout << "Configuration can pass data between steps" << std::endl;
}

} // namespace nlm