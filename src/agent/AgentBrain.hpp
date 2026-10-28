#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Scripting system for custom neural behaviors
    struct ScriptCommand {
        enum Type {
            SET_PARAMETER,
            SET_NEURON_TYPE,
            SET_SYNAPSE_WEIGHT,
            ENABLE_PLASTICITY,
            DISABLE_PLASTICITY,
            SET_DEVELOPMENT_STAGE,
            SET_NEUROMODULATION,
            SET_CURIOCITY,
            SET_NOVELTY,
            SET_PREDICTION_ERROR,
            SET_REWARD_FUNCTION,
            SET_LEARNING_RATE,
            ADD_EVENT_HANDLER,
            REMOVE_EVENT_HANDLER,
            EXECUTE_SCRIPT,
            JUMP_IF_TRUE,
            JUMP_IF_FALSE,
            CALL_FUNCTION,
            RETURN,
            COMMENT
        };
        
        Type type;
        std::string target;
        std::string value;
        std::string condition;
        std::string script;
        
        ScriptCommand(Type t, const std::string& tgt, const std::string& val = "")
            : type(t), target(tgt), value(val), condition(""), script("") {}
        
        ScriptCommand(Type t, const std::string& tgt, const std::string& val, const std::string& cond)
            : type(t), target(tgt), value(val), condition(cond), script("") {}
    };
    
    // Script interpreter for neural network behaviors
    class ScriptEngine {
    public:
        ScriptEngine();
        ~ScriptEngine();
        
        // Load script from file
        bool loadScript(const std::string& filename);
        
        // Execute script with parameters
        bool execute(const std::string& scriptName, const std::vector<std::string>& parameters);
        
        // Execute script from string
        bool executeString(const std::string& script);
        
        // Add script command
        void addCommand(const ScriptCommand& cmd);
        
        // Get script variable
        std::string getVariable(const std::string& name) const;
        
        // Set script variable
        void setVariable(const std::string& name, const std::string& value);
        
        // Get list of available scripts
        std::vector<std::string> getAvailableScripts() const;
        
        // Get script documentation
        std::string getScriptDocumentation(const std::string& name) const;
        
        // Check if script exists
        bool hasScript(const std::string& name) const;
        
        // Get execution count for script
        uint64_t getExecutionCount(const std::string& name) const;
        
        // Reset execution counts
        void resetExecutionCounts();
        
        // Enable/disable scripting
        void enableScripting(bool enable) { enabled_ = enable; }
        bool isScriptingEnabled() const { return enabled_; }
        
        // Enable/disable debug mode
        void setDebugMode(bool debug) { debugMode_ = debug; }
        bool isDebugMode() const { return debugMode_; }
        
        // Get script status
        std::string getStatus() const;
        
    private:
        struct Script {
            std::string name;
            std::string description;
            std::vector<ScriptCommand> commands;
            std::string documentation;
            uint64_t executionCount;
            
            Script(const std::string& n, const std::string& desc, const std::string& doc)
                : name(n), description(desc), documentation(doc), executionCount(0) {}
        };
        
        std::vector<Script> scripts_;
        std::unordered_map<std::string, std::string> variables_;
        bool enabled_;
        bool debugMode_;
        std::vector<std::string> callStack_;
        
        // Execute single command
        bool executeCommand(const ScriptCommand& cmd);
        
        // Parse script line
        ScriptCommand parseCommand(const std::string& line);
        
        // Get variable value as float
        float getFloatVariable(const std::string& name, float defaultValue = 0.0f) const;
        
        // Get variable value as int
        int getIntVariable(const std::string& name, int defaultValue = 0) const;
        
        // Get variable value as bool
        bool getBoolVariable(const std::string& name, bool defaultValue = false) const;
        
        // Check condition
        bool evaluateCondition(const std::string& condition);
    };
    
    // Access to script engine
    ScriptEngine& getScriptEngine() { return scriptEngine_; }
    const ScriptEngine& getScriptEngine() const { return scriptEngine_; }
    
    // Enable/disable scripting
    void enableScripting(bool enable) { scriptEngine_.enableScripting(enable); }
    bool isScriptingEnabled() const { return scriptEngine_.isScriptingEnabled(); }
    
    // Execute script
    bool executeScript(const std::string& scriptName, const std::vector<std::string>& parameters = std::vector<std::string>());
    
    // Add script command
    void addScriptCommand(const ScriptCommand& cmd) { scriptEngine_.addCommand(cmd); }
    
    // Get script variable
    std::string getScriptVariable(const std::string& name) const { return scriptEngine_.getVariable(name); }
    
    // Set script variable
    void setScriptVariable(const std::string& name, const std::string& value) { scriptEngine_.setVariable(name, value); }
    
    // Get available scripts
    std::vector<std::string> getAvailableScripts() const { return scriptEngine_.getAvailableScripts(); }
    
    // Get script documentation
    std::string getScriptDocumentation(const std::string& name) const { return scriptEngine_.getScriptDocumentation(name); }
    
    // Check if script exists
    bool hasScript(const std::string& name) const { return scriptEngine_.hasScript(name); }
    
    // Load scripts from directory
    bool loadScriptsFromDirectory(const std::string& directory);
    
    // Save current script state
    void saveScriptState(const std::string& filename);
    
    // Load script state from file
    bool loadScriptState(const std::string& filename);
    
    // Get script execution status
    std::string getScriptStatus() const { return scriptEngine_.getStatus(); }
    
    // Reset all scripts
    void resetAllScripts() { scriptEngine_.resetExecutionCounts(); }
    
    // Enable debug mode
    void setScriptDebugMode(bool debug) { scriptEngine_.setDebugMode(debug); }
    
private:
    ScriptEngine scriptEngine_;  // Script interpreter for neural behaviors

    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};
