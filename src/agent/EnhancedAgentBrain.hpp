#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>

namespace nlm {

// Forward declarations
class AgentBrain;

// Motor command to string mapping
std::string motorCommandToString(MotorCommand cmd);

// Advanced Command structure for enhanced agent capabilities
struct AdvancedCommand {
    MotorCommand baseCommand;
    float confidence;           // How confident we are in this command
    float urgency;              // How urgent this command is (0-1)
    float explorationFactor;    // How much exploration vs exploitation
    std::vector<std::string> tags;  // Command metadata
    float cost;                 // Resource cost of executing this command
    float rewardValue;          // Expected reward value
    bool requiresContext;       // Whether this command needs context
    std::unordered_map<std::string, float> parameters; // Dynamic parameters
    
    AdvancedCommand() : baseCommand(MotorCommand::Wait), confidence(0.0f), 
                       urgency(0.0f), explorationFactor(0.0f), cost(0.0f), 
                       rewardValue(0.0f), requiresContext(false) {}
};

// Advanced Agent State for enhanced decision making
struct AdvancedAgentState {
    // Physical state
    float energy;                    // Current energy level (0-1)
    float health;                    // Current health (0-1)
    float hunger;                    // Hunger level (0-1)
    float fatigue;                   // Fatigue level (0-1)
    
    // Cognitive state
    float curiosity;                 // Current curiosity level (0-1)
    float boredom;                   // Current boredom level (0-1)
    float stress;                    // Current stress level (0-1)
    float focus;                     // Current focus level (0-1)
    
    // Memory state
    std::vector<std::string> recentActions;     // Last N actions taken
    std::vector<std::string> recentObservations; // Last N observations
    std::unordered_map<std::string, float> learnedValues; // Q-learning style values
    
    // Developmental state
    float developmentalAge;           // Age in simulation steps
    float plasticity;                 // Current plasticity level
    std::string behavioralStyle;      // "explorative", "exploitative", "balanced"
    
    // Social state (if multi-agent)
    std::vector<std::string> knownAgents;      // Other agents we've encountered
    std::unordered_map<std::string, float> relationshipScores; // Trust/punishment values
    
    AdvancedAgentState() {
        reset();
    }
    
    void reset() {
        energy = 1.0f;
        health = 1.0f;
        hunger = 0.0f;
        fatigue = 0.0f;
        curiosity = 0.0f;
        boredom = 0.0f;
        stress = 0.0f;
        focus = 1.0f;
        developmentalAge = 0.0f;
        plasticity = 1.0f;
        behavioralStyle = "balanced";
        
        recentActions.clear();
        recentObservations.clear();
        learnedValues.clear();
        knownAgents.clear();
        relationshipScores.clear();
    }
    
    void updateEnergy(float delta) {
        energy = std::clamp(energy + delta, 0.0f, 1.0f);
        if (energy < 0.2f) stress = std::min(stress + 0.1f, 1.0f);
    }
    
    void updateHealth(float delta) {
        health = std::clamp(health + delta, 0.0f, 1.0f);
        if (health < 0.3f) stress = std::min(stress + 0.2f, 1.0f);
    }
    
    void addRecentAction(const std::string& action, size_t maxHistory = 10) {
        recentActions.push_back(action);
        if (recentActions.size() > maxHistory) {
            recentActions.erase(recentActions.begin());
        }
    }
    
    void addRecentObservation(const std::string& observation, size_t maxHistory = 10) {
        recentObservations.push_back(observation);
        if (recentObservations.size() > maxHistory) {
            recentObservations.erase(recentObservations.begin());
        }
    }
    
    float getActionValue(const std::string& action) const {
        auto it = learnedValues.find(action);
        return it != learnedValues.end() ? it->second : 0.0f;
    }
    
    void updateActionValue(const std::string& action, float value, float learningRate = 0.1f) {
        learnedValues[action] = learnedValues[action] * (1.0f - learningRate) + value * learningRate;
    }
    
