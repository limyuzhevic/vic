#include "AdvancedConfigurationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <regex>

namespace nlm {
namespace AdvancedConfiguration {

// LuaScriptContext implementation
void LuaScriptContext::loadScript(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open script file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    executeScript(buffer.str());
}

void LuaScriptContext::executeScript(const std::string& script) {
    // Simple Lua-like script execution
    // In a real implementation, this would use a proper Lua interpreter
    
    std::istringstream stream(script);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue; // Skip empty lines and comments
        }
        
        // Variable assignment
        if (line.find(" = ") != std::string::npos) {
            size_t pos = line.find(" = ");
            std::string varName = line.substr(0, pos);
            std::string varValue = line.substr(pos + 3);
            
            // Remove whitespace
            varName.erase(0, varName.find_first_not_of(" \t"));
            varName.erase(varName.find_last_not_of(" \t") + 1);
            varValue.erase(0, varValue.find_first_not_of(" \t"));
            varValue.erase(varValue.find_last_not_of(" \t") + 1);
            
            // Try to infer type
            if (std::all_of(varValue.begin(), varValue.end(), ::isdigit) ||
                (varValue[0] == '-' && std::all_of(varValue.begin() + 1, varValue.end(), ::isdigit))) {
                setVariable(varName, "int", varValue);
            } else if (std::regex_match(varValue, std::regex("-?\d+\.\d+")) ||
                       std::regex_match(varValue, std::regex("-?\d+\.")) ||
                       std::regex_match(varValue, std::regex("^\.\d+"))) {
                setVariable(varName, "float", varValue);
            } else if (varValue == "true" || varValue == "false" || varValue == "1" || varValue == "0") {
                setVariable(varName, "bool", varValue);
            } else {
                setVariable(varName, "string", varValue);
            }
        }
        
        // Function call
        else if (line.find("(") != std::string::npos && line.find(")") != std::string::npos) {
            size_t parenPos = line.find("(");
            std::string funcName = line.substr(0, parenPos);
            std::string argsStr = line.substr(parenPos + 1, line.find(")") - parenPos - 1);
            
            // Parse arguments
            std::vector<std::string> args;
            std::istringstream argStream(argsStr);
            std::string arg;
            
            while (std::getline(argStream, arg, ',')) {
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                args.push_back(arg);
            }
            
            // Call function
            std::string result = callFunction(funcName, args);
            
            // Check if result is an assignment
            if (line.find(" = ") != std::string::npos) {
                size_t assignPos = line.find(" = ");
                std::string varName = line.substr(0, assignPos);
                varName.erase(0, varName.find_first_not_of(" \t"));
                varName.erase(varName.find_last_not_of(" \t") + 1);
                setVariable(varName, "string", result);
            }
        }
    }
}

void LuaScriptContext::saveScript(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open script file for writing: " + filename);
    }
    
    // Save variables
    file << "-- Variables\n";
    for (const auto& pair : variables_) {
        file << pair.first << " = " << pair.second.value << "\n";
    }
    
    file << "\n";
    
    // Save functions (simplified)
    file << "-- Functions\n";
    for (const auto& pair : functions_) {
        if (pair.second.isBuiltIn) continue;
        file << "function " << pair.first << "()\n";
        file << "  -- Implementation would be saved here\n";
        file << "end\n\n";
    }
    
    file.close();
}

std::shared_ptr<Config> LuaScriptContext::toConfig() const {
    auto config = std::make_shared<Config>();
    
    for (const auto& pair : variables_) {
        const Variable& var = pair.second;
        
        if (var.type == "int") {
            config->setAuto(var.name, var.value);
        } else if (var.type == "float") {
            config->setAuto(var.name, var.value);
        } else if (var.type == "bool") {
            config->setAuto(var.name, var.value);
        } else if (var.type == "string") {
            config->setAuto(var.name, var.value);
        }
    }
    
    return config;
}

LuaScriptContext LuaScriptContext::fromConfig(const Config& config) {
    LuaScriptContext ctx;
    
    // Note: Config class doesn't expose its internal data directly
    // In a real implementation, this would access config's internal storage
    
    return ctx;
}

// Built-in function implementations
std::string LuaScriptContext::builtin_log(const std::vector<std::string>& args) {
    if (args.empty()) return "";
    
    std::cout << "[Lua] " << args[0] << std::endl;
    return "";
}

std::string LuaScriptContext::builtin_random(const std::vector<std::string>& args) {
    if (args.empty()) return "0.0";
    
    // Parse range specification
    std::string range = args[0];
    float min = 0.0f, max = 1.0f;
    
    if (range.find(",") != std::string::npos) {
        size_t pos = range.find(",");
        std::string minStr = range.substr(0, pos);
        std::string maxStr = range.substr(pos + 1);
        
        try {
            min = std::stof(minStr);
            max = std::stof(maxStr);
        } catch (...) {
            return "0.0";
        }
    } else {
        try {
            max = std::stof(range);
        } catch (...) {
            return "0.0";
        }
    }
    
    // Generate random number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    
    return std::to_string(dis(gen));
}

std::string LuaScriptContext::builtin_range(const std::vector<std::string>& args) {
    if (args.empty()) return "";
    
    std::string range = args[0];
    // Note: This is a simplified range implementation
    // In practice, you would need a more sophisticated parser for table operations
    return "[range not fully implemented]";
}

std::string LuaScriptContext::builtin_mean(const std::vector<std::string>& args) {
    if (args.empty()) return "0.0";
    
    // Parse comma-separated values
    std::vector<float> values;
    std::istringstream stream(args[0]);
    std::string token;
    
    while (std::getline(stream, token, ',')) {
        try {
            values.push_back(std::stof(token));
        } catch (...) {
            // Skip invalid values
        }
    }
    
    if (values.empty()) return "0.0";
    
    float sum = 0.0f;
    for (float val : values) {
        sum += val;
    }
    
    return std::to_string(sum / static_cast<float>(values.size()));
}

std::string LuaScriptContext::builtin_median(const std::vector<std::string>& args) {
    if (args.empty()) return "0.0";
    
    std::vector<float> values;
    std::istringstream stream(args[0]);
    std::string token;
    
    while (std::getline(stream, token, ',')) {
        try {
            values.push_back(std::stof(token));
        } catch (...) {
            // Skip invalid values
        }
    }
    
    if (values.empty()) return "0.0";
    
    std::sort(values.begin(), values.end());
    size_t mid = values.size() / 2;
    
    if (values.size() % 2 == 0) {
        return std::to_string((values[mid - 1] + values[mid]) / 2.0f);
    } else {
        return std::to_string(values[mid]);
    }
}

