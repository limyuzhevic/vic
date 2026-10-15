// NLM Scripting System
// Extensible scripting interface for expert users
// Allows custom brain control and automation

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <functional>
#include <stack>
#include <algorithm>
#include <cctype>

using namespace nlm;
using namespace std;

// Forward declarations
class ScriptContext;
class NLMScript;
class ScriptException;

// Script exception for error handling
class ScriptException : public std::runtime_error {
public:
    ScriptException(const std::string& message, const std::string& scriptName = "", int lineNumber = 0)
        : std::runtime_error(message), scriptName(scriptName), lineNumber(lineNumber) {}
    
    std::string scriptName;
    int lineNumber;
};

// Type definitions for script values
class ScriptValue {
public:
    enum Type {
        NULL_TYPE,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        LIST,
        BRAIN,
        CONFIG,
        WORLD,
        AGENT
    };
    
private:
    Type type;
    
    // Union for stored values
    union ValueUnion {
        bool boolean;
        int64_t integer;
        double float_val;
        std::string* string;
        std::vector<ScriptValue>* list;
        std::shared_ptr<Brain>* brain;
        std::shared_ptr<Config>* config;
        std::shared_ptr<SimpleWorld>* world;
        std::shared_ptr<AgentBrain>* agent;
        
        ValueUnion() : string(nullptr) {}
        ~ValueUnion() {}
    } value;
    
public:
    ScriptValue() : type(NULL_TYPE) {}
    ~ScriptValue() { clear(); }
    
    void clear() {
        switch (type) {
            case STRING: delete value.string; break;
            case LIST: delete value.list; break;
            case BRAIN: delete value.brain; break;
            case CONFIG: delete value.config; break;
            case WORLD: delete value.world; break;
            case AGENT: delete value.agent; break;
            default: break;
        }
        type = NULL_TYPE;
        new (&value) ValueUnion();
    }
    
    // Type conversion constructors
    ScriptValue(bool v) { type = BOOLEAN; value.boolean = v; }
    ScriptValue(int64_t v) { type = INTEGER; value.integer = v; }
    ScriptValue(double v) { type = FLOAT; value.float_val = v; }
    ScriptValue(const std::string& v) { type = STRING; value.string = new std::string(v); }
    ScriptValue(const std::vector<ScriptValue>& v) { type = LIST; value.list = new std::vector<ScriptValue>(v); }
    
    // Setters
    void setBrain(std::shared_ptr<Brain> b) {
        clear();
        type = BRAIN;
        value.brain = new std::shared_ptr<Brain>(b);
    }
    
    void setConfig(std::shared_ptr<Config> c) {
        clear();
        type = CONFIG;
        value.config = new std::shared_ptr<Config>(c);
    }
    
    void setWorld(std::shared_ptr<SimpleWorld> w) {
        clear();
        type = WORLD;
        value.world = new std::shared_ptr<SimpleWorld>(w);
    }
    
    void setAgent(std::shared_ptr<AgentBrain> a) {
        clear();
        type = AGENT;
        value.agent = new std::shared_ptr<AgentBrain>(a);
    }
    
    // Getters
    Type getType() const { return type; }
    
    bool getBoolean() const { return type == BOOLEAN ? value.boolean : false; }
    int64_t getInteger() const { return type == INTEGER ? value.integer : 0; }
    double getFloat() const { return type == FLOAT ? value.float_val : 0.0; }
    std::string getString() const { 
        return type == STRING ? *value.string : ""; 
    }
    const std::vector<ScriptValue>& getList() const {
        return type == LIST ? *value.list : *(new std::vector<ScriptValue>());
    }
    std::shared_ptr<Brain> getBrain() const { 
        return type == BRAIN ? *value.brain : nullptr; 
    }
    std::shared_ptr<Config> getConfig() const { 
        return type == CONFIG ? *value.config : nullptr; 
    }
    std::shared_ptr<SimpleWorld> getWorld() const { 
        return type == WORLD ? *value.world : nullptr; 
    }
    std::shared_ptr<AgentBrain> getAgent() const { 
        return type == AGENT ? *value.agent : nullptr; 
    }
    
