// Agent commands - high-level actions that can be issued to the agent
// This provides a command interface for more advanced users to control the agent
class AgentCommands {
public:
    // Agent command types for advanced control
    enum class CommandType {
        MoveTo,           // Move to specific coordinates
        LookAt,           // Look at specific point
        Wait,            // Wait/hold action
        Repeat,          // Repeat last action
        Sequence,        // Execute sequence of commands
        Conditional,     // Conditional execution based on state
        Loop,            // Loop execution with conditions
        SaveState,       // Save agent state to checkpoint
        LoadState,       // Load agent state from checkpoint
        SetEnergy,       // Set energy level
        SetHealth,       // Set health level
        ModifyPlasticity, // Modify plasticity parameters
        EnableFeature,   // Enable/disable features
        Custom          // Custom command with parameters
    };
    
    // Base command interface
    class Command {
    public:
        virtual ~Command() = default;
        virtual void execute(Agent& agent) = 0;
        virtual std::string toString() const = 0;
        virtual std::unique_ptr<Command> clone() const = 0;
    };
    
    // Concrete command implementations
    class MoveToCommand : public Command {
        float x, y;
        float speed;
        bool waitUntilReached;
    public:
        MoveToCommand(float x, float y, float speed = 0.5f, bool wait = false)
            : x(x), y(y), speed(speed), waitUntilReached(wait) {}
        
        void execute(Agent& agent) override {
            // Calculate direction to target
            float dx = x - agent.getAgentBody().x;
            float dy = y - agent.getAgentBody().y;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            if (dist > 0.01f) {
                float angle = std::atan2(dy, dx);
                agent.getAgentBody().orientation = angle;
                
                // Execute movement
                agent.applyReward(0.0f); // Trigger movement
                
                if (waitUntilReached) {
                    // Wait until close enough
                    while (dist > 0.1f && agent.isAlive()) {
                        agent.step(0.1);
                        float newDist = std::sqrt(
                            (x - agent.getAgentBody().x) * (x - agent.getAgentBody().x) +
                            (y - agent.getAgentBody().y) * (y - agent.getAgentBody().y)
                        );
                        if (newDist != dist) dist = newDist;
                        else break;
                    }
                }
            }
        }
        
        std::string toString() const override {
            return "MoveTo(" + std::to_string(x) + ", " + std::to_string(y) + ")";
        }
        
        std::unique_ptr<Command> clone() const override {
            return std::make_unique<MoveToCommand>(*this);
        }
    };
    
    class LookAtCommand : public Command {
        float targetX, targetY;
        float turnSpeed;
    public:
        LookAtCommand(float tx, float ty, float speed = 0.5f)
            : targetX(tx), targetY(ty), turnSpeed(speed) {}
        
        void execute(Agent& agent) override {
            float dx = targetX - agent.getAgentBody().x;
            float dy = targetY - agent.getAgentBody().y;
            float angle = std::atan2(dy, dx);
            
            float angleDiff = angle - agent.getAgentBody().orientation;
            while (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
            while (angleDiff < -M_PI) angleDiff += 2.0f * M_PI;
            
            angleDiff = std::min(angleDiff, turnSpeed);
            agent.getAgentBody().orientation += angleDiff;
        }
        
        std::string toString() const override {
            return "LookAt(" + std::to_string(targetX) + ", " + std::to_string(targetY) + ")";
        }
        
        std::unique_ptr<Command> clone() const override {
            return std::make_unique<LookAtCommand>(*this);
        }
    };
    
    class SequenceCommand : public Command {
        std::vector<std::unique_ptr<Command>> commands;
    public:
        void addCommand(std::unique_ptr<Command> cmd) {
            commands.push_back(std::move(cmd));
        }
        
        void execute(Agent& agent) override {
            for (auto& cmd : commands) {
                cmd->execute(agent);
                if (!agent.isAlive()) break;
            }
        }
        
        std::string toString() const override {
            std::string result = "Sequence(";
            for (size_t i = 0; i < commands.size(); ++i) {
                if (i > 0) result += ", ";
                result += commands[i]->toString();
            }
            result += ")";
            return result;
        }
        