std::string LuaScriptContext::builtin_sum(const std::vector<std::string>& args) {
    if (args.empty()) return "0.0";
    
    std::vector<float> values;
    std::istringstream stream(args[0]);
    std::string token;
    
    while (std::getline(stream, token, ',')) {
        try {
            values.push_back(std::stof(token));
        } catch (...) {
            // Skip invalid values
        }
    }
    
    float sum = 0.0f;
    for (float val : values) {
        sum += val;
    }
    
    return std::to_string(sum);
}

std::string LuaScriptContext::builtin_concat(const std::vector<std::string>& args) {
    std::string result;
    for (const std::string& arg : args) {
        result += arg;
    }
    return result;
}

std::string LuaScriptContext::builtin_file_read(const std::vector<std::string>& args) {
    if (args.empty()) return "";
    
    std::string filename = args[0];
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string LuaScriptContext::builtin_file_write(const std::vector<std::string>& args) {
    if (args.size() < 2) return "";
    
    std::string filename = args[0];
    std::string content = args[1];
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    file << content;
    return "";
}

std::string LuaScriptContext::builtin_date_time(const std::vector<std::string>& args) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    
    // Convert to milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    
    // Convert to seconds, minutes, hours, days
    auto totalSeconds = duration.count() / 1000;
    auto seconds = totalSeconds % 60;
    auto totalMinutes = totalSeconds / 60;
    auto minutes = totalMinutes % 60;
    auto totalHours = totalMinutes / 60;
    auto hours = totalHours % 24;
    auto days = totalHours / 24;
    
    return "Date: " + std::to_string(days) + " days, " +
           "Time: " + std::to_string(hours) + ":" +
           (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
           (seconds < 10 ? "0" : "") + std::to_string(seconds) + "." +
           (ms < 100 ? (ms < 10 ? "00" : "0") : "") + std::to_string(ms);
}

std::string LuaScriptContext::builtin_experiment_run(const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Error: No experiment name provided";
    }
    
    // Note: In a real implementation, this would interface with the experiment framework
    return "Experiment " + args[0] + " would be run here";
}

std::string LuaScriptContext::builtin_config_load(const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Error: No config file provided";
    }
    
    // Note: This would integrate with the Config system
    return "Config " + args[0] + " would be loaded here";
}

std::string LuaScriptContext::builtin_config_save(const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Error: No config file provided";
    }
    
    // Note: This would integrate with the Config system
    return "Config " + args[0] + " would be saved here";
}

// Expression parsing (simplified)
std::vector<LuaScriptContext::Expression> LuaScriptContext::parseExpression(const std::string& expr) {
    std::vector<Expression> result;
    
    // Tokenize expression
    std::vector<std::string> tokens;
    std::string currentToken;
    
    for (char c : expr) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else if (c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            currentToken += c;
        }
    }
    
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    // Build expression tree (simplified)
    for (const std::string& token : tokens) {
        Expression expr;
        
        if (token == "(" || token == ")" || token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            expr.type = Expression::Type::OPERATOR;
            expr.operatorChar = token[0];
            result.push_back(expr);
        } else {
            expr.type = Expression::Type::OPERAND;
            
            // Try to parse as number
            try {
                expr.numericValue = std::stof(token);
            } catch (...) {
                // Try to parse as variable
                expr.stringValue = token;
            }
            
            result.push_back(expr);
        }
    }
    
    return result;
}

bool LuaScriptContext::validateExpression(const Expression& expr) const {
    // Basic validation
    switch (expr.type) {
        case Expression::Type::OPERAND:
            return true;  // Can be any value
        case Expression::Type::OPERATOR:
            return expr.operatorChar == '+' || expr.operatorChar == '-' ||
                   expr.operatorChar == '*' || expr.operatorChar == '/' ||
                   expr.operatorChar == '^';
        case Expression::Type::FUNCTION:
            return hasFunction(expr.functionName);
        case Expression::Type::VARIABLE:
            return hasVariable(expr.variableName);
        default:
            return false;
    }
}

float LuaScriptContext::parseNumber(const std::string& str) const {
    try {
        return std::stof(str);
    } catch (...) {
        return 0.0f;
    }
}

bool LuaScriptContext::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

int LuaScriptContext::getOperatorPrecedence(char op) const {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

bool LuaScriptContext::isOperatorAssociative(char op) const {
    return op == '+' || op == '-' || op == '*' || op == '/';
}

std::string LuaScriptContext::infixToPostfix(const std::string& infix) {
    // Shunting-yard algorithm (simplified)
    std::stack<char> operatorStack;
    std::string output;
    
    for (char c : infix) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            output += c;
        } else if (isOperator(c)) {
            while (!operatorStack.empty() && isOperator(operatorStack.top()) &&
                   (getOperatorPrecedence(operatorStack.top()) >= getOperatorPrecedence(c))) {
                output += ' ';
                output += operatorStack.top();
                output += ' ';
                operatorStack.pop();
            }
            operatorStack.push(c);
        } else if (c == '(') {
            operatorStack.push(c);
        } else if (c == ')') {
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                output += ' ';
                output += operatorStack.top();
                output += ' ';
                operatorStack.pop();
            }
            if (!operatorStack.empty() && operatorStack.top() == '(') {
                operatorStack.pop();
            }
        }
    }
    
    while (!operatorStack.empty()) {
        if (operatorStack.top() == '(') {
            // Mismatched parentheses
            break;
        }
        output += ' ';
        output += operatorStack.top();
        output += ' ';
        operatorStack.pop();
    }
    
    return output;
}

// Gaussian Process implementation
void LuaScriptContext::BayesianOptimizer::GaussianProcess::addObservation(const std::vector<float>& x, float y) {
    X.push_back(x);
    y_vec.push_back(y);
    
    // Update kernel matrix
    size_t n = X.size();
    K = std::vector<std::vector<float>>(n, std::vector<float>(n, 0.0f));
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            float dist = 0.0f;
            for (size_t k = 0; k < x.size(); ++k) {
                float dx = X[i][k] - X[j][k];
                dist += dx * dx;
            }
            K[i][j] = std::exp(-0.5f * dist);
        }
    }
    
    // Cholesky decomposition (simplified)
    L = K;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i; j < n; ++j) {
            float sum = L[i][j];
            for (size_t k = 0; k < i; ++k) {
                sum -= L[i][k] * L[j][k];
            }
            if (i == j) {
                L[i][j] = std::sqrt(sum);
            } else {
                L[i][j] = sum / (L[i][i] + 1e-10f);
            }
        }
    }
    
    // Solve for alpha: K * alpha = y
    alpha = std::vector<float>(n, 0.0f);
    for (size_t i = 0; i < n; ++i) {
        float sum = y_vec[i];
        for (size_t k = 0; k < i; ++k) {
            sum -= L[i][k] * alpha[k];
        }
        alpha[i] = sum / L[i][i];
    }
}