    std::string getBehavioralRecommendation() const {
        if (behavioralStyle == "explorative" && curiosity > 0.7f) {
            return "explore_area";
        } else if (behavioralStyle == "exploitative" && stress < 0.3f) {
            return "return_to_safe_zone";
        } else {
            return "continue_current_behavior";
        }
    }
};

// Decision context for adaptive command selection
struct DecisionContext {
    std::string currentSituation;
    float urgency;
    float energyLevel;
    float stressLevel;
    std::vector<std::string> availableCommands;
    std::unordered_map<std::string, float> environmentalFactors;
};

// Command execution record for learning
struct CommandExecution {
    std::string commandName;
    float executedConfidence;
    float actualOutcome;
    float expectedOutcome;
    std::chrono::system_clock::time_point timestamp;
};

// Enhanced Agent Capabilities class
class AdvancedAgentCapabilities {
public:
    AdvancedAgentCapabilities() {
        initializeDefaultCapabilities();
    }
    
    void initializeDefaultCapabilities() {
        // Define agent capabilities
        capabilities_ = {
            {"navigation", 1.0f},
            {"exploration", 0.8f},
            {"avoidance", 0.9f},
            {"interaction", 0.7f},
            {"memory", 0.6f},
            {"reasoning", 0.5f},
            {"learning", 0.8f},
            {"social", 0.3f}
        };
        
        // Define command sets
        commandSets_ = {
            {"basic", {"move_forward", "move_backward", "turn_left", "turn_right", "interact", "wait"}},
            {"navigation", {"go_to", "patrol", "follow_path", "avoid_obstacle"}},
            {"exploration", {"explore_area", "search_for_resources", "discover_new_locations"}},
            {"interaction", {"approach_object", "manipulate_object", "use_tool"}},
            {"strategic", {"plan_action", "evaluate_sequence", "adapt_behavior"}}
        };
    }
    
    float getCapabilityLevel(const std::string& capability) const {
        auto it = capabilities_.find(capability);
        return it != capabilities_.end() ? it->second : 0.0f;
    }
    
    bool hasCapability(const std::string& capability) const {
        return capabilities_.find(capability) != capabilities_.end() && 
               capabilities_.at(capability) > 0.1f;
    }
    
    void setCapability(const std::string& capability, float level) {
        capabilities_[capability] = std::clamp(level, 0.0f, 1.0f);
    }
    
    const std::vector<std::string>& getCommandsForSet(const std::string& commandSet) const {
        auto it = commandSets_.find(commandSet);
        static const std::vector<std::string> empty = {};
        return it != commandSets_.end() ? it->second : empty;
    }
    
    void addCommandToSet(const std::string& commandSet, const std::string& command) {
        commandSets_[commandSet].push_back(command);
    }
    
    void removeCommandFromSet(const std::string& commandSet, const std::string& command) {
        auto it = commandSets_.find(commandSet);
        if (it != commandSets_.end()) {
            auto& commands = it->second;
            commands.erase(std::remove(commands.begin(), commands.end(), command), commands.end());
        }
    }
    
    std::vector<std::string> getAllAvailableCommands() const {
        std::vector<std::string> allCommands;
        for (const auto& set : commandSets_) {
            allCommands.insert(allCommands.end(), set.second.begin(), set.second.end());
        }
        // Remove duplicates
        std::sort(allCommands.begin(), allCommands.end());
        allCommands.erase(std::unique(allCommands.begin(), allCommands.end()), allCommands.end());
        return allCommands;
    }
    
    // Learn from experience - improve capabilities based on performance
    void learnFromExperience(const std::string& capability, float performance, float learningRate = 0.1f) {
        if (capabilities_.find(capability) != capabilities_.end()) {
            float current = capabilities_[capability];
            // Learning formula: new = current + rate * (performance - current)
            capabilities_[capability] = current + learningRate * (performance - current);
            capabilities_[capability] = std::clamp(capabilities_[capability], 0.0f, 1.0f);
        }
    }
    