    // Type checking
    bool isNull() const { return type == NULL_TYPE; }
    bool isBoolean() const { return type == BOOLEAN; }
    bool isInteger() const { return type == INTEGER; }
    bool isFloat() const { return type == FLOAT; }
    bool isString() const { return type == STRING; }
    bool isList() const { return type == LIST; }
    bool isBrain() const { return type == BRAIN; }
    bool isConfig() const { return type == CONFIG; }
    bool isWorld() const { return type == WORLD; }
    bool isAgent() const { return type == AGENT; }
    
    // Arithmetic operations
    ScriptValue operator+(const ScriptValue& other) const {
        if (type == INTEGER && other.type == INTEGER) {
            return ScriptValue(value.integer + other.getInteger());
        } else if (type == FLOAT && other.type == FLOAT) {
            return ScriptValue(value.float_val + other.getFloat());
        }
        return ScriptValue(0.0);
    }
    
    ScriptValue operator-(const ScriptValue& other) const {
        if (type == INTEGER && other.type == INTEGER) {
            return ScriptValue(value.integer - other.getInteger());
        } else if (type == FLOAT && other.type == FLOAT) {
            return ScriptValue(value.float_val - other.getFloat());
        }
        return ScriptValue(0.0);
    }
    
    ScriptValue operator*(const ScriptValue& other) const {
        if (type == INTEGER && other.type == INTEGER) {
            return ScriptValue(value.integer * other.getInteger());
        } else if (type == FLOAT && other.type == FLOAT) {
            return ScriptValue(value.float_val * other.getFloat());
        }
        return ScriptValue(0.0);
    }
    
    ScriptValue operator/(const ScriptValue& other) const {
        if (type == INTEGER && other.type == INTEGER && other.getInteger() != 0) {
            return ScriptValue(value.integer / other.getInteger());
        } else if (type == FLOAT && other.type == FLOAT && other.getFloat() != 0) {
            return ScriptValue(value.float_val / other.getFloat());
        }
        return ScriptValue(0.0);
    }
    
    // Comparison operators
    bool operator==(const ScriptValue& other) const {
        if (type != other.type) return false;
        switch (type) {
            case BOOLEAN: return value.boolean == other.value.boolean;
            case INTEGER: return value.integer == other.value.integer;
            case FLOAT: return value.float_val == other.value.float_val;
            case STRING: return *value.string == *other.value.string;
            default: return false;
        }
    }
    
    bool operator!=(const ScriptValue& other) const {
        return !(*this == other);
    }
    
    bool operator<(const ScriptValue& other) const {
        if (type != other.type) return false;
        switch (type) {
            case INTEGER: return value.integer < other.value.integer;
            case FLOAT: return value.float_val < other.value.float_val;
            case STRING: return *value.string < *other.value.string;
            default: return false;
        }
    }
}; 

// Simple tokenizer for script parsing
class ScriptTokenizer {
private:
    std::string script;
    size_t position;
    
public:
    ScriptTokenizer(const std::string& script) : script(script), position(0) {}
    
    // Skip whitespace
    void skipWhitespace() {
        while (position < script.length() && std::isspace(script[position])) {
            position++;
        }
    }
    
    // Check if current position is at end of script
    bool isAtEnd() {
        skipWhitespace();
        return position >= script.length();
    }
    
    // Get current character
    char peek() {
        if (position >= script.length()) return '\0';
        return script[position];
    }
    
    // Advance position
    char advance() {
        skipWhitespace();
        return position < script.length() ? script[position++] : '\0';
    }
    
    // Get current token
    std::string token() {
        size_t start = position;
        
        while (position < script.length() && 
               !std::isspace(script[position]) && 
               script[position] != ';' && 
               script[position] != '{' && 
               script[position] != '}' && 
               script[position] != '(' && 
               script[position] != ')' && 
               script[position] != '=' && 
               script[position] != '+' && 
               script[position] != '-' && 
               script[position] != '*' && 
               script[position] != '/' && 
               script[position] != ',' && 
               script[position] != '"' && 
               script[position] != '\'') {
            position++;
        }
        
        return script.substr(start, position - start);
    }
    