float LuaScriptContext::BayesianOptimizer::GaussianProcess::predict(const std::vector<float>& x) const {
    if (X.empty()) return 0.0f;
    
    float mean = 0.0f;
    float var = 0.0f;
    
    // Predict mean
    for (size_t i = 0; i < X.size(); ++i) {
        float sum = 0.0f;
        for (size_t k = 0; k < x.size(); ++k) {
            float dx = X[i][k] - x[k];
            sum += dx * dx;
        }
        float k_star = std::exp(-0.5f * sum);
        mean += k_star * alpha[i];
        
        // Predict variance
        var += k_star * k_star;
    }
    
    return mean;
}

float LuaScriptContext::BayesianOptimizer::GaussianProcess::getUncertainty(const std::vector<float>& x) const {
    float uncertainty = 0.0f;
    
    for (size_t i = 0; i < X.size(); ++i) {
        float sum = 0.0f;
        for (size_t k = 0; k < x.size(); ++k) {
            float dx = X[i][k] - x[k];
            sum += dx * dx;
        }
        float k_star = std::exp(-0.5f * sum);
        uncertainty += k_star * k_star;
    }
    
    return uncertainty;
}

std::map<std::string, float> LuaScriptContext::BayesianOptimizer::optimize(size_t iterations) {
    std::map<std::string, float> bestResult;
    float bestScore = std::numeric_limits<float>::lowest();
    
    // Initialize with random parameters
    for (const auto& pair : parameters_) {
        bestResult[pair.first] = pair.second.currentValue;
    }
    
    // Simplified optimization loop
    for (size_t i = 0; i < iterations; ++i) {
        // Generate candidate parameters
        std::vector<float> candidateX;
        std::map<std::string, float> candidateParams;
        
        for (const auto& pair : parameters_) {
            std::uniform_real_distribution<> dis(pair.second.minValue, pair.second.maxValue);
            std::random_device rd;
            std::mt19937 gen(rd());
            float value = dis(gen);
            
            candidateParams[pair.first] = value;
            candidateX.push_back(value);
        }
        
        // Evaluate objective function
        float score = evaluateObjective(candidateParams);
        
        // Update best result
        if (score > bestScore) {
            bestScore = score;
            bestResult = candidateParams;
        }
        
        // Update Gaussian process
        gp_.addObservation(candidateX, score);
        
        // Select next point using acquisition function
        std::vector<float> nextX = selectNextPoint();
        
        if (nextX.empty()) {
            break; // No more points to evaluate
        }
        
        // Update parameters
        for (size_t j = 0; j < parameters_.size(); ++j) {
            parameters_.begin()->second.currentValue = nextX[j];
            std::advance(parameters_.begin(), 1);
        }
    }
    
    return bestResult;
}

float LuaScriptContext::BayesianOptimizer::evaluateObjective(const std::map<std::string, float>& params) const {
    // Simplified objective function
    float score = 0.0f;
    
    for (const auto& pair : parameters_) {
        float value = params.at(pair.first);
        
        // Simple objective: maximize value
        if (pair.second.objectiveFunction) {
            score += pair.second.objectiveFunction(params);
        } else {
            score += value;
        }
    }
    
    return score;
}

float LuaScriptContext::BayesianOptimizer::expectedImprovement(const std::vector<float>& x) const {
    float mean = gp_.predict(x);
    float uncertainty = gp_.getUncertainty(x);
    float currentBest = gp_.y_vec.empty() ? std::numeric_limits<float>::lowest() : *std::max_element(gp_.y_vec.begin(), gp_.y_vec.end());
    
    if (uncertainty < 1e-10f) return 0.0f;
    
    float z = (mean - currentBest) / std::sqrt(uncertainty);
    float ei = (mean - currentBest) * std::erf(z / std::sqrt(2.0f)) +
               uncertainty * std::exp(-0.5f * z * z) * std::sqrt(2.0f * M_PI);
    
    return ei;
}

float LuaScriptContext::BayesianOptimizer::probabilityOfImprovement(const std::vector<float>& x) const {
    float mean = gp_.predict(x);
    float uncertainty = gp_.getUncertainty(x);
    float currentBest = gp_.y_vec.empty() ? std::numeric_limits<float>::lowest() : *std::max_element(gp_.y_vec.begin(), gp_.y_vec.end());
    
    if (uncertainty < 1e-10f) return 0.0f;
    
    float z = (mean - currentBest) / std::sqrt(uncertainty);
    float pi = 0.5f * (1.0f + std::erf(z / std::sqrt(2.0f)));
    
    return pi;
}

std::vector<float> LuaScriptContext::BayesianOptimizer::selectNextPoint() {
    std::vector<float> bestX;
    float bestEI = std::numeric_limits<float>::lowest();
    
    // Random search for next point (simplified)
    for (size_t i = 0; i < 100; ++i) { // Try 100 random points
        std::vector<float> candidateX;
        float totalUncertainty = 0.0f;
        
        for (const auto& param : parameters_) {
            std::uniform_real_distribution<> dis(param.second.minValue, param.second.maxValue);
            std::random_device rd;
            std::mt19937 gen(rd());
            float value = dis(gen);
            
            candidateX.push_back(value);
            totalUncertainty += (value - param.second.currentValue) * (value - param.second.currentValue);
        }
        
        if (totalUncertainty < 1e-10f) continue; // Skip points too close to current
        
        float ei = expectedImprovement(candidateX);
        if (ei > bestEI) {
            bestEI = ei;
            bestX = candidateX;
        }
    }
    
    return bestX;
}