    // Get recommended command based on current state and capabilities
    std::string getRecommendedCommand(const std::string& situation, float urgency = 0.5f) const {
        // Simple recommendation logic - in production would use ML
        if (situation == "low_energy") {
            return "wait";
        } else if (situation == "blocked") {
            return "avoid_obstacle";
        } else if (getCapabilityLevel("exploration") > 0.7f) {
            return "explore_area";
        }
        
        // Default to basic movement
        return "move_forward";
    }
    
private:
    std::unordered_map<std::string, float> capabilities_;
    std::unordered_map<std::string, std::vector<std::string>> commandSets_;
};

// Enhanced Agent Brain with advanced capabilities and adaptive decision making
class EnhancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> baseBrain_;
    AdvancedAgentState state_;
    AdvancedAgentCapabilities capabilities_;
    std::unordered_map<std::string, AdvancedCommand> advancedCommands_;
    std::mt19937 randomEngine_;
    
    // Command execution history for learning
    std::vector<CommandExecution> commandHistory_;
    
    // Adaptive decision making constraints
    std::unordered_map<std::string, std::vector<std::pair<std::string, float>>> commandDependencies_;
    std::unordered_map<std::string, std::vector<std::pair<std::string, float>>> commandConstraints_;
    
public:
    EnhancedAgentBrain(std::shared_ptr<AgentBrain> baseBrain)
        : baseBrain_(baseBrain), randomEngine_(std::random_device{}()) {
        initializeAdvancedCommands();
        setupDefaultCommandConfigurations();
    }
    
    void initializeAdvancedCommands() {
        // Define advanced commands with their parameters
        advancedCommands_["explore_area"] = {
            .baseCommand = MotorCommand::MoveForward,
            .confidence = 0.7f,
            .urgency = 0.6f,
            .explorationFactor = 0.8f,
            .tags = {"exploration", "navigation", "learning"},
            .cost = 0.3f,
            .rewardValue = 1.0f,
            .requiresContext = true,
            .parameters = {{ "radius", 5.0f }, { "thoroughness", 0.7f }}
        };
        
        advancedCommands_["go_to"] = {
            .baseCommand = MotorCommand::MoveForward,
            .confidence = 0.9f,
            .urgency = 0.8f,
            .explorationFactor = 0.2f,
            .tags = {"navigation", "goal-oriented", "precision"},
            .cost = 0.4f,
            .rewardValue = 1.5f,
            .requiresContext = true,
            .parameters = {{ "target_x", 0.0f }, { "target_y", 0.0f }}
        };
        
        advancedCommands_["avoid_obstacle"] = {
            .baseCommand = MotorCommand::TurnLeft,
            .confidence = 0.8f,
            .urgency = 0.9f,
            .explorationFactor = 0.1f,
            .tags = {"safety", "emergency", "reactive"},
            .cost = 0.1f,
            .rewardValue = 0.5f,
            .requiresContext = true,
            .parameters = {{ "avoidance_radius", 2.0f }, { "turn_strength", 0.7f }}
        };
        
        advancedCommands_["search_for_resources"] = {
            .baseCommand = MotorCommand::MoveForward,
            .confidence = 0.6f,
            .urgency = 0.4f,
            .explorationFactor = 0.9f,
            .tags = {"foraging", "exploration", "resource-seeking"},
            .cost = 0.5f,
            .rewardValue = 1.2f,
            .requiresContext = true,
            .parameters = {{ "search_pattern", "grid" }, { "detection_range", 3.0f }}
        };
        
        advancedCommands_["plan_action"] = {
            .baseCommand = MotorCommand::Wait,
            .confidence = 0.5f,
            .urgency = 0.3f,
            .explorationFactor = 0.7f,
            .tags = {"cognition", "strategic", "thinking"},
            .cost = 0.2f,
            .rewardValue = 0.8f,
            .requiresContext = true,
            .parameters = {{ "plan_horizon", 5 }, { "complexity", "medium" }}
        };
    }
    
