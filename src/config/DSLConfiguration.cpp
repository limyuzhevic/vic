// Configuration DSL implementation
// 
// Implements the domain-specific language for configuring NLM experiments.
// Provides parsing, evaluation, compilation, and validation of DSL configurations.

#include "DSLConfiguration.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>

namespace nlm {

// DSLEvaluator implementation
DSLValue DSLEvaluator::evaluate(const std::string& expression, 
                               const std::map<std::string, DSLValue>& variables) {
    try {
        std::vector<DSLValue> tokens = tokenize(expression);
        if (tokens.empty()) {
            return DSLValue(); // null
        }
        
        std::vector<DSLValue> postfix = shuntingYard(tokens);
        return evaluatePostfix(postfix, variables);
    } catch (const std::exception& e) {
        PluginUtils::logPluginEvent("DSL", "Expression evaluation error: " + std::string(e.what()));
        return DSLValue(); // Return null on error
    }
}

void DSLEvaluator::registerFunction(const std::string& name, 
                                   std::function<DSLValue(const std::vector<DSLValue>&)> func) {
    functions[name] = func;
}

void DSLEvaluator::registerOperator(char op, int precedence,
                                   std::function<DSLValue(DSLValue, DSLValue)> func) {
    operators[op] = {precedence, func};
}

std::map<std::string, DSLValue> DSLEvaluator::parseBlock(const std::string& block,
                                                         const std::map<std::string, DSLValue>& variables) {
    std::map<std::string, DSLValue> result;
    
    std::vector<std::string> lines = splitLines(block);
    for (const auto& line : lines) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || isComment(trimmed)) {
            continue;
        }
        
        // Parse key = value
        size_t equalsPos = trimmed.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = trim(trimmed.substr(0, equalsPos));
            std::string valueStr = trim(trimmed.substr(equalsPos + 1));
            
            if (!key.empty()) {
                // Try to parse as expression
                DSLValue value = evaluate(valueStr, variables);
                result[key] = value;
            }
        }
    }
    
    return result;
}

bool DSLEvaluator::validate(const std::map<std::string, DSLValue>& config,
                           const std::map<std::string, DSLValue>& schema) {
    // Basic validation - can be extended with more sophisticated schema validation
    for (const auto& pair : schema) {
        const std::string& key = pair.first;
        const DSLValue& schemaValue = pair.second;
        
        if (config.find(key) == config.end()) {
            // Required key missing
            continue;
        }
        
        const DSLValue& configValue = config.at(key);
        
        // Type checking
        if (!typesMatch(schemaValue, configValue)) {
            PluginUtils::logPluginEvent("DSL", 
                "Type mismatch for key '" + key + "': expected " + 
                std::to_string(static_cast<int>(schemaValue.index())) + 
                ", got " + std::to_string(static_cast<int>(configValue.index())));
            return false;
        }
    }
    
    return true;
}

std::string DSLEvaluator::toString(const DSLValue& value) {
    std::stringstream ss;
    
    value.visit([&](std::monostate) { ss << "null"; });
    value.visit([&](bool b) { ss << (b ? "true" : "false"); });
    value.visit([&](int64_t i) { ss << i; });
    value.visit([&](double d) { 
        // Remove trailing zeros
        ss << std::fixed << std::setprecision(6) << d;
        std::string result = ss.str();
        // Remove trailing .000000 if it's an integer
        if (result.find('.') != std::string::npos) {
            result.erase(result.find_last_not_of('0') + 1, std::string::npos);
            if (result.back() == '.') {
                result.pop_back();
            }
        }
        ss.str(result);
        ss.clear();
    });
    value.visit([&](const std::string& s) { ss << '"' << escapeString(s) << '"'; });
    value.visit([&](const std::vector<DSLValue>& arr) {
        ss << '[';
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) ss << ', ';
            ss << toString(arr[i]);
        }
        ss << ']';
    });
    value.visit([&](const std::map<std::string, DSLValue>& obj) {
        ss << '{';
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (it != obj.begin()) ss << ', ';
            ss << '"' << escapeString(it->first) << '": ' << toString(it->second);
        }
        ss << '}';
    });
    
    return ss.str();
}

