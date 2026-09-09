#include "AgentBrainAdvanced.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <unordered_map>

namespace nlm {

AgentBrainAdvanced::AdvancedDecisionMaker::AdvancedDecisionMaker()
    : learningRate_(0.1f), discountFactor_(0.95f), explorationRate_(0.1f) {}

AdvancedCommand AgentBrainAdvanced::AdvancedDecisionMaker::selectAction(
    const std::vector<float>& state, 
    const std::vector<AdvancedCommand>& availableCommands,
    float explorationRate) {
    
    // Check if state is in state space
    auto it = std::find(stateSpace_.begin(), stateSpace_.end(), state);
    size_t stateIndex = 0;
    bool stateExists = (it != stateSpace_.end());
    if (stateExists) {
        stateIndex = std::distance(stateSpace_.begin(), it);
    }
    
    // Initialize state if new
    if (!stateExists) {
        stateSpace_.push_back(state);
        QValues_.emplace_back(availableCommands.size(), 0.0f);
        stateIndex = stateSpace_.size() - 1;
    }
    
    // Exploration vs exploitation
    float randVal = (std::rand() % 1000) / 1000.0f;
    if (randVal < explorationRate) {
        // Random exploration
        return availableCommands[(std::rand() % availableCommands.size())];
    }
    
    // Find best action
    size_t bestActionIndex = 0;
    float bestValue = QValues_[stateIndex][0];
    
    for (size_t i = 1; i < availableCommands.size(); ++i) {
        if (QValues_[stateIndex][i] > bestValue) {
            bestValue = QValues_[stateIndex][i];
            bestActionIndex = i;
        }
    }
    
    return availableCommands[bestActionIndex];
}

void AgentBrainAdvanced::AdvancedDecisionMaker::updatePolicy(
    const std::vector<float>& state, 
    AdvancedCommand action, 
    float reward) {
    
    // Find state index
    auto it = std::find(stateSpace_.begin(), stateSpace_.end(), state);
    if (it == stateSpace_.end()) return;
    
    size_t stateIndex = std::distance(stateSpace_.begin(), it);
    
    // Find action index
    auto actionIt = std::find(actionSpace_.begin(), actionSpace_.end(), action);
    if (actionIt == actionSpace_.end()) {
        actionSpace_.push_back(action);
        for (auto& row : QValues_) {
            row.push_back(0.0f);
        }
        actionIt = actionSpace_.end() - 1;
    }
    
    size_t actionIndex = std::distance(actionSpace_.begin(), actionIt);
    
    // Q-learning update
    float oldValue = QValues_[stateIndex][actionIndex];
    float maxNextValue = 0.0f;
    
    // Find maximum Q-value for next state (simplified - assuming same state)
    if (!QValues_[stateIndex].empty()) {
        maxNextValue = *std::max_element(QValues_[stateIndex].begin(), QValues_[stateIndex].end());
    }
    
    QValues_[stateIndex][actionIndex] = oldValue + 
        learningRate_ * (reward + discountFactor_ * maxNextValue - oldValue);
}

AgentBrainAdvanced::AgentBrainAdvanced(std::shared_ptr<Brain> brain)
    : AgentBrain(brain)
    , demonstrationLearningEnabled_(false)
    , pathfindingEnabled_(false)
    , socialLearningEnabled_(false)
    , explorationBias_(0.5f)
    , riskAversion_(0.5f)
    , socialInfluence_(0.5f)
    , decisionConfidenceThreshold_(0.7f)
    , actionSelectionMethod_(1)  // epsilon-greedy
    , totalAdvancedActions_(0)
    , averageActionValue_(0.0f)
    , lastLearningUpdate_(0.0) {}

AgentBrainAdvanced::~AgentBrainAdvanced() = default;

void AgentBrainAdvanced::initialize(const SimpleWorld& world, const AgentBody& body) {
    AgentBrain::initialize(world);
    
    // Initialize advanced state from body
    advancedState_.x = body.x;
    advancedState_.y = body.y;
    advancedState_.orientation = body.orientation;
    advancedState_.energy = body.energy;
    
    // Set up advanced systems
    currentPath_.clear();
    waypoints_.clear();
    targetPosition_ = NavigationNode(0.0f, 0.0f);
    
    recentExperiences_.clear();
    demonstrationExperiences_.clear();
    
    NLM_LOG_INFO("Advanced AgentBrain initialized with enhanced capabilities");
}

void AgentBrainAdvanced::update(double timestep) {
    // Update base agent
    AgentBrain::updateNeuromodulation(timestep);
    
    // Update advanced systems
    updateAdvancedDecisionMaking(timestep);
    updateNavigation(timestep);
    updateLearningSystems(timestep);
    updateSocialSystems(timestep);
    updateEmotionalState(timestep);
    
    // Update advanced state
    advancedState_.lastActionTime += timestep;
    
    // Decay curiosity
    advancedState_.curiosity *= 0.95f;
    
    // Update statistics
    totalAdvancedActions_++;
    lastLearningUpdate_ = timestep;
}

AdvancedCommand AgentBrainAdvanced::selectAdvancedAction() {
    std::vector<float> state = extractFeatures(SensoryPercept(), advancedState_);
    std::vector<AdvancedCommand> availableCommands;
    
    // Add all commands to available list
    availableCommands.push_back(AdvancedCommand::NoOp);
    availableCommands.push_back(AdvancedCommand::MoveToTarget);
    availableCommands.push_back(AdvancedCommand::ExploreArea);
    availableCommands.push_back(AdvancedCommand::AvoidDanger);
    availableCommands.push_back(AdvancedCommand::SeekResource);
    
    // Calculate exploration rate based on curiosity and state
    float explorationRate = explorationBias_ * advancedState_.curiosity;
    explorationRate = std::clamp(explorationRate, 0.0f, 1.0f);
    
    // Select action based on method
    AdvancedCommand selectedAction;
    
    switch (actionSelectionMethod_) {
        case 0: // Greedy
            selectedAction = decisionMaker_.selectAction(state, availableCommands, 0.0f);
            break;
        case 1: // Epsilon-greedy
            selectedAction = decisionMaker_.selectAction(state, availableCommands, explorationRate);
            break;
        case 2: // Softmax
            selectedAction = decisionMaker_.selectAction(state, availableCommands, 1.0f - decisionConfidenceThreshold_);
            break;
        default:
            selectedAction = decisionMaker_.selectAction(state, availableCommands, explorationRate);
    }
    
    // Apply risk aversion
    if (riskAversion_ > 0.5f) {
        // Bias towards safer actions
        if (selectedAction == AdvancedCommand::AvoidDanger || 
            selectedAction == AdvancedCommand::NoOp) {
            // Already safe, keep
        } else {
            selectedAction = AdvancedCommand::NoOp; // Default to safe action
        }
    }
    
    return selectedAction;
}

void AgentBrainAdvanced::learnFromExperience(const std::vector<float>& state, 
                                            AdvancedCommand action, 
                                            float reward) {
    // Update Q-learning
    decisionMaker_.updatePolicy(state, action, reward);
    
    // Store experience
    std::stringstream ss;
    ss << "exp_" << recentExperiences_.size();
    
    LearningRecord record(state, action, reward, ss.str());
    recentExperiences_.push_back(record);
    
    // Update average action value
    static float actionValueSum = 0.0f;
    static size_t actionCount = 0;
    actionValueSum += reward;
    actionCount++;
    
    if (actionCount > 0) {
        averageActionValue_ = actionValueSum / actionCount;
    }
    
    // Demonstration learning if enabled
    if (demonstrationLearningEnabled_) {
        demonstrationExperiences_.push_back(record);
        // Could implement imitation learning here
    }
}

void AgentBrainAdvanced::setTargetPosition(float x, float y) {
    targetPosition_ = NavigationNode(x, y);
    
    if (pathfindingEnabled_) {
        // Calculate path using A*
        NavigationNode start(advancedState_.x, advancedState_.y);
        currentPath_ = pathfinder_.findPath(start, targetPosition_);
        
        if (!currentPath_.empty()) {
            NLM_LOG_INFO("Found path with " + std::to_string(currentPath_.size()) + " waypoints");
        }
    }
}

void AgentBrainAdvanced::addWaypoint(float x, float y) {
    waypoints_.push_back(NavigationNode(x, y));
    
    // Rebuild path
    if (!waypoints_.empty()) {
        NavigationNode start(advancedState_.x, advancedState_.y);
        NavigationNode goal = waypoints_.back();
        currentPath_ = pathfinder_.findPath(start, goal);
    }
}

void AgentBrainAdvanced::clearPath() {
    currentPath_.clear();
    waypoints_.clear();
    targetPosition_ = NavigationNode(0.0f, 0.0f);
}

float AgentBrainAdvanced::getDistanceToTarget() const {
    if (targetPosition_.x == 0.0f && targetPosition_.y == 0.0f) return 0.0f;
    
    float dx = targetPosition_.x - advancedState_.x;
    float dy = targetPosition_.y - advancedState_.y;
    return std::sqrt(dx * dx + dy * dy);
}

void AgentBrainAdvanced::enableDemonstrationLearning(bool enable) {
    demonstrationLearningEnabled_ = enable;
    NLM_LOG_INFO("Demonstration learning " + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrainAdvanced::enablePathfinding(bool enable) {
    pathfindingEnabled_ = enable;
    NLM_LOG_INFO("Pathfinding " + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrainAdvanced::enableSocialLearning(bool enable) {
    socialLearningEnabled_ = enable;
    NLM_LOG_INFO("Social learning " + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrainAdvanced::addCustomCommand(const std::string& name, AdvancedCommand command) {
    // In a real implementation, this would add to a command registry
    // For now, just log it
    NLM_LOG_INFO("Added custom command: " + name + " (type " + std::to_string(static_cast<int>(command)) + ")");
}

void AgentBrainAdvanced::removeCommand(AdvancedCommand command) {
    // In a real implementation, this would remove from command registry
    NLM_LOG_INFO("Removed command: " + std::to_string(static_cast<int>(command)));
}

std::vector<std::string> AgentBrainAdvanced::getAvailableCommands() const {
    std::vector<std::string> commandNames;
    commandNames.push_back("NoOp");
    commandNames.push_back("MoveToTarget");
    commandNames.push_back("FollowPath");
    commandNames.push_back("ExploreArea");
    commandNames.push_back("AvoidDanger");
    commandNames.push_back("SeekResource");
    commandNames.push_back("SocialInteraction");
    commandNames.push_back("LearnFromDemonstration");
    commandNames.push_back("PlanAction");
    commandNames.push_back("ExecuteSequence");
    commandNames.push_back("AdaptiveResponse");
    return commandNames;
}

float AgentBrainAdvanced::getCuriosityLevelAdvanced() const {
    return advancedState_.curiosity;
}

float AgentBrainAdvanced::getExplorationRate() const {
    return explorationBias_ * advancedState_.curiosity;
}

size_t AgentBrainAdvanced::getExperienceMemorySize() const {
    return recentExperiences_.size();
}

void AgentBrainAdvanced::setExplorationBias(float bias) noexcept {
    explorationBias_ = std::clamp(bias, 0.0f, 1.0f);
}

void AgentBrainAdvanced::setRiskAversion(float aversion) noexcept {
    riskAversion_ = std::clamp(aversion, 0.0f, 1.0f);
}

void AgentBrainAdvanced::setSocialInfluence(float influence) noexcept {
    socialInfluence_ = std::clamp(influence, 0.0f, 1.0f);
}

void AgentBrainAdvanced::compressMemory() {
    // Simple memory compression - keep most recent experiences
    if (recentExperiences_.size() > 1000) {
        size_t keepCount = std::min<size_t>(recentExperiences_.size() / 2, 1000);
        recentExperiences_.erase(recentExperiences_.begin(), 
                               recentExperiences_.end() - keepCount);
        NLM_LOG_INFO("Compressed memory to " + std::to_string(recentExperiences_.size()) + " experiences");
    }
}

void AgentBrainAdvanced::pruneOldExperiences(size_t maxKeep) {
    if (recentExperiences_.size() > maxKeep) {
        size_t removeCount = recentExperiences_.size() - maxKeep;
        recentExperiences_.erase(recentExperiences_.begin(), 
                               recentExperiences_.begin() + removeCount);
        NLM_LOG_INFO("Pruned " + std::to_string(removeCount) + " old experiences");
    }
}

void AgentBrainAdvanced::exportExperiences(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + filename);
        return;
    }
    
    file << "Learning Experiences Export" << std::endl;
    file << "===========================" << std::endl;
    file << "Total experiences: " << recentExperiences_.size() << std::endl;
    file << std::endl;
    
    for (size_t i = 0; i < recentExperiences_.size(); ++i) {
        const auto& exp = recentExperiences_[i];
        file << "Experience " << i << ":" << std::endl;
        file << "  Context: " << exp.context << std::endl;
        file << "  State: [";
        for (size_t j = 0; j < exp.state.size(); ++j) {
            file << exp.state[j];
            if (j < exp.state.size() - 1) file << ", ";
        }
        file << "]" << std::endl;
        file << "  Action: " << static_cast<int>(exp.action) << std::endl;
        file << "  Reward: " << exp.reward << std::endl;
        file << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Exported " + std::to_string(recentExperiences_.size()) + 
                 " experiences to " + filename);
}

void AgentBrainAdvanced::importExperiences(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for import: " + filename);
        return;
    }
    
    std::string line;
    std::vector<float> state;
    AdvancedCommand action = AdvancedCommand::NoOp;
    float reward = 0.0f;
    std::string context;
    size_t lineNum = 0;
    size_t expNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        if (line.find("Experience ") == 0) {
            // Start of new experience
            if (!context.empty()) {
                // Save previous experience
                recentExperiences_.push_back(LearningRecord(state, action, reward, context));
                expNum++;
            }
            
            context = "";
            state.clear();
            action = AdvancedCommand::NoOp;
            reward = 0.0f;
        } else if (line.find("Context: ") == 0) {
            context = line.substr(9);
        } else if (line.find("State: [") == 0) {
            // Parse state values
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            std::string stateStr = line.substr(start, end - start);
            
            std::stringstream ss(stateStr);
            std::string token;
            while (std::getline(ss, token, ',')) {
                state.push_back(std::stof(token));
            }
        } else if (line.find("Action: ") == 0) {
            action = static_cast<AdvancedCommand>(std::stoi(line.substr(8)));
        } else if (line.find("Reward: ") == 0) {
            reward = std::stof(line.substr(8));
        }
    }
    
    // Save last experience
    if (!context.empty()) {
        recentExperiences_.push_back(LearningRecord(state, action, reward, context));
        expNum++;
    }
    
    file.close();
    NLM_LOG_INFO("Imported " + std::to_string(recentExperiences_.size()) + 
                 " experiences from " + filename);
}

void AgentBrainAdvanced::setDecisionConfidenceThreshold(float threshold) noexcept {
    decisionConfidenceThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

void AgentBrainAdvanced::setActionSelectionMethod(int method) noexcept {
    actionSelectionMethod_ = method;
}

void AgentBrainAdvanced::updatePolicyGradient(const std::vector<float>& state, 
                                              AdvancedCommand action, 
                                              float reward) {
    // Policy gradient update for learning
    // This is a simplified version - could be enhanced with neural network
    decisionMaker_.updatePolicy(state, action, reward);
    
    // Store for analysis
    std::stringstream ss;
    ss << "pg_" << recentExperiences_.size();
    recentExperiences_.push_back(LearningRecord(state, action, reward, ss.str()));
}

void AgentBrainAdvanced::updateQLearning(const std::vector<float>& state, 
                                        AdvancedCommand action, 
                                        float reward, 
                                        const std::vector<float>& nextState) {
    // Q-learning update
    decisionMaker_.updatePolicy(state, action, reward);
    
    // Also update with next state if provided
    if (!nextState.empty()) {
        // Could implement more sophisticated Q-learning here
        // For now, just store the experience
        std::stringstream ss;
        ss << "ql_" << recentExperiences_.size();
        recentExperiences_.push_back(LearningRecord(state, action, reward, ss.str()));
    }
}

void AgentBrainAdvanced::processSensoryInput(const SensoryPercept& percept) {
    // Process with base class
    AgentBrain::processSensoryInput(percept);
    
    // Update advanced state from sensory input
    advancedState_.x = percept.getVision()[0]; // Simplified - use first vision value
    advancedState_.y = percept.getVision()[1]; // Simplified
    
    // Update emotional state based on sensory input
    float visionIntensity = 0.0f;
    for (float val : percept.getVision()) {
        visionIntensity += val;
    }
    visionIntensity /= percept.getVision().size();
    
    updateEmotionalResponse(visionIntensity, percept.getInternal()[0]);
}

MotorCommand AgentBrainAdvanced::decodeMotorCommand() {
    // Select advanced action
    AdvancedCommand advancedCmd = selectAdvancedAction();
    
    // Convert to base motor command
    MotorCommand baseCmd = MotorCommand::Wait;
    
    switch (advancedCmd) {
        case AdvancedCommand::NoOp:
            baseCmd = MotorCommand::Wait;
            break;
        case AdvancedCommand::MoveToTarget:
            baseCmd = MotorCommand::MoveForward;
            break;
        case AdvancedCommand::ExploreArea:
            baseCmd = MotorCommand::TurnLeft;
            break;
        case AdvancedCommand::AvoidDanger:
            baseCmd = MotorCommand::TurnRight;
            break;
        case AdvancedCommand::SeekResource:
            baseCmd = MotorCommand::MoveForward;
            break;
        case AdvancedCommand::SocialInteraction:
            baseCmd = MotorCommand::Interact;
            break;
        case AdvancedCommand::LearnFromDemonstration:
            baseCmd = MotorCommand::Wait; // Pause for learning
            break;
        case AdvancedCommand::PlanAction:
            baseCmd = MotorCommand::TurnLeft;
            break;
        case AdvancedCommand::ExecuteSequence:
            baseCmd = MotorCommand::MoveForward;
            break;
        case AdvancedCommand::AdaptiveResponse:
            baseCmd = MotorCommand::Interact;
            break;
    }
    
    return baseCmd;
}

void AgentBrainAdvanced::updateAdvancedDecisionMaking(double timestep) {
    // Update decision maker based on recent experiences
    if (!recentExperiences_.empty() && 
        std::chrono::steady_clock::now() - lastLearningUpdate_ > 
        std::chrono::milliseconds(100)) {
        
        // Sample a batch of experiences for learning
        size_t sampleSize = std::min<size_t>(recentExperiences_.size(), 10);
        for (size_t i = 0; i < sampleSize; ++i) {
            size_t idx = (std::rand() % recentExperiences_.size());
            const auto& exp = recentExperiences_[idx];
            decisionMaker_.updatePolicy(exp.state, exp.action, exp.reward);
        }
    }
}

void AgentBrainAdvanced::updateNavigation(double timestep) {
    if (pathfindingEnabled_ && !currentPath_.empty()) {
        // Move towards next waypoint
        if (!currentPath_.empty()) {
            NavigationNode& nextWaypoint = currentPath_[0];
            
            float dx = nextWaypoint.x - advancedState_.x;
            float dy = nextWaypoint.y - advancedState_.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance < 0.1f) {
                // Reached waypoint, remove it
                currentPath_.erase(currentPath_.begin());
                
                // If no more waypoints, go to next goal
                if (currentPath_.empty() && !waypoints_.empty()) {
                    NavigationNode start(advancedState_.x, advancedState_.y);
                    NavigationNode goal = waypoints_.front();
                    waypoints_.erase(waypoints_.begin());
                    currentPath_ = pathfinder_.findPath(start, goal);
                }
            } else {
                // Move towards waypoint
                float speed = 0.5f * timestep; // Movement speed
                advancedState_.x += (dx / distance) * speed;
                advancedState_.y += (dy / distance) * speed;
                
                // Update orientation to face direction
                advancedState_.orientation = std::atan2(dy, dx);
            }
        }
    }
}

void AgentBrainAdvanced::updateLearningSystems(double timestep) {
    // Update learning systems based on experiences
    if (demonstrationLearningEnabled_ && !demonstrationExperiences_.empty()) {
        // Could implement imitation learning from demonstrations
        // For now, just count
        static size_t demonstrationCount = 0;
        demonstrationCount++;
        if (demonstrationCount % 100 == 0) {
            NLM_LOG_INFO("Processed " + std::to_string(demonstrationCount) + 
                         " demonstration experiences");
        }
    }
}

void AgentBrainAdvanced::updateSocialSystems(double timestep) {
    if (socialLearningEnabled_) {
        // Update social influence
        advancedState_.socialBond += socialInfluence_ * timestep * 0.1f;
        advancedState_.socialBond = std::clamp(advancedState_.socialBond, 0.0f, 1.0f);
        
        // Social learning from nearby agents (simplified)
        float socialLearningRate = socialInfluence_ * 0.05f;
        advancedState_.curiosity += socialLearningRate * timestep;
    }
}

void AgentBrainAdvanced::updateEmotionalState(double timestep) {
    // Update emotional state based on various factors
    advancedState_.stress *= 0.95f; // Stress decays
    
    // Curiosity increases with exploration
    if (advancedState_.curiosity < 1.0f) {
        advancedState_.curiosity += 0.01f * timestep;
    }
    
    // Energy changes based on activity
    advancedState_.energy *= 0.99f; // Energy naturally depletes
    advancedState_.energy = std::clamp(advancedState_.energy, 0.0f, 1.0f);
}

AdvancedCommand AgentBrainAdvanced::executePathfinding() {
    if (currentPath_.empty() && targetPosition_.x != 0.0f) {
        NavigationNode start(advancedState_.x, advancedState_.y);
        currentPath_ = pathfinder_.findPath(start, targetPosition_);
    }
    
    if (!currentPath_.empty()) {
        return AdvancedCommand::MoveToTarget;
    }
    
    return AdvancedCommand::ExploreArea;
}

AdvancedCommand AgentBrainAdvanced::executeExploration() {
    // Explore based on curiosity and novelty
    if (advancedState_.curiosity > 0.8f) {
        return AdvancedCommand::ExploreArea;
    } else if (advancedState_.energy < 0.3f) {
        return AdvancedCommand::SeekResource;
    }
    
    return AdvancedCommand::NoOp;
}

AdvancedCommand AgentBrainAdvanced::executeSocialBehavior() {
    if (socialLearningEnabled_ && advancedState_.socialBond > 0.5f) {
        return AdvancedCommand::SocialInteraction;
    }
    
    return AdvancedCommand::NoOp;
}

AdvancedCommand AgentBrainAdvanced::executeAdaptiveBehavior() {
    // Choose based on current state and goals
    if (advancedState_.stress > 0.7f) {
        return AdvancedCommand::AvoidDanger;
    } else if (advancedState_.curiosity > 0.6f) {
        return AdvancedCommand::LearnFromDemonstration;
    } else if (advancedState_.energy < 0.2f) {
        return AdvancedCommand::SeekResource;
    }
    
    return AdvancedCommand::PlanAction;
}

std::vector<float> AgentBrainAdvanced::extractFeatures(const SensoryPercept& percept, 
                                                      const AdvancedAgentState& state) const {
    std::vector<float> features;
    
    // Add state features
    features.push_back(state.x);
    features.push_back(state.y);
    features.push_back(state.orientation);
    features.push_back(state.energy);
    features.push_back(state.curiosity);
    features.push_back(state.stress);
    features.push_back(state.socialBond);
    
    // Add sensory features
    for (float val : percept.getVision()) {
        features.push_back(val);
    }
    
    for (float val : percept.getTouch()) {
        features.push_back(val);
    }
    
    for (float val : percept.getInternal()) {
        features.push_back(val);
    }
    
    return features;
}

float AgentBrainAdvanced::calculateActionValue(const std::vector<float>& state, 
                                               AdvancedCommand action) const {
    // Simple heuristic-based action value calculation
    float value = 0.0f;
    
    // Distance to target
    float dx = targetPosition_.x - state.x;
    float dy = targetPosition_.y - state.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    value += distance * 0.1f;
    
    // Energy consideration
    value += (1.0f - state.energy) * 0.2f;
    
    // Curiosity factor
    value += state.curiosity * 0.3f;
    
    // Stress factor
    value += state.stress * 0.4f;
    
    return value;
}

void AgentBrainAdvanced::updateEmotionalResponse(float reward, float predictionError) {
    // Update emotional state based on reward and prediction error
    if (reward > 0.5f) {
        // Positive reward - increase positive emotions
        advancedState_.curiosity += 0.1f;
        advancedState_.socialBond += 0.05f;
    } else if (reward < -0.5f) {
        // Negative reward - increase stress
        advancedState_.stress += 0.2f;
        advancedState_.energy -= 0.1f;
    }
    
    // Prediction error affects learning and attention
    if (std::abs(predictionError) > 0.3f) {
        // High prediction error - increased curiosity
        advancedState_.curiosity += 0.15f;
    }
    
    // Clamp values
    advancedState_.curiosity = std::clamp(advancedState_.curiosity, 0.0f, 1.0f);
    advancedState_.stress = std::clamp(advancedState_.stress, 0.0f, 1.0f);
    advancedState_.socialBond = std::clamp(advancedState_.socialBond, 0.0f, 1.0f);
}

} // namespace nlm