    void setupDefaultCommandConfigurations() {
        // Set up command dependencies and relationships
        commandDependencies_["explore_area"] = {{"move_forward", 0.0f}, {"sense_environment", 0.0f}};
        commandDependencies_["go_to"] = {{"move_forward", 0.0f}, {"sense_target", 0.0f}};
        commandDependencies_["avoid_obstacle"] = {{"sense_imminent_danger", 0.0f}};
        commandDependencies_["search_for_resources"] = {{"scan_area", 0.0f}, {"detect_resources", 0.0f}};
        commandDependencies_["plan_action"] = {{"assess_situation", 0.0f}, {"evaluate_options", 0.0f}};
        
        // Set up command execution constraints
        commandConstraints_["explore_area"] = {{ "min_energy", 0.3f }};
        commandConstraints_["go_to"] = {{ "target_acquired", 1.0f }};
        commandConstraints_["avoid_obstacle"] = {{ "immediate_threat", 1.0f }};
    }
    
    // Adaptive decision making engine
    std::string makeAdaptiveDecision(const DecisionContext& context) {
        // Phase 1: Filter available commands based on state and capabilities
        std::vector<std::string> viableCommands;
        for (const auto& pair : advancedCommands_) {
            const auto& cmd = pair.second;
            const auto& cmdName = pair.first;
            
            if (!meetsCommandConstraints(cmdName, context)) continue;
            if (!hasRequiredCapabilities(cmdName, context)) continue;
            if (!contextualRelevance(cmdName, context)) continue;
            
            viableCommands.push_back(cmdName);
        }
        
        // Phase 2: Score commands based on multiple factors
        std::vector<std::pair<std::string, float>> scoredCommands;
        for (const auto& cmdName : viableCommands) {
            const auto& cmd = advancedCommands_.at(cmdName);
            float score = calculateCommandScore(cmd, context);
            scoredCommands.emplace_back(cmdName, score);
        }
        
        // Phase 3: Apply exploration-exploitation tradeoff
        std::vector<std::pair<std::string, float>> finalScores;
        for (auto& pair : scoredCommands) {
            const auto& cmdName = pair.first;
            float score = pair.second;
            
            // Add exploration component
            float explorationBonus = 0.0f;
            if (state_.curiosity > 0.5f && randomEngine_() % 100 < 20) {
                explorationBonus = 0.2f * state_.explorationFactor;
            }
            
            // Add constraint penalties
            float constraintPenalty = getCommandConstraintPenalties(cmdName, context);
            
            float finalScore = score + explorationBonus - constraintPenalty;
            finalScores.emplace_back(cmdName, finalScore);
        }
        
        // Phase 4: Select best command
        if (finalScores.empty()) {
            return "wait"; // Fallback command
        }
        
        // Sort by score (highest first)
        std::sort(finalScores.begin(), finalScores.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        return finalScores[0].first;
    }
    
    // Command execution with learning
    MotorCommand executeAdvancedCommand(const std::string& commandName, 
                                       const std::unordered_map<std::string, float>& parameters) {
        auto it = advancedCommands_.find(commandName);
        if (it == advancedCommands_.end()) {
            NLM_LOG_WARNING("Unknown advanced command: " + commandName);
            return MotorCommand::Wait;
        }
        
        const auto& cmd = it->second;
        
        // Check if command can be executed
        if (!canExecuteCommand(cmd, parameters)) {
            NLM_LOG_WARNING("Cannot execute command " + commandName + " - constraints not met");
            return MotorCommand::Wait;
        }
        
        // Execute command
        MotorCommand result = executeCommandBase(cmd.baseCommand, parameters);
        
        // Update state based on command
        updateStateAfterCommand(cmd, result, parameters);
        
        // Learn from execution
        recordCommandExecution(commandName, cmd, result, parameters);
        
        // Store command in agent's action history
        if (baseBrain_) {
            // Could update agent's state through base brain
        }
        
        NLM_LOG_INFO("Executed advanced command: " + commandName + " -> " + 
                    motorCommandToString(result));
        
        return result;
    }
    
    // Get enhanced motor command with advanced features
    MotorCommand getEnhancedMotorCommand() {
        // Build decision context
        DecisionContext context;
        context.currentSituation = assessCurrentSituation();
        context.urgency = calculateUrgency();
        context.energyLevel = state_.energy;
        context.stressLevel = state_.stress;
        context.availableCommands = getAllAvailableAdvancedCommands();
        context.environmentalFactors = getEnvironmentalFactors();
        
        // Make adaptive decision
        std::string selectedCommand = makeAdaptiveDecision(context);
        
        // Get command parameters
        std::unordered_map<std::string, float> parameters = getCommandParameters(selectedCommand);
        
        // Execute advanced command
        return executeAdvancedCommand(selectedCommand, parameters);
    }
    
    // Get all available advanced commands
    std::vector<std::string> getAllAvailableAdvancedCommands() const {
        std::vector<std::string> commands;
        for (const auto& pair : advancedCommands_) {
            commands.push_back(pair.first);
        }
        return commands;
    }
    
    // Get command capabilities and metadata
    std::unordered_map<std::string, std::unordered_map<std::string, float>> getCommandMetadata() const {
        std::unordered_map<std::string, std::unordered_map<std::string, float>> metadata;
        for (const auto& pair : advancedCommands_) {
            const auto& cmd = pair.second;
            std::unordered_map<std::string, float> cmdMeta;
            cmdMeta["confidence"] = cmd.confidence;
            cmdMeta["urgency"] = cmd.urgency;
            cmdMeta["exploration_factor"] = cmd.explorationFactor;
            cmdMeta["cost"] = cmd.cost;
            cmdMeta["reward_value"] = cmd.rewardValue;
            
            // Convert tags to numeric values (simplified)
            float tagScore = 0.0f;
            for (const auto& tag : cmd.tags) {
                if (tag == "exploration") tagScore += 0.3f;
                else if (tag == "navigation") tagScore += 0.2f;
                else if (tag == "safety") tagScore += 0.4f;
                else if (tag == "emergency") tagScore += 0.5f;
                else if (tag == "learning") tagScore += 0.1f;
                else if (tag == "goal-oriented") tagScore += 0.3f;
                else if (tag == "precision") tagScore += 0.2f;
                else if (tag == "foraging") tagScore += 0.4f;
                else if (tag == "resource-seeking") tagScore += 0.3f;
                else if (tag == "cognition") tagScore += 0.2f;
                else if (tag == "strategic") tagScore += 0.3f;
                else if (tag == "thinking") tagScore += 0.2f;
            }
            cmdMeta["tag_score"] = tagScore;
            
            metadata[pair.first] = cmdMeta;
        }
        return metadata;
    }
    
    // Get agent's current capabilities
    std::unordered_map<std::string, float> getCurrentCapabilities() const {
        return capabilities_.capabilities_;
    }
    
    // Update agent's behavioral style based on experience
    void updateBehavioralStyle(float exploration, float exploitation, float learningRate = 0.1f) {
        capabilities_.learnFromExperience("exploration", exploration, learningRate);
        
        // Update behavioral style based on capabilities
        if (exploration > 0.7f && exploitation > 0.7f) {
            state_.behavioralStyle = "balanced";
        } else if (exploration > exploitation) {
            state_.behavioralStyle = "explorative";
        } else {
            state_.behavioralStyle = "exploitative";
        }
    }
    
    // Get agent's learned values for reinforcement learning
    std::unordered_map<std::string, float> getLearnedValues() const {
        return state_.learnedValues;
    }
    
    // Apply reinforcement learning update
    void applyReinforcementLearning(const std::string& action, float reward, float learningRate = 0.1f) {
        float currentValue = state_.getActionValue(action);
        state_.updateActionValue(action, reward, learningRate);
        
        // Update capabilities based on learning
        if (reward > 0.5f) {
            capabilities_.learnFromExperience("learning", 0.3f, learningRate);
        }
        
        NLM_LOG_INFO("RL update: action " + action + " received reward " + std::to_string(reward));
    }
    
    // Get command history for analysis
    std::vector<std::string> getCommandHistory(size_t maxHistory = 10) const {
        std::vector<std::string> history;
        size_t start = std::max(size_t(0), commandHistory_.size() - maxHistory);
        
        for (size_t i = start; i < commandHistory_.size(); ++i) {
            history.push_back(commandHistory_[i].commandName);
        }
        
        return history;
    }
    
    // Get advanced agent state
    AdvancedAgentState getAgentState() const {
        return state_;
    }
    
    // Update agent state
    void updateAgentState(const AdvancedAgentState& newState) {
        state_ = newState;
    }
    
    // Get environmental factors
    std::unordered_map<std::string, float> getEnvironmentalFactors() const {
        std_t::unordered_map<std::string, float> factors;
        
        // Simulate environmental factors
        factors["temperature"] = 25.0f + (randomEngine_() % 20 - 10); // 15-35°C
        factors["humidity"] = 40.0f + (randomEngine_() % 40); // 40-80%
        factors["light_level"] = randomEngine_() % 100; // 0-100%
        factors["noise_level"] = randomEngine_() % 50; // 0-50 dB
        
        return factors;
    }
    
    // Assess current situation
    std::string assessCurrentSituation() const {
        if (state_.energy < 0.2f) {
            return "low_energy";
        } else if (state_.health < 0.3f) {
            return "injured";
        } else if (state_.stress > 0.7f) {
            return "high_stress";
        } else if (state_.curiosity > 0.8f) {
            return "curious";
        } else if (state_.boredom > 0.7f) {
            return "bored";
        } else {
            return "normal";
        }
    }
    
    // Calculate command urgency
    float calculateUrgency() const {
        float urgency = 0.0f;
        
        if (state_.stress > 0.7f) urgency += 0.4f;
        if (state_.energy < 0.3f) urgency += 0.3f;
        if (state_.health < 0.5f) urgency += 0.3f;
        
        return std::min(urgency, 1.0f);
    }
    
    // Get command parameters
    std::unordered_map<std::string, float> getCommandParameters(const std::string& commandName) const {
        auto it = advancedCommands_.find(commandName);
        if (it != advancedCommands_.end()) {
            return it->second.parameters;
        }
        return {};
    }
    
    // Helper functions
    bool meetsCommandConstraints(const std::string& commandName, const DecisionContext& context) const {
        auto it = commandConstraints_.find(commandName);
        if (it == commandConstraints_.end()) return true;
        
        for (const auto& constraint : it->second) {
            if (constraint == "min_energy" && context.energyLevel < constraint.second) {
                return false;
            } else if (constraint == "target_acquired" && !constraint.second) {
                return false;
            } else if (constraint == "immediate_threat" && !constraint.second) {
                return false;
            }
        }
        
        return true;
    }
    
    bool hasRequiredCapabilities(const std::string& commandName, const DecisionContext& context) const {
        // Check if agent has required capabilities for this command
        const auto& cmd = advancedCommands_.at(commandName);
        
        for (const auto& tag : cmd.tags) {
            if (!capabilities_.hasCapability(tag)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool contextualRelevance(const std::string& commandName, const DecisionContext& context) const {
        const auto& cmd = advancedCommands_.at(commandName);
        
        // Check if command is relevant to current situation
        if (context.currentSituation == "low_energy" && commandName != "wait") {
            return false;
        }
        
        // Add more context-specific relevance checks
        return true;
    }
    
    float calculateCommandScore(const AdvancedCommand& cmd, const DecisionContext& context) const {
        float score = 0.0f;
        
        // Base confidence
        score += cmd.confidence * 0.4f;
        
        // Urgency alignment
        if (cmd.urgency > 0.5f) {
            score += (context.urgency - 0.5f) * 0.2f;
        }
        
        // Capability alignment
        float capabilityScore = 0.0f;
        for (const auto& tag : cmd.tags) {
            capabilityScore += capabilities_.getCapabilityLevel(tag) * 0.1f;
        }
        score += capabilityScore * 0.3f;
        
        // Cost consideration
        score -= cmd.cost * 0.2f;
        
        // Reward expectation
        score += cmd.rewardValue * 0.1f;
        
        return score;
    }
    
    float getCommandConstraintPenalties(const std::string& commandName, const DecisionContext& context) const {
        float penalty = 0.0f;
        
        auto it = commandConstraints_.find(commandName);
        if (it != commandConstraints_.end()) {
            for (const auto& constraint : it->second) {
                if (constraint == "min_energy" && context.energyLevel < constraint.second) {
                    penalty += (constraint.second - context.energyLevel) * 2.0f;
                } else if (constraint == "target_acquired" && !constraint.second) {
                    penalty += 1.0f;
                } else if (constraint == "immediate_threat" && !constraint.second) {
                    penalty += 0.5f;
                }
            }
        }
        
        return penalty;
    }
    
    bool canExecuteCommand(const AdvancedCommand& cmd, const std::unordered_map<std::string, float>& parameters) const {
        for (const auto& constraint : commandConstraints_) {
            if (constraint.first == "min_energy" && cmd.baseCommand == MotorCommand::MoveForward) {
                return state_.energy >= constraint.second;
            }
        }
        return true;
    }
    
    MotorCommand executeCommandBase(MotorCommand baseCommand, const std::unordered_map<std::string, float>& parameters) {
        // In a real implementation, this would map the base command to the appropriate motor control
        // For now, just return the base command
        return baseCommand;
    }
    
    void updateStateAfterCommand(const AdvancedCommand& cmd, MotorCommand result, 
                                const std::unordered_map<std::string, float>& parameters) {
        // Update agent state based on command execution
        if (cmd.cost > 0.0f) {
            state_.updateEnergy(-cmd.cost * 0.1f);
        }
        
        if (result == MotorCommand::MoveForward || result == MotorCommand::MoveBackward) {
            state_.fatigue += 0.1f;
        }
        
        // Update curiosity based on exploration
        if (cmd.explorationFactor > 0.5f) {
            state_.curiosity = std::min(state_.curiosity + 0.1f, 1.0f);
        }
        
        state_.developmentalAge += 1.0;
    }
    
    void recordCommandExecution(const std::string& commandName, const AdvancedCommand& cmd,
                               MotorCommand result, const std::unordered_map<std::string, float>& parameters) {
        CommandExecution exec;
        exec.commandName = commandName;
        exec.executedConfidence = cmd.confidence;
        exec.actualOutcome = cmd.rewardValue;
        exec.expectedOutcome = cmd.rewardValue;
        exec.timestamp = std::chrono::system_clock::now();
        
        commandHistory_.push_back(exec);
        
        // Keep history manageable
        if (commandHistory_.size() > 1000) {
            commandHistory_.erase(commandHistory_.begin());
        }
        
        // Learn from execution
        float learningRate = 0.05f;
        state_.updateActionValue(commandName, exec.actualOutcome, learningRate);
    }
    
    std::string motorCommandToString(MotorCommand cmd) const {
        switch (cmd) {
            case MotorCommand::MoveForward: return "move_forward";
            case MotorCommand::MoveBackward: return "move_backward";
            case MotorCommand::TurnLeft: return "turn_left";
            case MotorCommand::TurnRight: return "turn_right";
            case MotorCommand::LookLeft: return "look_left";
            case MotorCommand::LookRight: return "look_right";
            case MotorCommand::Interact: return "interact";
            case MotorCommand::Wait: return "wait";
            default: return "unknown";
        }
    }
    
private:
    std::unordered_map<std::string, std::vector<std::pair<std::string, float>>> commandDependencies_;
    std::unordered_map<std::string, std::vector<std::pair<std::string, float>>> commandConstraints_;
};

}} // namespace nlm

// Motor command to string helper function
inline std::string nlm::motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "move_forward";
        case MotorCommand::MoveBackward: return "move_backward";
        case MotorCommand::TurnLeft: return "turn_left";
        case MotorCommand::TurnRight: return "turn_right";
        case MotorCommand::LookLeft: return "look_left";
        case MotorCommand::LookRight: return "look_right";
        case MotorCommand::Interact: return "interact";
        case MotorCommand::Wait: return "wait";
        default: return "unknown";
    }
}