DSLValue DSLEvaluator::fromString(const std::string& str) {
    if (str == "null" || str == "nil") {
        return DSLValue();
    } else if (str == "true" || str == "false") {
        return str == "true";
    } else if (str.front() == '"' && str.back() == '"') {
        return escapeString(str.substr(1, str.size() - 2));
    } else {
        // Try to parse as number
        try {
            if (str.find('.') != std::string::npos) {
                return std::stod(str);
            } else {
                return std::stoll(str);
            }
        } catch (...) {
            // Not a number, return as string
            return str;
        }
    }
}

// Helper methods for DSLEvaluator
std::vector<DSLValue> DSLEvaluator::tokenize(const std::string& expression) {
    std::vector<DSLValue> tokens;
    std::string current;
    
    for (size_t i = 0; i < expression.size(); ++i) {
        char c = expression[i];
        
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(fromString(current));
                current.clear();
            }
        } else if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}') {
            if (!current.empty()) {
                tokens.push_back(fromString(current));
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else if (c == '"') {
            size_t endQuote = expression.find('"', i + 1);
            if (endQuote == std::string::npos) {
                throw std::runtime_error("Unterminated string literal");
            }
            std::string strLiteral = expression.substr(i + 1, endQuote - i - 1);
            tokens.push_back(strLiteral);
            i = endQuote;
        } else if (c == ',' || c == ':' || c == '=' || c == '+' || c == '-' || 
                   c == '*' || c == '/' || c == '%' || c == '^' || c == '<' || c == '>') {
            if (!current.empty()) {
                tokens.push_back(fromString(current));
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(fromString(current));
    }
    
    return tokens;
}

std::vector<DSLValue> DSLEvaluator::shuntingYard(const std::vector<DSLValue>& tokens) {
    std::vector<DSLValue> output;
    std::vector<DSLValue> stack;
    
    for (const DSLValue& token : tokens) {
        if (std::holds_alternative<std::string>(token)) {
            std::string str = std::get<std::string>(token);
            if (isNumber(str) || isString(str)) {
                output.push_back(token);
            } else if (functions.find(str) != functions.end()) {
                stack.push_back(token);
            } else if (operators.find(str[0]) != operators.end()) {
                while (!stack.empty() && 
                       stack.back() != '(' &&
                       operators.at(stack.back().get<std::string>()[0]).first >= 
                       operators.at(str[0]).first) {
                    output.push_back(stack.back());
                    stack.pop_back();
                }
                stack.push_back(token);
            } else {
                output.push_back(token);
            }
        } else {
            output.push_back(token);
        }
    }
    
    while (!stack.empty()) {
        output.push_back(stack.back());
        stack.pop_back();
    }
    
    return output;
}

DSLValue DSLEvaluator::evaluatePostfix(const std::vector<DSLValue>& tokens,
                                      const std::map<std::string, DSLValue>& variables) {
    std::stack<DSLValue> stack;
    
    for (const DSLValue& token : tokens) {
        if (std::holds_alternative<std::string>(token)) {
            std::string str = std::get<std::string>(token);
            
            if (isNumber(str)) {
                if (str.find('.') != std::string::npos) {
                    stack.push(std::stod(str));
                } else {
                    stack.push(std::stoll(str));
                }
            } else if (isString(str)) {
                stack.push(escapeString(str));
            } else if (variables.find(str) != variables.end()) {
                stack.push(variables.at(str));
            } else if (functions.find(str) != functions.end()) {
                // Function call (simplified - should be handled differently)
                stack.push(functions[str](std::vector<DSLValue>{}));
            } else {
                stack.push(token);
            }
        } else {
            stack.push(token);
        }
    }
    
    if (stack.size() != 1) {
        throw std::runtime_error("Invalid expression: too many operands");
    }
    
    return stack.top();
}

bool DSLEvaluator::typesMatch(const DSLValue& schema, const DSLValue& config) {
    // Simple type matching - schema type is preferred if specified
    return schema.index() == config.index();
}

bool DSLEvaluator::isNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), 
        [](char c) { return std::isdigit(c) || c == '.' || c == '-'; });
}

bool DSLEvaluator::isString(const std::string& str) {
    return !str.empty() && 
           (std::isalpha(str[0]) || str[0] == '"');
}

std::string DSLEvaluator::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\0': result += "\\0"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            default: result += c; break;
        }
    }
    return result;
}

// DSLParser implementation
std::map<std::string, DSLValue> DSLParser::parse(const std::string& config) {
    return parseWithVariables(config, std::map<std::string, DSLValue>{});
}