// Evolutionary Algorithm implementation
std::map<std::string, float> LuaScriptContext::EvolutionaryAlgorithm::optimize(size_t iterations) {
    std::map<std::string, float> bestResult;
    float bestFitness = std::numeric_limits<float>::lowest();
    
    // Initialize population
    std::vector<Individual> population;
    for (size_t i = 0; i < populationSize_; ++i) {
        population.push_back(createIndividual());
    }
    
    // Evolution loop
    for (size_t gen = 0; gen < maxGenerations_; ++gen) {
        // Evaluate fitness
        for (auto& individual : population) {
            individual.fitness = evaluateFitness(individual.genes);
            individual.timestamp = getCurrentTime();
            
            if (individual.fitness > bestFitness) {
                bestFitness = individual.fitness;
                bestResult = individual.genes;
            }
        }
        
        // Selection and reproduction
        std::vector<Individual> newPopulation;
        
        // Elitism: keep best individual
        Individual bestIndividual = *std::max_element(population.begin(), population.end(),
                                                       [](const Individual& a, const Individual& b) {
                                                           return a.fitness < b.fitness;
                                                       });
        newPopulation.push_back(bestIndividual);
        
        // Generate offspring
        while (newPopulation.size() < populationSize_) {
            // Tournament selection
            Individual parent1 = tournamentSelection(population);
            Individual parent2 = tournamentSelection(population);
            
            // Crossover
            Individual child = crossover(parent1, parent2);
            
            // Mutation
            child = mutate(child);
            
            newPopulation.push_back(child);
        }
        
        population = newPopulation;
    }
    
    return bestResult;
}

LuaScriptContext::EvolutionaryAlgorithm::Individual LuaScriptContext::EvolutionaryAlgorithm::createIndividual() const {
    Individual individual;
    
    for (const auto& pair : parameters_) {
        individual.genes[pair.first] = pair.second.currentValue;
    }
    
    return individual;
}

LuaScriptContext::EvolutionaryAlgorithm::Individual LuaScriptContext::EvolutionaryAlgorithm::crossover(const Individual& parent1, const Individual& parent2) const {
    Individual child;
    
    for (const auto& pair : parameters_) {
        const std::string& name = pair.first;
        float value1 = parent1.genes.at(name);
        float value2 = parent2.genes.at(name);
        
        // Simple one-point crossover
        if (rand() % 2 == 0) {
            child.genes[name] = value1;
        } else {
            child.genes[name] = value2;
        }
    }
    
    return child;
}

LuaScriptContext::EvolutionaryAlgorithm::Individual LuaScriptContext::EvolutionaryAlgorithm::mutate(const Individual& individual) const {
    Individual mutated = individual;
    
    for (auto& pair : mutated.genes) {
        const std::string& name = pair.first;
        
        // Apply mutation with probability
        if (rand() % 100 < static_cast<int>(mutationRate_ * 100)) {
            std::uniform_real_distribution<> dis(parameters_.at(name).minValue, parameters_.at(name).maxValue);
            std::random_device rd;
            std::mt19937 gen(rd());
            mutated.genes[name] = dis(gen);
        }
    }
    
    return mutated;
}

float LuaScriptContext::EvolutionaryAlgorithm::evaluateFitness(const std::map<std::string, float>& genes) const {
    // Simplified fitness function
    float fitness = 0.0f;
    
    for (const auto& pair : parameters_) {
        float value = genes.at(pair.first);
        
        // Simple fitness: maximize value
        if (pair.second.objectiveFunction) {
            std::map<std::string, float> params = parameters_;
            params[pair.first] = value;
            fitness += pair.second.objectiveFunction(params);
        } else {
            fitness += value;
        }
    }
    
    return fitness;
}

LuaScriptContext::EvolutionaryAlgorithm::Individual LuaScriptContext::EvolutionaryAlgorithm::tournamentSelection(const std::vector<Individual>& population) const {
    size_t tournamentSize = std::min<size_t>(3, population.size());
    size_t bestIndex = rand() % population.size();
    
    for (size_t i = 1; i < tournamentSize; ++i) {
        size_t candidateIndex = rand() % population.size();
        if (population[candidateIndex].fitness > population[bestIndex].fitness) {
            bestIndex = candidateIndex;
        }
    }
    
    return population[bestIndex];
}

// AdvancedConfigurationSystem implementation
double getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
}

void AdvancedConfigurationSystem::initializeDefaultTemplates() {
    // Initialize default configuration templates
    auto neuralConfig = std::make_shared<Config>();
    neuralConfig->setAuto("neuron_count", "1000");
    neuralConfig->setAuto("region_count", "1");
    neuralConfig->setAuto("connection_probability", "0.1");
    neuralConfig->setAuto("simulation_timestep", "0.001");
    neuralConfig->setAuto("random_seed", "42");
    
    configurationTemplates_["neural_network"] = neuralConfig;
    
    auto experimentalConfig = std::make_shared<Config>();
    experimentalConfig->setAuto("experiment_type", "learning");
    experimentalConfig->setAuto("reward_function", "standard");
    experimentalConfig->setAuto("reset_interval", "1000");
    
    configurationTemplates_["experimental"] = experimentalConfig;
}

void AdvancedConfigurationSystem::addConfigurationTemplate(const std::string& name, const std::shared_ptr<Config>& config) {
    if (!config) {
        throw std::runtime_error("Configuration cannot be null");
    }
    
    configurationTemplates_[name] = config;
}

std::shared_ptr<Config> AdvancedConfigurationSystem::getConfigurationTemplate(const std::string& name) const {
    auto it = configurationTemplates_.find(name);
    if (it != configurationTemplates_.end()) {
        return it->second;
    }
    return nullptr;
}

bool AdvancedConfigurationSystem::hasConfigurationTemplate(const std::string& name) const {
    return configurationTemplates_.find(name) != configurationTemplates_.end();
}

std::vector<std::string> AdvancedConfigurationSystem::getConfigurationTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : configurationTemplates_) {
        names.push_back(pair.first);
    }
    return names;
}

void AdvancedConfigurationSystem::addExperimentTemplate(const ExperimentTemplate& template_) {
    if (!validateExperiment(template_)) {
        throw std::runtime_error("Invalid experiment template");
    }
    
    experimentTemplates_[template_.name] = template_;
}

std::shared_ptr<AdvancedConfigurationSystem::ExperimentTemplate> AdvancedConfigurationSystem::getExperimentTemplate(const std::string& name) const {
    auto it = experimentTemplates_.find(name);
    if (it != experimentTemplates_.end()) {
        return std::make_shared<ExperimentTemplate>(it->second);
    }
    return nullptr;
}

bool AdvancedConfigurationSystem::hasExperimentTemplate(const std::string& name) const {
    return experimentTemplates_.find(name) != experimentTemplates_.end();
}

std::vector<std::string> AdvancedConfigurationSystem::getExperimentTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : experimentTemplates_) {
        names.push_back(pair.first);
    }
    return names;
}

AdvancedConfigurationSystem::ExperimentResult AdvancedConfigurationSystem::runExperiment(const std::string& templateName,
                                                                                    const std::map<std::string, float>& parameters) {
    auto template_ = getExperimentTemplate(templateName);
    if (!template_) {
        throw std::runtime_error("Experiment template not found: " + templateName);
    }
    
    return runExperiment(*template_, parameters);
}