    // Check if next token is a specific keyword
    bool check(const std::string& word) {
        skipWhitespace();
        return position + word.length() < script.length() &&
               script.substr(position, word.length()) == word;
    }
    
    // Match and consume keyword
    bool match(const std::string& word) {
        if (!check(word)) return false;
        position += word.length();
        skipWhitespace();
        return true;
    }
}; 

// Simple interpreter for script execution
class ScriptInterpreter {
private:
    struct Variable {
        std::string name;
        ScriptValue value;
        bool isGlobal;
    };
    
    std::vector<Variable> variables;
    ScriptContext* context;
    
public:
    ScriptInterpreter(ScriptContext* ctx) : context(ctx) {}
    
    void execute(const std::string& script) {
        NLM_LOG_INFO("Executing script: " + script);
        
        ScriptTokenizer tokenizer(script);
        std::stack<ScriptValue> valueStack;
        
        while (!tokenizer.isAtEnd()) {
            char c = tokenizer.peek();
            
            if (c == ';') {
                // Statement separator
                tokenizer.advance();
                // TODO: Process valueStack
                valueStack = std::stack<ScriptValue>();
            } else if (c == ' ') {
                tokenizer.advance();
            } else if (c == '=') {
                // Assignment
                tokenizer.advance();
                std::string varName = tokenizer.token();
                
                // Simple expression evaluation (placeholder)
                ScriptValue exprValue;
                if (tokenizer.check("brain")) {
                    exprValue.setBrain(context->getBrain());
                    tokenizer.match("brain");
                } else if (tokenizer.check("config")) {
                    exprValue.setConfig(context->getConfig());
                    tokenizer.match("config");
                }
                
                // Store variable
                variables.push_back({varName, exprValue, true});
            } else {
                // Simple value
                std::string token = tokenizer.token();
                if (token == "true" || token == "false") {
                    valueStack.push(ScriptValue(token == "true"));
                } else if (std::all_of(token.begin(), token.end(), ::isdigit) ||
                          (token[0] == '-' && std::all_of(token.begin() + 1, token.end(), ::isdigit))) {
                    valueStack.push(ScriptValue(std::stoll(token)));
                } else if (token.find('.') != std::string::npos && 
                          std::all_of(token.begin(), token.end(), [](char c) {
                              return std::isdigit(c) || c == '.' || c == '-'; 
                          })) {
                    valueStack.push(ScriptValue(std::stod(token)));
                } else {
                    valueStack.push(ScriptValue(token));
                }
            }
        }
        
        NLM_LOG_INFO("Script execution complete");
    }
    
    // Get variable by name
    ScriptValue getVariable(const std::string& name) {
        auto it = std::find_if(variables.begin(), variables.end(),
                               [&name](const Variable& v) { return v.name == name; });n        
        return it != variables.end() ? it->value : ScriptValue();
    }
}; 

// Script context for variable and function management
class ScriptContext {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    std::string scriptName;
    
public:
    ScriptContext(const std::string& name = "default")
        : scriptName(name) {}
    
    void setBrain(std::shared_ptr<Brain> b) { brain = b; }
    void setConfig(std::shared_ptr<Config> c) { config = c; }
    void setWorld(std::shared_ptr<SimpleWorld> w) { world = w; }
    void setAgent(std::shared_ptr<AgentBrain> a) { agent = a; }
    
    std::shared_ptr<Brain> getBrain() const { return brain; }
    std::shared_ptr<Config> getConfig() const { return config; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world; }
    std::shared_ptr<AgentBrain> getAgent() const { return agent; }
    
    const std::string& getScriptName() const { return scriptName; }
}; 

// Main scripting interface
class NLMScriptingSystem {
private:
    ScriptContext context;
    ScriptInterpreter interpreter;
    
public:
    NLMScriptingSystem(ScriptContext ctx) : context(ctx), interpreter(&context) {}
    