std::map<std::string, DSLValue> DSLParser::parseWithVariables(
    const std::string& config,
    const std::map<std::string, DSLValue>& variables) {
    std::map<std::string, DSLValue> result;
    
    std::vector<std::string> lines = splitLines(config);
    std::string currentBlock;
    
    for (const auto& line : lines) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || isComment(trimmed)) {
            currentBlock += line + "\n";
            continue;
        }
        
        // Check if this is a block start/end
        if (trimmed.find('{') != std::string::npos) {
            currentBlock += line + "\n";
        } else if (trimmed.find('}') != std::string::npos) {
            currentBlock += line + "\n";
            // Parse the block
            auto blockResult = parseBlock(currentBlock, variables);
            for (const auto& pair : blockResult) {
                result[pair.first] = pair.second;
            }
            currentBlock.clear();
        } else {
            currentBlock += line + "\n";
        }
    }
    
    // Parse any remaining block
    if (!currentBlock.empty()) {
        auto blockResult = parseBlock(currentBlock, variables);
        for (const auto& pair : blockResult) {
            result[pair.first] = pair.second;
        }
    }
    
    return result;
}

bool DSLParser::validateNLMConfig(const std::map<std::string, DSLValue>& config) {
    // Basic NLM configuration schema
    std::map<std::string, DSLValue> schema = {
        {"random_seed", int64_t(0)},
        {"simulation_timestep", 0.001},
        {"neuron_count", int64_t(500)},
        {"region_count", int64_t(1)},
        {"connection_probability", 0.15},
        {"stdp_ltp_weight", 0.02},
        {"stdp_ltd_weight", 0.015},
        {"stdp_tau", 20.0},
        {"synaptogenesis_rate", 0.0001},
        {"pruning_rate", 0.00001},
        {"enable_neuromodulation", true},
        {"enable_working_memory", true},
        {"enable_episodic_memory", true},
        {"replay_interval", 100},
        {"consolidation_interval", 1000}
    };
    
    return evaluator->validate(config, schema);
}

std::map<std::string, std::variant<int64_t, double, std::string>> 
DSLParser::convertToNLMConfig(const std::map<std::string, DSLValue>& dslConfig) {
    std::map<std::string, std::variant<int64_t, double, std::string>> result;
    
    for (const auto& pair : dslConfig) {
        const std::string& key = pair.first;
        const DSLValue& value = pair.second;
        
        // Convert DSL value to NLM configuration value
        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                // null - skip
            } else if constexpr (std::is_same_v<T, bool>) {
                result[key] = val ? 1 : 0;  // Convert bool to int for config
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result[key] = val;
            } else if constexpr (std::is_same_v<T, double>) {
                result[key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                result[key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<DSLValue>>) {
                // Arrays not supported in basic NLM config
            } else if constexpr (std::is_same_v<T, std::map<std::string, DSLValue>>) {
                // Objects not supported in basic NLM config
            }
        }, value);
    }
    
    return result;
}