AdvancedConfigurationSystem::ExperimentResult AdvancedConfigurationSystem::runExperiment(const ExperimentTemplate& template_,
                                                                                    const std::map<std::string, float>& parameters) {
    ExperimentResult result;
    result.experimentName = template_.name;
    result.startTime = getCurrentTime();
    
    try {
        // Apply parameter variations
        std::shared_ptr<Config> config = applyParameterVariations(template_, parameters);
        
        // In a real implementation, this would run the actual experiment
        // For now, we simulate experiment execution
        NLM_LOG_INFO("Running experiment: " + template_.name);
        
        // Simulate experiment work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Generate random results
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        result.finalPerformance = dis(gen);
        result.parameters = parameters;
        
        for (const auto& param : parameters) {
            result.metrics[param.first] = { param.second, result.finalPerformance };
        }
        
        result.status = "success";
        result.endTime = getCurrentTime();
        
        addExperimentResult(result);
        
    } catch (const std::exception& e) {
        result.status = "failed";
        result.errorMessage = std::string(e.what());
        result.endTime = getCurrentTime();
        
        addExperimentResult(result);
        throw;
    }
    
    return result;
}

ExperimentResult AdvancedConfigurationSystem::runScriptExperiment(const std::string& scriptName,
                                                                   const std::map<std::string, float>& parameters) {
    // Load and execute Lua script
    ExperimentResult result;
    result.experimentName = scriptName;
    result.startTime = getCurrentTime();
    
    try {
        executeLuaScriptFile(scriptName);
        
        // In a real implementation, this would extract results from the script execution
        result.finalPerformance = 0.5f; // Placeholder
        result.status = "success";
        result.endTime = getCurrentTime();
        
        addExperimentResult(result);
        
    } catch (const std::exception& e) {
        result.status = "failed";
        result.errorMessage = std::string(e.what());
        result.endTime = getCurrentTime();
        
        addExperimentResult(result);
        throw;
    }
    
    return result;
}

bool AdvancedConfigurationSystem::executeLuaScript(const std::string& script) {
    return luaContext_.executeScript(script);
}

bool AdvancedConfigurationSystem::executeLuaScriptFile(const std::string& filename) {
    try {
        luaContext_.loadScript(filename);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to execute Lua script: " + std::string(e.what()));
        return false;
    }
}

std::string AdvancedConfigurationSystem::evaluateLuaExpression(const std::string& expression) {
    try {
        float result = luaContext_.evaluate(expression);
        return std::to_string(result);
    } catch (const std::exception& e) {
        return "Error: " + std::string(e.what());
    }
}

OptimizationResult AdvancedConfigurationSystem::optimizeParameters(const std::string& objective,
                                                                  const std::map<std::string, OptimizationParameter>& parameters,
                                                                  const std::string& method) {
    OptimizationResult result;
    result.optimizationName = objective + "_optimization";
    result.startTime = getCurrentTime();
    result.optimizerType = method;
    
    std::map<std::string, float> bestParameters;
    
    try {
        if (method == "bayesian") {
            LuaScriptContext::BayesianOptimizer bayesianOptimizer;
            bayesianOptimizer.parameters_ = parameters;
            bayesianOptimizer.acquisitionFunctionWeight_ = 0.5f;
            
            // Create simple objective function
            auto objectiveFunc = [objective](const std::map<std::string, float>& params) {
                // Simplified objective: sum of parameters
                float score = 0.0f;
                for (const auto& param : params) {
                    score += param.second;
                }
                return score;
            };
            
            for (auto& param : bayesianOptimizer.parameters_) {
                param.second.objectiveFunction = objectiveFunc;
            }
            
            bestParameters = bayesianOptimizer.optimize(100);
        } else if (method == "evolutionary") {
            LuaScriptContext::EvolutionaryAlgorithm eaOptimizer;
            eaOptimizer.parameters_ = parameters;
            eaOptimizer.populationSize_ = 50;
            eaOptimizer.mutationRate_ = 0.1f;
            eaOptimizer.crossoverRate_ = 0.8f;
            eaOptimizer.maxGenerations_ = 50;
            
            // Create simple objective function
            auto objectiveFunc = [objective](const std::map<std::string, float>& params) {
                float score = 0.0f;
                for (const auto& param : params) {
                    score += param.second;
                }
                return score;
            };
            
            for (auto& param : eaOptimizer.parameters_) {
                param.second.objectiveFunction = objectiveFunc;
            }
            
            bestParameters = eaOptimizer.optimize(100);
        } else {
            throw std::runtime_error("Unsupported optimization method: " + method);
        }
        
        result.bestParameters = bestParameters;
        result.bestScore = evaluateObjective(bestParameters, objective);
        result.endTime = getCurrentTime();
        
        addOptimizationResult(result);
        
    } catch (const std::exception& e) {
        result.status = "failed";
        result.errorMessage = std::string(e.what());
        result.endTime = getCurrentTime();
        
        addOptimizationResult(result);
        throw;
    }
    
    return result;
}

OptimizationResult AdvancedConfigurationSystem::hyperparameterSearch(const std::string& objective,
                                                                    const std::map<std::string, std::vector<float>>& parameterGrids,
                                                                    const std::string& method) {
    OptimizationResult result;
    result.optimizationName = objective + "_search";
    result.startTime = getCurrentTime();
    result.optimizerType = method + "_search";
    
    std::map<std::string, float> bestParameters;
    float bestScore = std::numeric_limits<float>::lowest();
    
    try {
        if (method == "grid_search") {
            // Perform grid search
            std::vector<std::map<std::string, float>> parameterCombinations;
            generateParameterCombinations(parameterGrids, parameterCombinations);
            
            for (const auto& combo : parameterCombinations) {
                float score = evaluateObjective(combo, objective);
                
                if (score > bestScore) {
                    bestScore = score;
                    bestParameters = combo;
                }
                
                result.parameterHistory.push_back(combo);
                result.scoreHistory.push_back(score);
            }
        } else {
            throw std::runtime_error("Unsupported hyperparameter search method: " + method);
        }
        
        result.bestParameters = bestParameters;
        result.bestScore = bestScore;
        result.endTime = getCurrentTime();
        
        addOptimizationResult(result);
        
    } catch (const std::exception& e) {
        result.status = "failed";
        result.errorMessage = std::string(e.what());
        result.endTime = getCurrentTime();
        
        addOptimizationResult(result);
        throw;
    }
    
    return result;
}