        std::unique_ptr<Command> clone() const override {
            auto result = std::make_unique<SequenceCommand>();
            for (const auto& cmd : commands) {
                result->addCommand(cmd->clone());
            }
            return result;
        }
    };
    
    class ConditionalCommand : public Command {
        std::function<bool(const Agent&)> condition;
        std::unique_ptr<Command> ifTrue;
        std::unique_ptr<Command> ifFalse;
    public:
        ConditionalCommand(std::function<bool(const Agent&)> cond,
                          std::unique_ptr<Command> trueCmd,
                          std::unique_ptr<Command> falseCmd = nullptr)
            : condition(cond), ifTrue(std::move(trueCmd)), ifFalse(std::move(falseCmd)) {}
        
        void execute(Agent& agent) override {
            if (condition(agent)) {
                if (ifTrue) ifTrue->execute(agent);
            } else {
                if (ifFalse) ifFalse->execute(agent);
            }
        }
        
        std::string toString() const override {
            std::string result = "If(";
            // Condition string would need to be implemented
            result += ", " + (ifTrue ? ifTrue->toString() : "null");
            if (ifFalse) result += ", " + ifFalse->toString();
            result += ")";
            return result;
        }
        
        std::unique_ptr<Command> clone() const override {
            auto result = std::make_unique<ConditionalCommand>(condition,
                                                                 ifTrue ? ifTrue->clone() : nullptr,
                                                                 ifFalse ? ifFalse->clone() : nullptr);
            return result;
        }
    };
    
    class LoopCommand : public Command {
        std::unique_ptr<Command> body;
        std::function<bool(const Agent&)> condition;
        size_t maxIterations;
    public:
        LoopCommand(std::unique_ptr<Command> bodyCmd,
                   std::function<bool(const Agent&)> cond,
                   size_t maxIter = 1000)
            : body(std::move(bodyCmd)), condition(cond), maxIterations(maxIter) {}
        
        void execute(Agent& agent) override {
            size_t iter = 0;
            while (condition(agent) && iter < maxIterations && agent.isAlive()) {
                body->execute(agent);
                iter++;
            }
        }
        
        std::string toString() const override {
            std::string result = "Loop(body=";
            result += body ? body->toString() : "null";
            result += ", maxIter=" + std::to_string(maxIterations) + ")";
            return result;
        }
        
        std::unique_ptr<Command> clone() const override {
            auto result = std::make_unique<LoopCommand>(body ? body->clone() : nullptr,
                                                         condition, maxIterations);
            return result;
        }
    };
    
    class SaveStateCommand : public Command {
        std::string filename;
    public:
        SaveStateCommand(const std::string& fname) : filename(fname) {}
        
        void execute(Agent& agent) override {
            // Save agent state to file
            // Implementation would involve serialization
            NLM_LOG_INFO("Saving agent state to: " + filename);
            // In a real implementation, this would save agentBody_, brain_, etc.
        }
        
        std::string toString() const override {
            return "SaveState(" + filename + ")";
        }
        
        std::unique_ptr<Command> clone() const override {
            return std::make_unique<SaveStateCommand>(filename);
        }
    };
    
    class LoadStateCommand : public Command {
        std::string filename;
    public:
        LoadStateCommand(const std::string& fname) : filename(fname) {}
        
        void execute(Agent& agent) override {
            // Load agent state from file
            NLM_LOG_INFO("Loading agent state from: " + filename);
            // In a real implementation, this would restore agentBody_, brain_, etc.
        }
        
        std::string toString() const override {
            return "LoadState(" + filename + ")";
        }
        
        std::unique_ptr<Command> clone() const override {
            return std::make_unique<LoadStateCommand>(filename);
        }
    };
    
    class EnableFeatureCommand : public Command {
        std::string featureName;
        bool enable;
    public:
        EnableFeatureCommand(const std::string& name, bool enable)
            : featureName(name), enable(enable) {}
        