void DSLParser::initializeBuiltins() {
    // Mathematical functions
    evaluator->registerFunction("abs", [](const std::vector<DSLValue>& args) {
        if (args.size() != 1) throw std::runtime_error("abs requires 1 argument");
        if (std::holds_alternative<double>(args[0])) {
            return std::abs(std::get<double>(args[0]));
        } else if (std::holds_alternative<int64_t>(args[0])) {
            return std::abs(std::get<int64_t>(args[0]));
        } else {
            throw std::runtime_error("abs requires numeric argument");
        }
    });
    
    evaluator->registerFunction("pow", [](const std::vector<DSLValue>& args) {
        if (args.size() != 2) throw std::runtime_error("pow requires 2 arguments");
        double base = std::get<double>(args[0]);
        double exponent = std::get<double>(args[1]);
        return std::pow(base, exponent);
    });
    
    evaluator->registerFunction("min", [](const std::vector<DSLValue>& args) {
        if (args.size() < 2) throw std::runtime_error("min requires at least 2 arguments");
        DSLValue result = args[0];
        for (size_t i = 1; i < args.size(); ++i) {
            if (compareValues(result, args[i]) > 0) {
                result = args[i];
            }
        }
        return result;
    });
    
    evaluator->registerFunction("max", [](const std::vector<DSLValue>& args) {
        if (args.size() < 2) throw std::runtime_error("max requires at least 2 arguments");
        DSLValue result = args[0];
        for (size_t i = 1; i < args.size(); ++i) {
            if (compareValues(result, args[i]) < 0) {
                result = args[i];
            }
        }
        return result;
    });
    
    // Comparison operators
    evaluator->registerOperator('<', 10, 
        [](DSLValue left, DSLValue right) {
            return compareValues(left, right) < 0;
        });
    
    evaluator->registerOperator('>', 10,
        [](DSLValue left, DSLValue right) {
            return compareValues(left, right) > 0;
        });
    
    evaluator->registerOperator('<=', 10,
        [](DSLValue left, DSLValue right) {
            return compareValues(left, right) <= 0;
        });
    
    evaluator->registerOperator('>=', 10,
        [](DSLValue left, DSLValue right) {
            return compareValues(left, right) >= 0;
        });
    
    evaluator->registerOperator('==', 9,
        [](DSLValue left, DSLValue right) {
            return compareValues(left, right) == 0;
        });
    
    evaluator->registerOperator('!=', 9,
        [](DSLValue left, DSLValue right) {
            return compareValues(left, right) != 0;
        });
    
    evaluator->registerOperator('&&', 8,
        [](DSLValue left, DSLValue right) {
            return std::get<bool>(left) && std::get<bool>(right);
        });
    
    evaluator->registerOperator('||', 7,
        [](DSLValue left, DSLValue right) {
            return std::get<bool>(left) || std::get<bool>(right);
        });
    
    evaluator->registerOperator('!', 12,
        [](DSLValue left, DSLValue) {
            return !std::get<bool>(left);
        });
}

std::vector<std::string> DSLParser::splitLines(const std::string& text) const {
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

std::string DSLParser::trim(const std::string& str) const {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    
    return str.substr(start, end - start);
}

bool DSLParser::isComment(const std::string& line) const {
    return line.find('#') != std::string::npos ||
           line.find("//") != std::string::npos;
}

std::string DSLParser::extractKeyValue(const std::string& line) const {
    // Simple key=value extraction
    size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos) {
        return "";
    }
    
    std::string key = trim(line.substr(0, equalsPos));
    std::string value = trim(line.substr(equalsPos + 1));
    
    return key + " = " + value;
}

std::string DSLParser::extractExpression(const std::string& line) const {
    // Extract expression from curly braces
    size_t braceStart = line.find('{');
    size_t braceEnd = line.find('}');
    
    if (braceStart == std::string::npos || braceEnd == std::string::npos) {
        return "";
    }
    
    return line.substr(braceStart + 1, braceEnd - braceStart - 1);
}

// DSLCompiler implementation
std::map<std::string, std::variant<int64_t, double, std::string>> 
DSLCompiler::compile(const std::map<std::string, DSLValue>& config) {
    std::map<std::string, std::variant<int64_t, double, std::string>> result;
    
    for (const auto& pair : config) {
        const std::string& key = pair.first;
        const DSLValue& value = pair.second;
        
        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                // null - skip
            } else if constexpr (std::is_same_v<T, bool>) {
                result[key] = val ? 1 : 0;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result[key] = val;
            } else if constexpr (std::is_same_v<T, double>) {
                result[key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                result[key] = val;
            }
        }, value);
    }
    
    // Apply optimizations
    applyMemoryOptimization(result);
    
    return result;
}