    void loadScript(const std::string& filename) {
        NLM_LOG_INFO("Loading script: " + filename);
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw ScriptException("Cannot open script file: " + filename);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        std::string script = buffer.str();
        interpreter.execute(script);
        
        NLM_LOG_INFO("Script loaded successfully");
    }
    
    void createBrain(const std::string& name, const std::string& configFile = "") {
        NLM_LOG_INFO("Creating brain: " + name);
        
        auto brainConfig = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            if (!brainConfig->loadFromFile(configFile)) {
                NLM_LOG_WARNING("Failed to load config from: " + configFile);
            }
        }
        
        auto brain = std::make_shared<Brain>(brainConfig);
        if (brain->initialize()) {
            context.setBrain(brain);
            NLM_LOG_INFO("Brain created successfully");
        } else {
            throw ScriptException("Failed to initialize brain");
        }
    }
    
    void createWorld(const std::string& name, int width, int height) {
        NLM_LOG_INFO("Creating world: " + name + " (" + std::to_string(width) + "x" + std::to_string(height) + ")");
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        context.setWorld(world);
        NLM_LOG_INFO("World created successfully");
    }
    
    void createAgent(const std::string& name) {
        NLM_LOG_INFO("Creating agent: " + name);
        
        if (!context.getBrain()) {
            throw ScriptException("No brain available for agent creation");
        }
        
        auto agent = std::make_shared<AgentBrain>(context.getBrain());
        agent->initialize(*context.getWorld());
        
        context.setAgent(agent);
        NLM_LOG_INFO("Agent created successfully");
    }
    
    void runSimulation(int steps, float timestep = 0.001f) {
        NLM_LOG_INFO("Running simulation: " + std::to_string(steps) + " steps");
        
        if (!context.getBrain()) {
            throw ScriptException("No brain available for simulation");
        }
        
        for (int step = 0; step < steps; ++step) {
            // Inject sensory input if available
            if (context.getAgent()) {
                auto percept = context.getWorld()->getSensoryPercept();
                context.getAgent()->processSensoryInput(percept);
            }
            
            // Step the brain
            context.getBrain()->step(step, step * timestep);
            
            // Get motor command if agent available
            if (context.getAgent()) {
                auto action = context.getAgent()->decodeMotorCommand();
                context.getWorld()->applyMotorCommand(action, context.getWorld()->getSimulationTime());
            }
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(context.getBrain()->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(context.getBrain()->getFiringNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("Simulation complete");
    }
    
    void exportState(const std::string& filename) {
        NLM_LOG_INFO("Exporting state to: " + filename);
        
        if (context.getBrain()) {
            // Create exports directory if it doesn't exist
            system(("mkdir -p exports/").c_str());
            
            context.getBrain()->save(filename);
            NLM_LOG_INFO("State exported successfully");
        } else {
            throw ScriptException("No brain available for export");
        }
    }
    
    void importState(const std::string& filename) {
        NLM_LOG_INFO("Importing state from: " + filename);
        
        if (context.getBrain() && context.getBrain()->load(filename)) {
            NLM_LOG_INFO("State imported successfully");
        } else {
            throw ScriptException("Failed to import state");
        }
    }
}; 

// Example script functions for common operations
void demonstrateScripting(NLMScriptingSystem& scripting) {
    NLM_LOG_INFO("=== NLM Scripting System Demonstration ===");
    
    // Create a brain with custom configuration
    scripting.createBrain("demo_brain");
    
    // Create a world
    scripting.createWorld("demo_world", 20, 20);
    
    // Create an agent
    scripting.createAgent("demo_agent");
    
    // Run a simple simulation
    scripting.runSimulation(100);
    
    // Export the results
    scripting.exportState("demo_brain_state.nlm");
    
    NLM_LOG_INFO("Scripting demonstration complete");
}

// Initialize the scripting system
void initializeScriptingSystem() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("NLM Scripting System initialized");
    
    // Create script context
    ScriptContext context("demo_script");
    
    // Create scripting system
    NLMScriptingSystem scripting(context);
    
    // Run demonstration
    demonstrateScripting(scripting);
}