        void execute(Agent& agent) override {
            if (featureName == "reward_modulation") {
                agent.enableRewardModulation(enable);
            } else if (featureName == "structural_plasticity") {
                agent.enableStructuralPlasticity(enable);
            } else if (featureName == "development") {
                agent.enableDevelopment(enable);
            } else if (featureName == "curiosity") {
                agent.enableCuriosity(enable);
            }
            NLM_LOG_INFO("Feature " + featureName + " set to " + 
                        std::string(enable ? "enabled" : "disabled"));
        }
        
        std::string toString() const override {
            return "EnableFeature(" + featureName + ", " + std::string(enable ? "true" : "false") + ")";
        }
        
        std::unique_ptr<Command> clone() const override {
            return std::make_unique<EnableFeatureCommand>(featureName, enable);
        }
    };
    
    class ModifyPlasticityCommand : public Command {
        float plasticityModifier;
        float synaptogenesisRate;
        float pruningRate;
    public:
        ModifyPlasticityCommand(float plasticMod = 1.0f, 
                               float synRate = 0.0001f,
                               float pruneRate = 0.00001f)
            : plasticityModifier(plasticMod), synaptogenesisRate(synRate), pruningRate(pruneRate) {}
        
        void execute(Agent& agent) override {
            if (agent.getBrain()) {
                // Modify brain plasticity parameters
                NLM_LOG_INFO("Modifying plasticity: modifier=" + std::to_string(plasticityModifier) +
                           ", synRate=" + std::to_string(synaptogenesisRate) +
                           ", pruneRate=" + std::to_string(pruningRate));
                // In a real implementation, this would modify the brain's plasticity system
            }
        }
        
        std::string toString() const override {
            return "ModifyPlasticity(plasticMod=" + std::to_string(plasticityModifier) +
                   ", synRate=" + std::to_string(synaptogenesisRate) +
                   ", pruneRate=" + std::to_string(pruningRate) + ")";
        }
        
        std::unique_ptr<Command> clone() const override {
            return std::make_unique<ModifyPlasticityCommand>(plasticityModifier, synaptogenesisRate, pruningRate);
        }
    };
    
    class CustomCommand : public Command {
        std::string name;
        std::vector<float> parameters;
        std::function<void(Agent&, const std::vector<float>&)> action;
    public:
        CustomCommand(const std::string& name, const std::vector<float>& params)
            : name(name), parameters(params) {}
        
        CustomCommand(const std::string& name,
                     const std::vector<float>& params,
                     std::function<void(Agent&, const std::vector<float>&)> action)
            : name(name), parameters(params), action(action) {}
        
        void execute(Agent& agent) override {
            if (action) {
                action(agent, parameters);
            } else {
                NLM_LOG_INFO("Custom command " + name + " executed with " + std::to_string(parameters.size()) + " parameters");
            }
        }
        
        std::string toString() const override {
            std::string result = "Custom(";
            result += name + ", params=[";
            for (size_t i = 0; i < parameters.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(parameters[i]);
            }
            result += "])";
            return result;
        }
        
        std::unique_ptr<Command> clone() const override {
            auto result = std::make_unique<CustomCommand>(name, parameters, action);
            return result;
        }
    };
    
    // Agent command interpreter/executor
    class CommandInterpreter {
        Agent& agent;
        std::vector<std::unique_ptr<Command>> commandHistory;
        std::unique_ptr<Command> lastCommand;
    public:
        CommandInterpreter(Agent& agent) : agent(agent) {}
        
        // Execute a command
        void executeCommand(const Command& cmd) {
            auto cmdCopy = cmd.clone();
            cmdCopy->execute(agent);
            commandHistory.push_back(std::move(cmdCopy));
            lastCommand = cmd.copy();
        }
        
        // Execute multiple commands sequentially
        void executeSequence(const std::vector<Command*>& commands) {
            for (auto cmd : commands) {
                executeCommand(*cmd);
            }
        }
        
        // Check if agent can execute a command
        bool canExecute(const Command& cmd) const {
            // Check preconditions based on command type
            return true; // Simplified - always true for now
        }
        
        // Get command history
        const std::vector<std::unique_ptr<Command>>& getCommandHistory() const {
            return commandHistory;
        }
        
        // Get last command
        const Command* getLastCommand() const {
            return lastCommand.get();
        }
        