std::string DSLCompiler::generateCppCode(const std::map<std::string, DSLValue>& config) const {
    std::stringstream ss;
    ss << "// Generated C++ configuration from DSL" << std::endl;
    ss << "// Auto-generated on " << __DATE__ << " " << __TIME__ << std::endl;
    ss << std::endl;
    ss << "#include \"core/Config/Config.hpp\"" << std::endl;
    ss << std::endl;
    ss << "void configureNLM(Config& config) {" << std::endl;
    
    for (const auto& pair : config) {
        ss << "    // " << pair.first << " = " << DSLEvaluator::toString(pair.second) << std::endl;
        
        // Generate appropriate config command
        if (pair.first == "random_seed") {
            ss << "    config.set(\"random_seed\", static_cast<int64_t>" << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "simulation_timestep") {
            ss << "    config.set(\"simulation_timestep\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "neuron_count") {
            ss << "    config.set(\"neuron_count\", static_cast<int64_t>" << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "region_count") {
            ss << "    config.set(\"region_count\", static_cast<int64_t>" << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "connection_probability") {
            ss << "    config.set(\"connection_probability\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "stdp_ltp_weight") {
            ss << "    config.set(\"stdp_ltp_weight\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "stdp_ltd_weight") {
            ss << "    config.set(\"stdp_ltd_weight\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "stdp_tau") {
            ss << "    config.set(\"stdp_tau\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "synaptogenesis_rate") {
            ss << "    config.set(\"synaptogenesis_rate\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "pruning_rate") {
            ss << "    config.set(\"pruning_rate\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "enable_neuromodulation") {
            ss << "    config.set(\"enable_neuromodulation\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "enable_working_memory") {
            ss << "    config.set(\"enable_working_memory\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "enable_episodic_memory") {
            ss << "    config.set(\"enable_episodic_memory\", " << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "replay_interval") {
            ss << "    config.set(\"replay_interval\", static_cast<int64_t>" << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else if (pair.first == "consolidation_interval") {
            ss << "    config.set(\"consolidation_interval\", static_cast<int64_t>" << 
                DSLEvaluator::toString(pair.second) << ", ConfigSource::User);" << std::endl;
        } else {
            ss << "    // Key \"" << pair.first << "\" not recognized" << std::endl;
        }
    }
    
    ss << "}" << std::endl;
    
    return ss.str();
}

std::map<std::string, std::variant<int64_t, double, std::string>> 
DSLCompiler::optimize(const std::map<std::string, DSLValue>& config,
                      const std::map<std::string, std::variant<int64_t, double, std::string>>& hardwareInfo) {
    std::map<std::string, std::variant<int64_t, double, std::string>> result;
    
    for (const auto& pair : config) {
        const std::string& key = pair.first;
        const DSLValue& value = pair.second;
        
        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                // null - skip
            } else if constexpr (std::is_same_v<T, bool>) {
                result[key] = val ? 1 : 0;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                // Optimize integer values for hardware
                if (key == "neuron_count" && hardwareInfo.find("core_count") != hardwareInfo.end()) {
                    int64_t coreCount = std::get<int64_t>(hardwareInfo.at("core_count"));
                    // Adjust neuron count based on available cores
                    int64_t optimalNeurons = std::min(val, coreCount * 100);
                    result[key] = optimalNeurons;
                } else {
                    result[key] = val;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                // Optimize floating point values
                if (key == "simulation_timestep") {
                    // Ensure timestep is reasonable for hardware
                    double timestep = val;
                    if (timestep < 0.0001) timestep = 0.0001;
                    if (timestep > 0.1) timestep = 0.1;
                    result[key] = timestep;
                } else {
                    result[key] = val;
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                result[key] = val;
            }
        }, value);
    }
    
    // Apply specific optimizations
    applyMemoryOptimization(result);
    applyPerformanceOptimization(result, hardwareInfo);
    applyAccuracyOptimization(result);
    
    return result;
}

void DSLCompiler::applyMemoryOptimization(
    std::map<std::string, std::variant<int64_t, double, std::string>>& config) const {
    // Reduce memory usage where possible
    if (config.find("neuron_count") != config.end() && 
        std::get<int64_t>(config["neuron_count"]) > 1000) {
        config["neuron_count"] = int64_t(1000);  // Cap at reasonable limit
    }
    
    // Optimize working memory settings
    if (config.find("enable_working_memory") != config.end()) {
        bool enableWM = std::get<int64_t>(config["enable_working_memory"]) != 0;
        if (!enableWM && config.find("replay_interval") != config.end()) {
            config.erase("replay_interval");
            config.erase("consolidation_interval");
        }
    }
}

void DSLCompiler::applyPerformanceOptimization(
    std::map<std::string, std::variant<int64_t, double, std::string>>& config,
    const std::map<std::string, std::variant<int64_t, double, std::string>>& hardwareInfo) const {
    // Optimize based on hardware capabilities
    if (hardwareInfo.find("simd_support") != hardwareInfo.end()) {
        bool simdSupport = std::get<int64_t>(hardwareInfo.at("simd_support")) != 0;
        if (simdSupport && config.find("connection_probability") != config.end()) {
            // Enable connection optimization with SIMD
            config["connection_probability"] = 0.20;  // Optimal for SIMD
        }
    }
}