AdvancedConfigurationSystem::ExperimentTemplate AdvancedConfigurationSystem::createAutomatedExperiment(const std::string& name,
                                                                                                 const std::string& objective,
                                                                                                 const std::map<std::string, std::pair<float, float>>& parameterRanges) {
    ExperimentTemplate template_;
    template_.name = name;
    template_.description = "Automated experiment for " + objective;
    template_.isValidated = true;
    
    // Create base config
    template_.baseConfig = std::make_shared<Config>();
    template_.baseConfig->setAuto("objective", objective);
    template_.baseConfig->setAuto("experiment_type", "automated");
    
    // Create parameter variations
    for (const auto& pair : parameterRanges) {
        const std::string& paramName = pair.first;
        float min = pair.second.first;
        float max = pair.second.second;
        
        template_.parameterVariations[paramName] = 
            [paramName, min, max](const std::map<std::string, float>& params) {
                auto config = std::make_shared<Config>();
                config->setAuto("objective", objective);
                
                for (const auto& p : params) {
                    config->setAuto(p.first, p.second);
                }
                
                return config;
            };
        
        template_.requiredVariables.push_back(paramName);
    }
    
    addExperimentTemplate(template_);
    
    return template_;
}

void AdvancedConfigurationSystem::scheduleExperiment(const ScheduledExperiment& scheduledExp) {
    scheduledExperiments_.push_back(scheduledExp);
}

void AdvancedConfigurationSystem::startScheduler() {
    if (schedulerRunning_) {
        return;
    }
    
    schedulerRunning_ = true;
    schedulerThread_ = std::thread(&AdvancedConfigurationSystem::runScheduledExperiments, this);
}

void AdvancedConfigurationSystem::stopScheduler() {
    schedulerRunning_ = false;
    if (schedulerThread_.joinable()) {
        schedulerThread_.join();
    }
}