        // Clear command history
        void clearHistory() {
            commandHistory.clear();
            lastCommand = nullptr;
        }
        
        // Save commands to file
        void saveCommands(const std::string& filename) {
            // Implementation would serialize command history
            NLM_LOG_INFO("Saving commands to: " + filename);
        }
        
        // Load commands from file
        void loadCommands(const std::string& filename) {
            // Implementation would deserialize command history
            NLM_LOG_INFO("Loading commands from: " + filename);
        }
        
        // Generate command from string (parsing)
        std::unique_ptr<Command> parseCommand(const std::string& commandStr) {
            // Simple command parsing - in a real implementation, this would be more sophisticated
            if (commandStr.find("MoveTo(") == 0) {
                // Parse MoveTo command
                size_t pos1 = commandStr.find('(', 6);
                size_t pos2 = commandStr.find(',', pos1);
                size_t pos3 = commandStr.find(')', pos2);
                if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                    float x = std::stof(commandStr.substr(pos1 + 1, pos2 - pos1 - 1));
                    float y = std::stof(commandStr.substr(pos2 + 1, pos3 - pos2 - 1));
                    return std::make_unique<MoveToCommand>(x, y);
                }
            } else if (commandStr.find("LookAt(") == 0) {
                // Parse LookAt command
                size_t pos1 = commandStr.find('(', 6);
                size_t pos2 = commandStr.find(',', pos1);
                size_t pos3 = commandStr.find(')', pos2);
                if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                    float x = std::stof(commandStr.substr(pos1 + 1, pos2 - pos1 - 1));
                    float y = std::stof(commandStr.substr(pos2 + 1, pos3 - pos2 - 1));
                    return std::make_unique<LookAtCommand>(x, y);
                }
            }
            // Add more command parsing here
            return nullptr;
        }
    };
    
    // Global command functions for easy scripting
    static std::unique_ptr<Command> makeMoveTo(float x, float y, float speed = 0.5f) {
        return std::make_unique<MoveToCommand>(x, y, speed);
    }
    
    static std::unique_ptr<Command> makeLookAt(float x, float y, float speed = 0.5f) {
        return std::make_unique<LookAtCommand>(x, y, speed);
    }
    
    static std::unique_ptr<Command> makeSequence() {
        return std::make_unique<SequenceCommand>();
    }
    
    static std::unique_ptr<Command> makeConditional(std::function<bool(const Agent&)> cond,
                                                     std::unique_ptr<Command> trueCmd,
                                                     std::unique_ptr<Command> falseCmd = nullptr) {
        return std::make_unique<ConditionalCommand>(cond, std::move(trueCmd), std::move(falseCmd));
    }
    
    static std::unique_ptr<Command> makeLoop(std::unique_ptr<Command> body,
                                            std::function<bool(const Agent&)> cond,
                                            size_t maxIter = 1000) {
        return std::make_unique<LoopCommand>(std::move(body), cond, maxIter);
    }
    
    static std::unique_ptr<Command> makeSaveState(const std::string& filename) {
        return std::make_unique<SaveStateCommand>(filename);
    }
    
    static std::unique_ptr<Command> makeLoadState(const std::string& filename) {
        return std::make_unique<LoadStateCommand>(filename);
    }
    
    static std::unique_ptr<Command> makeEnableFeature(const std::string& name, bool enable) {
        return std::make_unique<EnableFeatureCommand>(name, enable);
    }
    
    static std::unique_ptr<Command> makeModifyPlasticity(float plasticMod = 1.0f,
                                                         float synRate = 0.0001f,
                                                         float pruneRate = 0.00001f) {
        return std::make_unique<ModifyPlasticityCommand>(plasticMod, synRate, pruneRate);
    }
    
    static std::unique_ptr<Command> makeCustom(const std::string& name,
                                               const std::vector<float>& params = {}) {
        return std::make_unique<CustomCommand>(name, params);
    }
    
    static std::unique_ptr<Command> makeCustom(const std::string& name,
                                               const std::vector<float>& params,
                                               std::function<void(Agent&, const std::vector<float>&)> action) {
        return std::make_unique<CustomCommand>(name, params, action);
    }
};

} // namespace nlm