void DSLCompiler::applyAccuracyOptimization(
    std::map<std::string, std::variant<int64_t, double, std::string>>& config) const {
    // Prioritize accuracy over speed where possible
    if (config.find("stdp_tau") != config.end()) {
        double tau = std::get<double>(config["stdp_tau"]);
        if (tau < 10.0) {
            // Very short time constant - may cause instability
            config["stdp_tau"] = 10.0;  // Ensure reasonable stability
        }
    }
}

namespace DSLUtils {

std::string syntaxHighlight(const std::string& text) {
    std::stringstream ss;
    std::regex commentRegex("#.*|//.*");
    std::regex numberRegex("\b\d+\.\d+\b|\b\d+\b");
    std::regex stringRegex("\"[^\"]*\"");
    std::regex keywordRegex("\b(random_seed|simulation_timestep|neuron_count|region_count|connection_probability|stdp_ltp_weight|stdp_ltd_weight|stdp_tau|synaptogenesis_rate|pruning_rate|enable_neuromodulation|enable_working_memory|enable_episodic_memory|replay_interval|consolidation_interval)\b");
    
    std::string line;
    std::istringstream iss(text);
    
    while (std::getline(iss, line)) {
        // Apply syntax highlighting
        line = std::regex_replace(line, commentRegex, "[comment]$&[/comment]");
        line = std::regex_replace(line, numberRegex, "[number]$0[/number]");
        line = std::regex_replace(line, stringRegex, "[string]$0[/string]");
        line = std::regex_replace(line, keywordRegex, "[keyword]$0[/keyword]");
        
        ss << line << "\n";
    }
    
    return ss.str();
}

std::vector<DSLUtils::DSLError> DSLUtils::validateSyntax(const std::string& dslCode) {
    std::vector<DSLError> errors;
    
    std::vector<std::string> lines = DSLParser::splitLines(dslCode);
    
    for (size_t lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const std::string& line = lines[lineNum];
        std::string trimmed = DSLParser::trim(line);
        
        if (trimmed.empty()) continue;
        
        // Check for common syntax errors
        if (trimmed.find('{') != std::string::npos && trimmed.find('}') == std::string::npos) {
            DSLError error;
            error.message = "Unterminated block - missing closing brace";
            error.line = lineNum + 1;
            error.column = trimmed.find('{') + 1;
            error.severity = "error";
            errors.push_back(error);
        }
        
        if (trimmed.find('}') != std::string::npos && trimmed.find('{') == std::string::npos) {
            DSLError error;
            error.message = "Unexpected closing brace - missing opening brace";
            error.line = lineNum + 1;
            error.column = trimmed.find('}') + 1;
            error.severity = "error";
            errors.push_back(error);
        }
        
        // Check for unmatched quotes in strings
        size_t quoteCount = std::count(trimmed.begin(), trimmed.end(), '"');
        if (quoteCount % 2 != 0) {
            DSLError error;
            error.message = "Unmatched quotes in string literal";
            error.line = lineNum + 1;
            error.column = trimmed.find('"') + 1;
            error.severity = "error";
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::string DSLUtils::generateDocumentation(const std::map<std::string, DSLValue>& config) {
    std::stringstream ss;
    ss << "NLM Configuration Documentation" << std::endl;
    ss << std::string(50, '=') << std::endl;
    ss << std::endl;
    
    for (const auto& pair : config) {
        ss << "**" << pair.first << "**" << std::endl;
        ss << "Type: " << getTypeName(pair.second) << std::endl;
        ss << "Value: " << DSLEvaluator::toString(pair.second) << std::endl;
        ss << "Description: ";
        
        // Provide basic descriptions for common keys
        if (pair.first == "random_seed") {
            ss << "Random seed for reproducible experiments. Use same seed for debugging.";
        } else if (pair.first == "simulation_timestep") {
            ss << "Time step duration in seconds for neural simulation.";
        } else if (pair.first == "neuron_count") {
            ss << "Number of neurons in the network. Affects memory and computation time.";
        } else if (pair.first == "region_count") {
            ss << "Number of neural regions for distributed processing.";
        } else if (pair.first == "connection_probability") {
            ss << "Probability of synapse creation between neurons. Higher = more connections.";
        } else if (pair.first == "stdp_ltp_weight") {
            ss << "STDP long-term potentiation weight (A+). Controls strengthening.";
        } else if (pair.first == "stdp_ltd_weight") {
            ss << "STDP long-term depression weight (A-). Controls weakening.";
        } else if (pair.first == "stdp_tau") {
            ss << "STDP time constant (tau). Controls timing window for plasticity.";
        } else if (pair.first == "synaptogenesis_rate") {
            ss << "Rate of new synapse creation (structural plasticity).";
        } else if (pair.first == "pruning_rate") {
            ss << "Rate of synapse elimination (structural plasticity).";
        } else if (pair.first == "enable_neuromodulation") {
            ss << "Whether neuromodulation systems (dopamine, curiosity, etc.) are enabled.";
        } else if (pair.first == "enable_working_memory") {
            ss << "Whether working memory system is active.";
        } else if (pair.first == "enable_episodic_memory") {
            ss << "Whether episodic memory system is active.";
        } else if (pair.first == "replay_interval") {
            ss << "Interval (in simulation steps) for memory replay.";
        } else if (pair.first == "consolidation_interval") {
            ss << "Interval (in simulation steps) for memory consolidation.";
        } else {
            ss << "User-defined parameter.";
        }
        ss << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string DSLUtils::getExampleConfig(const std::string& experimentType) {
    if (experimentType == "simple") {
        return R"(
# Simple NLM experiment configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
enable_neuromodulation = true
enable_working_memory = true
";
    } else if (experimentType == "complex") {
        return R"(
# Complex NLM experiment configuration with neuromodulation
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.10
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_tau = 25.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005
enable_neuromodulation = true
enable_working_memory = true
enable_episodic_memory = true
replay_interval = 200
consolidation_interval = 2000
";
    } else if (experimentType == "research") {
        return R"(
# Research configuration - high precision and recording
random_seed = 99999
simulation_timestep = 0.0001
neuron_count = 10000
region_count = 5
connection_probability = 0.05
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 50.0
synaptogenesis_rate = 0.000001
pruning_rate = 0.0000001
enable_neuromodulation = true
enable_working_memory = true
enable_episodic_memory = true
replay_interval = 500
consolidation_interval = 5000
";
    }
    
    return "# Unknown experiment type: " + experimentType + "\n";
}

std::string DSLUtils::substituteVariables(const std::string& template,
                                        const std::map<std::string, std::string>& variables) {
    std::string result = template;
    
    for (const auto& pair : variables) {
        std::string placeholder = "${" + pair.first + "}";
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos = result.find(placeholder, pos + pair.second.length());
        }
    }
    
    return result;
}

// Helper functions for DSL evaluation
DSLValue compareValues(const DSLValue& a, const DSLValue& b) {
    if (a.index() != b.index()) {
        // Different types - compare by type precedence
        return DSLValue(0); // a < b
    }
    
    if (std::holds_alternative<bool>(a)) {
        return std::get<bool>(a) < std::get<bool>(b) ? DSLValue(-1) : 
               std::get<bool>(a) > std::get<bool>(b) ? DSLValue(1) : DSLValue(0);
    } else if (std::holds_alternative<int64_t>(a)) {
        return std::get<int64_t>(a) - std::get<int64_t>(b);
    } else if (std::holds_alternative<double>(a)) {
        double diff = std::get<double>(a) - std::get<double>(b);
        if (diff < 0) return DSLValue(-1);
        if (diff > 0) return DSLValue(1);
        return DSLValue(0);
    } else if (std::holds_alternative<std::string>(a)) {
        if (std::get<std::string>(a) < std::get<std::string>(b)) {
            return DSLValue(-1);
        } else if (std::get<std::string>(a) > std::get<std::string>(b)) {
            return DSLValue(1);
        } else {
            return DSLValue(0);
        }
    }
    
    return DSLValue(0);
}

std::string DSLUtils::getTypeName(const DSLValue& value) {
    if (std::holds_alternative<std::monostate>(value)) return "null";
    if (std::holds_alternative<bool>(value)) return "boolean";
    if (std::holds_alternative<int64_t>(value)) return "integer";
    if (std::holds_alternative<double>(value)) return "float";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<DSLValue>>(value)) return "array";
    if (std::holds_alternative<std::map<std::string, DSLValue>>(value)) return "object";
    return "unknown";
}

} // namespace DSLUtils

} // namespace nlm