void AdvancedConfigurationSystem::runScheduledExperiments() {
    while (schedulerRunning_) {
        for (auto& scheduledExp : scheduledExperiments_) {
            if (!scheduledExp.enabled) continue;
            
            auto now = std::chrono::system_clock::now();
            if (now >= scheduledExp.nextRun) {
                // Run scheduled experiment
                try {
                    ExperimentResult result = runScriptExperiment(scheduledExp.name, scheduledExp.parameters);
                    NLM_LOG_INFO("Scheduled experiment completed: " + scheduledExp.name);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Scheduled experiment failed: " + std::string(e.what()));
                }
                
                // Schedule next run (simplified - in practice, you'd use cron parsing)
                scheduledExp.nextRun = now + std::chrono::hours(24); // Daily
            }
        }
        
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

std::shared_ptr<Config> AdvancedConfigurationSystem::optimizeConfiguration(const std::string& objective,
                                                                         std::shared_ptr<Config> baseConfig,
                                                                         const std::map<std::string, std::pair<float, float>>& parameterRanges) {
    std::map<std::string, OptimizationParameter> optimizationParams;
    
    for (const auto& pair : parameterRanges) {
        OptimizationParameter param;
        param.name = pair.first;
        param.minValue = pair.second.first;
        param.maxValue = pair.second.second;
        
        if (baseConfig && baseConfig->has(pair.first)) {
            try {
                param.currentValue = std::stof(baseConfig->getString(pair.first));
            } catch (...) {
                param.currentValue = (param.minValue + param.maxValue) / 2.0f;
            }
        } else {
            param.currentValue = (param.minValue + param.maxValue) / 2.0f;
        }
        
        optimizationParams[param.name] = param;
    }
    
    // Create objective function
    auto objectiveFunc = [objective](const std::map<std::string, float>& params) {
        // Simplified objective: sum of weighted parameters
        float score = 0.0f;
        for (const auto& param : params) {
            score += param.second;
        }
        return score / static_cast<float>(params.size());
    };
    
    for (auto& param : optimizationParams) {
        param.second.objectiveFunction = objectiveFunc;
    }
    
    OptimizationResult result = optimizeParameters(objective, optimizationParams, "bayesian");
    
    // Create optimized config
    auto optimizedConfig = std::make_shared<Config>(*baseConfig);
    
    for (const auto& pair : result.bestParameters) {
        optimizedConfig->setAuto(pair.first, std::to_string(pair.second));
    }
    
    return optimizedConfig;
}

bool AdvancedConfigurationSystem::validateExperiment(const ExperimentTemplate& template_) const {
    if (template_.name.empty()) {
        return false;
    }
    
    if (!template_.baseConfig) {
        return false;
    }
    
    // Check required variables
    for (const auto& var : template_.requiredVariables) {
        if (template_.baseConfig->getString(var).empty()) {
            return false;
        }
    }
    
    return true;
}

bool AdvancedConfigurationSystem::validateConfiguration(const Config& config) const {
    // Basic validation
    if (config.getKeys().empty()) {
        return false;
    }
    
    // Check for valid parameter types
    for (const auto& key : config.getKeys()) {
        std::string value = config.getString(key);
        std::string type = config.getType(key);
        
        if (!validateParameterType(type, value)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> AdvancedConfigurationSystem::getConfigurationErrors(const Config& config) const {
    std::vector<std::string> errors;
    
    for (const auto& key : config.getKeys()) {
        std::string value = config.getString(key);
        std::string type = config.getType(key);
        
        if (!validateParameterType(type, value)) {
            errors.push_back("Invalid type for parameter '" + key + "': expected " + type + ", got '" + value + "'");
        }
    }
    
    return errors;
}

void AdvancedConfigurationSystem::addExperimentResult(const ExperimentResult& result) {
    experimentHistory_.push_back(result);
    
    // Keep only recent results
    if (experimentHistory_.size() > 1000) {
        experimentHistory_.erase(experimentHistory_.begin());
    }
}

AdvancedConfigurationSystem::ExperimentResult AdvancedConfigurationSystem::getLastExperimentResult(const std::string& experimentName) const {
    ExperimentResult result;
    
    for (auto it = experimentHistory_.rbegin(); it != experimentHistory_.rend(); ++it) {
        if (it->experimentName == experimentName) {
            result = *it;
            break;
        }
    }
    
    return result;
}

std::vector<AdvancedConfigurationSystem::ExperimentResult> AdvancedConfigurationSystem::getExperimentHistory(const std::string& experimentName) const {
    std::vector<ExperimentResult> history;
    
    for (const auto& result : experimentHistory_) {
        if (result.experimentName == experimentName) {
            history.push_back(result);
        }
    }
    
    return history;
}

void AdvancedConfigurationSystem::addOptimizationResult(const OptimizationResult& result) {
    optimizationHistory_.push_back(result);
    
    // Keep only recent results
    if (optimizationHistory_.size() > 100) {
        optimizationHistory_.erase(optimizationHistory_.begin());
    }
}

AdvancedConfigurationSystem::OptimizationResult AdvancedConfigurationSystem::getLastOptimizationResult(const std::string& optimizationName) const {
    OptimizationResult result;
    
    for (auto it = optimizationHistory_.rbegin(); it != optimizationHistory_.rend(); ++it) {
        if (it->optimizationName == optimizationName) {
            result = *it;
            break;
        }
    }
    
    return result;
}

std::vector<AdvancedConfigurationSystem::OptimizationResult> AdvancedConfigurationSystem::getOptimizationHistory(const std::string& optimizationName) const {
    std::vector<OptimizationResult> history;
    
    for (const auto& result : optimizationHistory_) {
        if (result.optimizationName == optimizationName) {
            history.push_back(result);
        }
    }
    
    return history;
}

bool AdvancedConfigurationSystem::exportConfiguration(const std::string& filename, const std::string& name) const {
    auto config = getConfigurationTemplate(name);
    if (!config) {
        return false;
    }
    
    return config->saveToFile(filename);
}

bool AdvancedConfigurationSystem::importConfiguration(const std::string& filename) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filename)) {
        return false;
    }
    
    // Add to templates with filename as name
    std::string name = filename.substr(filename.find_last_of("/\\") + 1);
    addConfigurationTemplate(name, config);
    
    return true;
}

bool AdvancedConfigurationSystem::exportExperimentResults(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Experiment Results\n";
        file << "===================\n\n";
        
        for (const auto& result : experimentHistory_) {
            file << "Experiment: " << result.experimentName << "\n";
            file << "Status: " << result.status << "\n";
            file << "Performance: " << result.finalPerformance << "\n";
            file << "Duration: " << (result.endTime - result.startTime) << " seconds\n";
            
            if (!result.errorMessage.empty()) {
                file << "Error: " << result.errorMessage << "\n";
            }
            
            file << "\n";
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export experiment results: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfigurationSystem::importExperimentResults(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        ExperimentResult currentResult;
        
        while (std::getline(file, line)) {
            if (line.find("Experiment: ") == 0) {
                if (!currentResult.experimentName.empty()) {
                    addExperimentResult(currentResult);
                }
                currentResult = ExperimentResult();
                currentResult.experimentName = line.substr(13);
            } else if (line.find("Status: ") == 0) {
                currentResult.status = line.substr(8);
            } else if (line.find("Performance: ") == 0) {
                currentResult.finalPerformance = std::stof(line.substr(12));
            } else if (line.find("Duration: ") == 0) {
                // Parse duration (simplified)
            } else if (line.find("Error: ") == 0) {
                currentResult.errorMessage = line.substr(7);
            } else if (line.empty()) {
                if (!currentResult.experimentName.empty()) {
                    addExperimentResult(currentResult);
                    currentResult = ExperimentResult();
                }
            }
        }
        
        if (!currentResult.experimentName.empty()) {
            addExperimentResult(currentResult);
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import experiment results: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfigurationSystem::exportOptimizationResults(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Optimization Results\n";
        file << "=====================\n\n";
        
        for (const auto& result : optimizationHistory_) {
            file << "Optimization: " << result.optimizationName << "\n";
            file << "Method: " << result.optimizerType << "\n";
            file << "Best Score: " << result.bestScore << "\n";
            file << "Duration: " << (result.endTime - result.startTime) << " seconds\n";
            
            file << "Best Parameters:\n";
            for (const auto& pair : result.bestParameters) {
                file << "  " << pair.first << ": " << pair.second << "\n";
            }
            
            file << "\n";
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export optimization results: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfigurationSystem::importOptimizationResults(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        OptimizationResult currentResult;
        
        while (std::getline(file, line)) {
            if (line.find("Optimization: ") == 0) {
                if (!currentResult.optimizationName.empty()) {
                    addOptimizationResult(currentResult);
                }
                currentResult = OptimizationResult();
                currentResult.optimizationName = line.substr(14);
            } else if (line.find("Method: ") == 0) {
                currentResult.optimizerType = line.substr(8);
            } else if (line.find("Best Score: ") == 0) {
                currentResult.bestScore = std::stof(line.substr(11));
            } else if (line.find("Best Parameters:") == 0) {
                // Parse parameters (simplified)
            }
        }
        
        if (!currentResult.optimizationName.empty()) {
            addOptimizationResult(currentResult);
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import optimization results: " + std::string(e.what()));
        return false;
    }
}

float AdvancedConfigurationSystem::compareConfigurations(const Config& config1, const Config& config2) const {
    float similarity = 0.0f;
    
    std::vector<std::string> keys1 = config1.getKeys();
    std::vector<std::string> keys2 = config2.getKeys();
    
    for (const auto& key : keys1) {
        if (config2.has(key)) {
            std::string value1 = config1.getString(key);
            std::string value2 = config2.getString(key);
            
            if (value1 == value2) {
                similarity += 1.0f;
            }
        }
    }
    
    return similarity / std::max<size_t>(keys1.size(), keys2.size());
}

std::string AdvancedConfigurationSystem::getConfigurationDifferences(const Config& config1, const Config& config2) const {
    std::string differences;
    
    std::vector<std::string> keys1 = config1.getKeys();
    std::vector<std::string> keys2 = config2.getKeys();
    
    for (const auto& key : keys1) {
        if (!config2.has(key)) {
            differences += "Config1 has parameter '" + key + "' which Config2 does not have.\n";
        } else if (config1.getString(key) != config2.getString(key)) {
            differences += "Parameter '" + key + "' differs: Config1 = '" + config1.getString(key) +
                           "', Config2 = '" + config2.getString(key) + "'.\n";
        }
    }
    
    for (const auto& key : keys2) {
        if (!config1.has(key)) {
            differences += "Config2 has parameter '" + key + "' which Config1 does not have.\n";
        }
    }
    
    if (differences.empty()) {
        differences = "Configurations are identical.";
    }
    
    return differences;
}

void AdvancedConfigurationSystem::analyzeExperimentResults(const std::string& experimentName) const {
    auto history = getExperimentHistory(experimentName);
    
    if (history.empty()) {
        NLM_LOG_WARN("No experiment results found for: " + experimentName);
        return;
    }
    
    float totalPerformance = 0.0f;
    size_t successCount = 0;
    
    for (const auto& result : history) {
        totalPerformance += result.finalPerformance;
        if (result.status == "success") {
            successCount++;
        }
    }
    
    float averagePerformance = totalPerformance / static_cast<float>(history.size());
    float successRate = static_cast<float>(successCount) / static_cast<float>(history.size()) * 100.0f;
    
    NLM_LOG_INFO("Experiment Analysis for " + experimentName + ":");
    NLM_LOG_INFO("  Number of runs: " + std::to_string(history.size()));
    NLM_LOG_INFO("  Average performance: " + std::to_string(averagePerformance));
    NLM_LOG_INFO("  Success rate: " + std::to_string(successRate) + "%");
    NLM_LOG_INFO("  Total duration: " + std::to_string(getTotalDuration(history)) + " seconds");
}

void AdvancedConfigurationSystem::analyzeOptimizationResults(const std::string& optimizationName) const {
    auto history = getOptimizationHistory(optimizationName);
    
    if (history.empty()) {
        NLM_LOG_WARN("No optimization results found for: " + optimizationName);
        return;
    }
    
    float totalScore = 0.0f;
    for (const auto& result : history) {
        totalScore += result.bestScore;
    }
    
    float averageScore = totalScore / static_cast<float>(history.size());
    
    NLM_LOG_INFO("Optimization Analysis for " + optimizationName + ":");
    NLM_LOG_INFO("  Number of runs: " + std::to_string(history.size()));
    NLM_LOG_INFO("  Average score: " + std::to_string(averageScore));
    NLM_LOG_INFO("  Best score: " + std::to_string(getBestScore(history)));
}

bool AdvancedConfigurationSystem::validateParameterType(const std::string& type, const std::string& value) {
    if (type == "int") {
        return std::all_of(value.begin(), value.end(), [](char c) { return std::isdigit(c) || c == '-'; }) ||
               (value.size() > 0 && value[0] == '-' && value.size() > 1 &&
                std::all_of(value.begin() + 1, value.end(), [](char c) { return std::isdigit(c); }));
    } else if (type == "float") {
        return std::regex_match(value, std::regex("-?\d+(\.\d+)?")) ||
               std::regex_match(value, std::regex("-?\d*\.\d+"));
    } else if (type == "bool") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    } else if (type == "string") {
        return true; // Any non-empty string is valid
    }
    
    return false;
}

bool AdvancedConfigurationSystem::validateParameterRange(const std::string& type, float value,
                                                        const std::pair<float, float>& range) {
    if (type == "int") {
        return static_cast<int>(value) >= static_cast<int>(range.first) &&
               static_cast<int>(value) <= static_cast<int>(range.second);
    } else if (type == "float") {
        return value >= range.first && value <= range.second;
    }
    
    return true; // No range validation for other types
}

std::string AdvancedConfigurationSystem::parameterToString(const std::map<std::string, float>& parameters) {
    std::stringstream ss;
    ss << "{";
    
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        if (it != parameters.begin()) ss << ", ";
        ss << "\"" << it->first << "\": " << it->second;
    }
    
    ss << "}";
    return ss.str();
}

std::map<std::string, float> AdvancedConfigurationSystem::stringToParameters(const std::string& str) {
    std::map<std::string, float> parameters;
    
    if (str.empty() || str[0] != '{' || str.back() != '}') {
        return parameters;
    }
    
    std::string content = str.substr(1, str.size() - 2);
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        size_t colonPos = item.find(':');
        if (colonPos != std::string::npos) {
            std::string key = item.substr(0, colonPos);
            std::string value = item.substr(colonPos + 1);
            
            // Remove quotes from key
            key.erase(0, key.find_first_not_of('"'));
            key.erase(key.find_last_not_of('"') + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            try {
                parameters[key] = std::stof(value);
            } catch (...) {
                // Skip invalid values
            }
        }
    }
    
    return parameters;
}

float AdvancedConfigurationSystem::evaluateObjective(const std::map<std::string, float>& params,
                                                     const std::string& objective) {
    // Simplified objective functions
    if (objective == "maximize_sum") {
        float sum = 0.0f;
        for (const auto& param : params) {
            sum += param.second;
        }
        return sum;
    } else if (objective == "maximize_product") {
        float product = 1.0f;
        for (const auto& param : params) {
            product *= param.second;
        }
        return product;
    } else if (objective == "balance") {
        float sum = 0.0f;
        for (const auto& param : params) {
            sum += param.second;
        }
        float mean = sum / static_cast<float>(params.size());
        float variance = 0.0f;
        
        for (const auto& param : params) {
            float diff = param.second - mean;
            variance += diff * diff;
        }
        
        return mean / (variance + 1.0f); // Prefer balanced parameters
    } else {
        // Default objective
        float score = 0.0f;
        for (const auto& param : params) {
            score += param.second;
        }
        return score / static_cast<float>(params.size());
    }
}

void AdvancedConfigurationSystem::generateParameterCombinations(
    const std::map<std::string, std::vector<float>>& parameterGrids,
    std::vector<std::map<std::string, float>>& combinations) {
    if (parameterGrids.empty()) {
        combinations.push_back(std::map<std::string, float>());
        return;
    }
    
    std::vector<std::string> keys;
    std::vector<std::vector<float>> values;
    
    for (const auto& pair : parameterGrids) {
        keys.push_back(pair.first);
        values.push_back(pair.second);
    }
    
    generateCombinationsRecursive(0, keys, values, std::map<std::string, float>(), combinations);
}

void AdvancedConfigurationSystem::generateCombinationsRecursive(
    size_t index, const std::vector<std::string>& keys,
    const std::vector<std::vector<float>>& values,
    std::map<std::string, float> current,
    std::vector<std::map<std::string, float>>& combinations) {
    if (index >= keys.size()) {
        combinations.push_back(current);
        return;
    }
    
    for (float value : values[index]) {
        current[keys[index]] = value;
        generateCombinationsRecursive(index + 1, keys, values, current, combinations);
    }
}

double AdvancedConfigurationSystem::getTotalDuration(const std::vector<ExperimentResult>& results) const {
    double totalDuration = 0.0;
    
    for (const auto& result : results) {
        totalDuration += (result.endTime - result.startTime);
    }
    
    return totalDuration;
}

float AdvancedConfigurationSystem::getBestScore(const std::vector<OptimizationResult>& results) const {
    float bestScore = std::numeric_limits<float>::lowest();
    
    for (const auto& result : results) {
        if (result.bestScore > bestScore) {
            bestScore = result.bestScore;
        }
    }
    
    return bestScore;
}

} // namespace AdvancedConfiguration
} // namespace nlm