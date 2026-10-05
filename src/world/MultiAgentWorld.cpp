#include "MultiAgentWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>

namespace nlm {

MultiAgentWorld::MultiAgentWorld()
    : lastStatsUpdate_(0.0), avgCooperationLevel_(0.0f),
      avgTrustLevel_(0.0f), avgTaskCompletionRate_(0.0f),
      maxEventLogSize_(1000)
{
    NLM_LOG_INFO("MultiAgentWorld initialized with advanced multi-agent dynamics");
}

MultiAgentWorld::~MultiAgentWorld() {
    NLM_LOG_INFO("MultiAgentWorld destroyed");
}

void MultiAgentWorld::addAgent(uint64_t agentId, float x, float y, float radius,
                            const AgentPersonality& personality) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    // Check if agent already exists
    if (agents_.find(agentId) != agents_.end()) {
        NLM_LOG_WARNING("Agent " + std::to_string(agentId) + " already exists");
        return;
    }
    
    // Create new agent info
    AgentInfo info;
    info.agentId = agentId;
    info.personality = personality;
    info.decisionState.agentId = agentId;
    info.decisionState.energy = 10.0f;
    info.decisionState.hunger = 0.5f;
    info.decisionState.fatigue = 0.0f;
    info.decisionState.stress = 0.0f;
    info.decisionState.motivation = 0.5f;
    info.decisionState.currentGoal = "idle";
    info.decisionState.goalProgress = 0.0f;
    info.lastInteractionTime = 0.0;
    info.socialEnergy = 10.0f;
    info.socialNeed = 0.5f;
    info.stressLevel = 0.0f;
    
    // Add agent to world (using parent class method)
    addAgentBody(x, y, radius);
    agents_[agentId] = info;
    
    // Update cooperation network
    establishConnection(agentId, agentId);  // Self-connection for initialization
    
    NLM_LOG_INFO("Added multi-agent agent " + std::to_string(agentId) + 
                 " with personality (exploration: " + std::to_string(personality.exploration) +
                 ", social: " + std::to_string(personality.social) + ")");
}

void MultiAgentWorld::removeAgent(uint64_t agentId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(agentId) == agents_.end()) {
        NLM_LOG_WARNING("Cannot remove agent " + std::to_string(agentId) + ": agent not found");
        return;
    }
    
    // Remove all connections with this agent
    AgentInfo& agent = agents_[agentId];
    for (const auto& connection : agent.socialConnections) {
        // Remove agent from other agent's connections
        auto otherIt = agents_.find(std::stoull(connection));
        if (otherIt != agents_.end()) {
            otherIt->second.socialConnections.erase(
                std::remove(otherIt->second.socialConnections.begin(),
                           otherIt->second.socialConnections.end(),
                           std::to_string(agentId)),
                otherIt->second.socialConnections.end());
        }
    }
    
    // Remove from world (using parent class method)
    removeAgentBody();  // Note: This is a simplification
    
    // Remove from social state
    agents_.erase(agentId);
    
    NLM_LOG_INFO("Removed multi-agent agent " + std::to_string(agentId));
}

AgentBody* MultiAgentWorld::getAgentBody(uint64_t agentId) {
    // Simplified implementation - would need proper agent body mapping
    // For now, return the main agent body
    return &SimpleWorld::getAgentBody();
}

const AgentBody* MultiAgentWorld::getAgentBody(uint64_t agentId) const {
    // Simplified implementation - would need proper agent body mapping
    // For now, return the main agent body
    return &SimpleWorld::getAgentBody();
}

AgentDecisionState* MultiAgentWorld::getAgentState(uint64_t agentId) {
    auto it = agents_.find(agentId);
    return (it != agents_.end()) ? &it->second.decisionState : nullptr;
}

const AgentDecisionState* MultiAgentWorld::getAgentState(uint64_t agentId) const {
    auto it = agents_.find(agentId);
    return (it != agents_.end()) ? &it->second.decisionState : nullptr;
}

void MultiAgentWorld::updateAgentState(uint64_t agentId, double timestep) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    AgentInfo& agent = it->second;
    AgentDecisionState& state = agent.decisionState;
    
    // Update agent's perception based on current world state
    updateAgentPerception(agent, timestep);
    
    // Update internal states
    state.energy = std::max(0.0f, state.energy - 0.01f * timestep);
    state.hunger = std::min(1.0f, state.hunger + 0.005f * timestep);
    state.fatigue = std::min(1.0f, state.fatigue + 0.003f * timestep);
    state.stress = std::max(0.0f, state.stress - 0.01f * timestep);
    state.motivation = std::max(0.0f, state.motivation - 0.002f * timestep);
    
    // Social energy decay
    agent.socialEnergy *= std::pow(0.98f, timestep);
    
    // Social need growth
    agent.socialNeed = std::min(1.0f, agent.socialNeed + 0.001f * timestep);
    
    // Stress recovery from positive social interactions
    if (agent.socialEnergy > 5.0f) {
        agent.stressLevel = std::max(0.0f, agent.stressLevel - 0.005f * timestep);
    }
    
    // Last interaction time update
    agent.lastInteractionTime += timestep;
}

void MultiAgentWorld::updateAgentEnergy(uint64_t agentId, float energyChange) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    it->second.decisionState.energy = std::max(0.0f, 
        it->second.decisionState.energy + energyChange);
    
    // Update social energy based on energy
    if (energyChange > 0.0f) {
        it->second.socialEnergy += energyChange * 0.5f;
    }
}

void MultiAgentWorld::updateAgentHunger(uint64_t agentId, float hungerChange) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    it->second.decisionState.hunger = std::clamp(it->second.decisionState.hunger + hungerChange, 0.0f, 1.0f);
}

void MultiAgentWorld::updateAgentFatigue(uint64_t agentId, float fatigueChange) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    it->second.decisionState.fatigue = std::clamp(it->second.decisionState.fatigue + fatigueChange, 0.0f, 1.0f);
}

void MultiAgentWorld::updateAgentStress(uint64_t agentId, float stressChange) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    it->second.stressLevel = std::clamp(it->second.stressLevel + stressChange, 0.0f, 1.0f);
}

void MultiAgentWorld::updateAgentMotivation(uint64_t agentId, float motivationChange) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    it->second.decisionState.motivation = std::max(0.0f, 
        it->second.decisionState.motivation + motivationChange);
}

void MultiAgentWorld::updateAgentBehavior(uint64_t agentId, double timestep) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    AgentInfo& agent = it->second;
    
    // Generate action plan
    ActionPlan plan = generateActionPlan(agentId);
    
    // Execute action plan
    executeActionPlan(agentId, plan);
    
    // Learn from experience
    learnFromExperience(agentId, plan.expectedOutcomes);
}

ActionPlan MultiAgentWorld::generateActionPlan(uint64_t agentId) {
    ActionPlan plan;
    plan.agentId = std::to_string(agentId);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return plan;
    }
    
    const AgentInfo& agent = it->second;
    const AgentDecisionState& state = agent.decisionState;
    
    // Determine optimal action based on agent's state and personality
    std::vector<std::string> actions;
    std::vector<float> outcomes;
    
    // Exploration action
    if (agent.personality.exploration > 0.7f && state.energy > 5.0f) {
        actions.push_back("explore_new_area");
        outcomes.push_back(calculateExplorationOutcome(agent, state));
    }
    
    // Social interaction action
    if (agent.personality.social > 0.6f && agent.socialNeed > 0.4f && 
        agent.socialEnergy > 3.0f) {
        actions.push_back("social_interaction");
        outcomes.push_back(calculateSocialInteractionOutcome(agent, state));
    }
    
    // Resource gathering action
    if (state.hunger > 0.6f) {
        actions.push_back("forage");
        outcomes.push_back(calculateForageOutcome(agent, state));
    }
    
    // Rest action
    if (state.fatigue > 0.7f || state.energy < 2.0f) {
        actions.push_back("rest");
        outcomes.push_back(calculateRestOutcome(agent, state));
    }
    
    // If no actions selected, idle action
    if (actions.empty()) {
        actions.push_back("idle");
        outcomes.push_back(0.0f);
    }
    
    // Select best action based on personality and outcomes
    float bestOutcome = -1000.0f;
    size_t bestActionIndex = 0;
    
    for (size_t i = 0; i < outcomes.size(); ++i) {
        float adjustedOutcome = outcomes[i];
        
        // Adjust outcome based on personality traits
        if (actions[i] == "explore_new_area" && agent.personality.exploration < 0.5f) {
            adjustedOutcome *= 0.5f;
        }
        
        if (actions[i] == "social_interaction" && agent.personality.social < 0.5f) {
            adjustedOutcome *= 0.5f;
        }
        
        if (actions[i] == "forage" && state.energy < 3.0f) {
            adjustedOutcome *= 0.6f;
        }
        
        if (actions[i] == "rest" && agent.personality.exploration > 0.8f) {
            adjustedOutcome *= 0.7f;
        }
        
        if (adjustedOutcome > bestOutcome) {
            bestOutcome = adjustedOutcome;
            bestActionIndex = i;
        }
    }
    
    // Build action plan
    plan.plannedActions = actions;
    plan.expectedOutcomes = outcomes;
    plan.planEfficiency = bestOutcome;
    plan.planRisk = 0.5f;  // Default risk
    plan.planDuration = 1.0f;  // Default duration
    plan.planContext = "agent_behavior_update";
    plan.preconditions = {"agent_alive", "world_stable"};
    plan.postconditions = {"action_completed", "state_updated"};
    
    return plan;
}

void MultiAgentWorld::executeActionPlan(uint64_t agentId, const ActionPlan& plan) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    AgentInfo& agent = it->second;
    AgentDecisionState& state = agent.decisionState;
    
    // Execute each planned action
    for (size_t i = 0; i < plan.plannedActions.size() && i < plan.expectedOutcomes.size(); ++i) {
        const std::string& action = plan.plannedActions[i];
        float expectedOutcome = plan.expectedOutcomes[i];
        
        // Update agent state based on action
        if (action == "explore_new_area") {
            updateAgentExploration(agent, expectedOutcome);
        } else if (action == "social_interaction") {
            updateAgentSocialInteraction(agent, expectedOutcome);
        } else if (action == "forage") {
            updateAgentForaging(agent, expectedOutcome);
        } else if (action == "rest") {
            updateAgentRest(agent, expectedOutcome);
        }
        
        // Record action outcome
        recordActionOutcome(agentId, action, expectedOutcome);
    }
    
    // Update agent's task based on executed actions
    updateAgentTasks(agentId, plan.plannedActions);
}

void MultiAgentWorld::learnFromExperience(uint64_t agentId, const std::vector<float>& outcomes) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    AgentInfo& agent = it->second;
    
    // Learn from outcomes
    float totalOutcome = 0.0f;
    for (float outcome : outcomes) {
        totalOutcome += outcome;
    }
    
    float avgOutcome = outcomes.empty() ? 0.0f : totalOutcome / outcomes.size();
    
    // Update agent's learned knowledge based on outcomes
    if (avgOutcome > 0.5f) {
        // Positive outcome - reinforce learned actions
        agent.learnedActions.push_back("successful_action");
        agent.decisionState.motivation += 0.1f;
    } else if (avgOutcome < -0.5f) {
        // Negative outcome - adjust learned actions
        agent.learnedActions.push_back("unsuccessful_action");
        agent.decisionState.stress += 0.05f;
    }
    
    // Update cultural beliefs
    if (avgOutcome > 0.0f) {
        agent.culturalBeliefs["cooperation"] = std::min(1.0f, 
            agent.culturalBeliefs["cooperation"] + 0.05f);
    }
}

void MultiAgentWorld::assignTask(const AgentTask& task) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    tasks_[task.taskId] = task;
    
    // Assign task to agent
    auto agentIt = agents_.find(std::stoull(task.agentId));
    if (agentIt != agents_.end()) {
        agentIt->second.tasks.push_back(task);
    }
    
    NLM_LOG_DEBUG("Assigned task: " + task.taskId + " to agent " + task.agentId);
}

void MultiAgentWorld::completeTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto taskIt = tasks_.find(taskId);
    if (taskIt == tasks_.end()) {
        return;
    }
    
    AgentTask& task = taskIt->second;
    task.status = "completed";
    task.currentProgress = 1.0f;
    
    // Find agent and update their task list
    for (auto& pair : agents_) {
        AgentInfo& agent = pair.second;
        for (auto taskIt = agent.tasks.begin(); taskIt != agent.tasks.end(); ++taskIt) {
            if (taskIt->taskId == taskId) {
                taskIt->status = "completed";
                taskIt->currentProgress = 1.0f;
                NLM_LOG_DEBUG("Task " + taskId + " completed by agent " + task.agentId);
                return;
            }
        }
    }
}

void MultiAgentWorld::failTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto taskIt = tasks_.find(taskId);
    if (taskIt == tasks_.end()) {
        return;
    }
    
    AgentTask& task = taskIt->second;
    task.status = "failed";
    
    // Find agent and update their task list
    for (auto& pair : agents_) {
        AgentInfo& agent = pair.second;
        for (auto taskIt = agent.tasks.begin(); taskIt != agent.tasks.end(); ++taskIt) {
            if (taskIt->taskId == taskId) {
                taskIt->status = "failed";
                NLM_LOG_DEBUG("Task " + taskId + " failed for agent " + task.agentId);
                return;
            }
        }
    }
}

AgentTask* MultiAgentWorld::getTask(const std::string& taskId) {
    auto it = tasks_.find(taskId);
    return (it != tasks_.end()) ? &it->second : nullptr;
}

std::vector<AgentTask> MultiAgentWorld::getAgentTasks(uint64_t agentId) const {
    std::vector<AgentTask> agentTasks;
    
    auto it = agents_.find(agentId);
    if (it != agents_.end()) {
        agentTasks = it->second.tasks;
    }
    
    return agentTasks;
}

void MultiAgentWorld::sendMessage(const CommunicationMessage& message) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    // Add to communications
    communications_[message.senderId + "_to_" + message.receiverId] = message;
    
    // Add to agent's message queue if receiver exists
    auto receiverIt = agents_.find(std::stoull(message.receiverId));
    if (receiverIt != agents_.end()) {
        receiverIt->second.messageQueue.push(message);
    }
    
    NLM_LOG_DEBUG("Message sent: " + message.senderId + " -> " + message.receiverId +
                 " type: " + message.messageType);
}

CommunicationMessage MultiAgentWorld::receiveMessage(uint64_t agentId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    CommunicationMessage message;
    
    auto agentIt = agents_.find(agentId);
    if (agentIt != agents_.end()) {
        if (!agentIt->second.messageQueue.empty()) {
            message = agentIt->second.messageQueue.front();
            agentIt->second.messageQueue.pop();
        }
    }
    
    return message;
}

std::vector<CommunicationMessage> MultiAgentWorld::getMessages(uint64_t agentId) const {
    std::vector<CommunicationMessage> messages;
    
    auto it = agents_.find(agentId);
    if (it != agents_.end()) {
        messages = std::vector<CommunicationMessage>(it->second.messageQueue);
    }
    
    return messages;
}

void MultiAgentWorld::processIncomingMessages(uint64_t agentId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        return;
    }
    
    AgentInfo& agent = it->second;
    
    // Process all messages in queue
    while (!agent.messageQueue.empty()) {
        CommunicationMessage message = agent.messageQueue.front();
        agent.messageQueue.pop();
        
        // Process message based on type
        if (message.messageType == "proposal") {
            processProposalMessage(agent, message);
        } else if (message.messageType == "request") {
            processRequestMessage(agent, message);
        } else if (message.messageType == "information") {
            processInformationMessage(agent, message);
        } else if (message.messageType == "coordination") {
            processCoordinationMessage(agent, message);
        }
        
        // Remove from communications if it was there
        std::string commKey = message.senderId + "_to_" + message.receiverId;
        communications_.erase(commKey);
    }
}

void MultiAgentWorld::establishConnection(uint64_t agentId1, uint64_t agentId2) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agentId1 == agentId2) {
        return;  // No self-connections
    }
    
    // Check if agents exist
    if (agents_.find(agentId1) == agents_.end() || 
        agents_.find(agentId2) == agents_.end()) {
        return;
    }
    
    // Add to both agents' social connections
    agents_[agentId1].socialConnections.push_back(std::to_string(agentId2));
    agents_[agentId2].socialConnections.push_back(std::to_string(agentId1));
    
    // Update trust levels
    updateTrustLevel(agentId1, agentId2, 0.1f);
    updateTrustLevel(agentId2, agentId1, 0.1f);
    
    NLM_LOG_DEBUG("Connection established between agents " + std::to_string(agentId1) +
                 " and " + std::to_string(agentId2));
}

void MultiAgentWorld::removeConnection(uint64_t agentId1, uint64_t agentId2) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    // Remove from agent1's connections
    auto agent1It = agents_.find(agentId1);
    if (agent1It != agents_.end()) {
        auto& connections = agent1It->second.socialConnections;
        connections.erase(std::remove(connections.begin(), connections.end(),
                                     std::to_string(agentId2)),
                         connections.end());
    }
    
    // Remove from agent2's connections
    auto agent2It = agents_.find(agentId2);
    if (agent2It != agents_.end()) {
        auto& connections = agent2It->second.socialConnections;
        connections.erase(std::remove(connections.begin(), connections.end(),
                                     std::to_string(agentId1)),
                         connections.end());
    }
    
    // Update trust levels
    updateTrustLevel(agentId1, agentId2, -0.05f);
    updateTrustLevel(agentId2, agentId1, -0.05f);
    
    NLM_LOG_DEBUG("Connection removed between agents " + std::to_string(agentId1) +
                 " and " + std::to_string(agentId2));
}

bool MultiAgentWorld::areConnected(uint64_t agentId1, uint64_t agentId2) const {
    auto agent1It = agents_.find(agentId1);
    auto agent2It = agents_.find(agentId2);
    
    if (agent1It == agents_.end() || agent2It == agents_.end()) {
        return false;
    }
    
    const auto& connections1 = agent1It->second.socialConnections;
    const auto& connections2 = agent2It->second.socialConnections;
    
    return std::find(connections1.begin(), connections1.end(), std::to_string(agentId2)) != connections1.end() &&
           std::find(connections2.begin(), connections2.end(), std::to_string(agentId1)) != connections2.end();
}

void MultiAgentWorld::updateTrustLevel(uint64_t agentId1, uint64_t agentId2, float trustChange) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto agent1It = agents_.find(agentId1);
    auto agent2It = agents_.find(agentId2);
    
    if (agent1It == agents_.end() || agent2It == agents_.end()) {
        return;
    }
    
    // Update trust levels in both directions
    if (cooperationNetwork_.trustLevels.find(std::to_string(agentId1) + "_to_" + std::to_string(agentId2)) != 
        cooperationNetwork_.trustLevels.end()) {
        cooperationNetwork_.trustLevels[std::to_string(agentId1) + "_to_" + std::to_string(agentId2)] =
            std::clamp(cooperationNetwork_.trustLevels[std::to_string(agentId1) + "_to_" + std::to_string(agentId2)] + trustChange,
                     0.0f, 1.0f);
    } else {
        cooperationNetwork_.trustLevels[std::to_string(agentId1) + "_to_" + std::to_string(agentId2)] = trustChange;
    }
    
    if (cooperationNetwork_.trustLevels.find(std::to_string(agentId2) + "_to_" + std::to_string(agentId1)) != 
        cooperationNetwork_.trustLevels.end()) {
        cooperationNetwork_.trustLevels[std::to_string(agentId2) + "_to_" + std::to_string(agentId1)] =
            std::clamp(cooperationNetwork_.trustLevels[std::to_string(agentId2) + "_to_" + std::to_string(agentId1)] + trustChange,
                     0.0f, 1.0f);
    } else {
        cooperationNetwork_.trustLevels[std::to_string(agentId2) + "_to_" + std::to_string(agentId1)] = trustChange;
    }
}

void MultiAgentWorld::shareGoal(uint64_t agentId, const std::string& goalId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto agentIt = agents_.find(agentId);
    if (agentIt == agents_.end()) {
        return;
    }
    
    // Add to agent's shared goals
    cooperationNetwork_.sharedGoals[std::to_string(agentId)].push_back(goalId);
    
    // Share goal with connected agents
    for (const auto& connection : agentIt->second.socialConnections) {
        uint64_t connectedId = std::stoull(connection);
        cooperationNetwork_.sharedGoals[connection].push_back(goalId);
    }
    
    NLM_LOG_DEBUG("Agent " + std::to_string(agentId) + " shared goal: " + goalId);
}

void MultiAgentWorld::removeSharedGoal(uint64_t agentId, const std::string& goalId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto agentIt = agents_.find(agentId);
    if (agentIt == agents_.end()) {
        return;
    }
    
    // Remove from agent's shared goals
    auto& goals = cooperationNetwork_.sharedGoals[std::to_string(agentId)];
    goals.erase(std::remove(goals.begin(), goals.end(), goalId), goals.end());
    
    // Remove from connected agents
    for (const auto& connection : agentIt->second.socialConnections) {
        auto& goals = cooperationNetwork_.sharedGoals[connection];
        goals.erase(std::remove(goals.begin(), goals.end(), goalId), goals.end());
    }
}

float MultiAgentWorld::getAverageCooperationLevel() const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.empty()) {
        return 0.0f;
    }
    
    float totalCooperation = 0.0f;
    int agentCount = 0;
    
    for (const auto& pair : agents_) {
        const AgentInfo& agent = pair.second;
        
        // Calculate cooperation level based on personality and behavior
        float cooperation = (agent.personality.cooperativeness + 
                           agent.personality.social * 0.5f) / 2.0f;
        
        // Adjust based on social energy
        cooperation *= (agent.socialEnergy / 20.0f);
        
        totalCooperation += cooperation;
        agentCount++;
    }
    
    avgCooperationLevel_ = (agentCount > 0) ? totalCooperation / agentCount : 0.0f;
    return avgCooperationLevel_;
}

float MultiAgentWorld::getAverageTrustLevel() const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (cooperationNetwork_.trustLevels.empty()) {
        return 0.0f;
    }
    
    float totalTrust = 0.0f;
    int trustCount = 0;
    
    for (const auto& pair : cooperationNetwork_.trustLevels) {
        totalTrust += pair.second;
        trustCount++;
    }
    
    avgTrustLevel_ = (trustCount > 0) ? totalTrust / trustCount : 0.0f;
    return avgTrustLevel_;
}

float MultiAgentWorld::getAverageTaskCompletionRate() const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (tasks_.empty()) {
        return 0.0f;
    }
    
    float totalCompleted = 0.0f;
    int taskCount = 0;
    
    for (const auto& pair : tasks_) {
        const AgentTask& task = pair.second;
        
        if (task.status == "completed") {
            totalCompleted += 1.0f;
        }
        
        taskCount++;
    }
    
    avgTaskCompletionRate_ = (taskCount > 0) ? totalCompleted / taskCount : 0.0f;
    return avgTaskCompletionRate_;
}

std::string MultiAgentWorld::getCooperationNetworkGraph() const {
    std::stringstream ss;
    ss << "=== Cooperation Network Graph ===\n";
    ss << "Agents: " << agents_.size() << "\n";
    ss << "Connections: " << cooperationNetwork_.connections.size() << "\n";
    ss << "Trust Level: " << getAverageTrustLevel() << "\n";
    ss << "Cooperation Level: " << getAverageCooperationLevel() << "\n";
    ss << "Task Completion Rate: " << getAverageTaskCompletionRate() << "\n";
    
    ss << "\n--- Agent Connections ---\n";
    for (const auto& pair : cooperationNetwork_.connections) {
        ss << "Agent " << pair.first << " -> ";
        for (size_t i = 0; i < pair.second.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << pair.second[i];
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string MultiAgentWorld::getCommunicationTrafficReport() const {
    std::stringstream ss;
    ss << "=== Communication Traffic Report ===\n";
    ss << "Total Messages: " << communications_.size() << "\n";
    
    // Count messages by type
    std::unordered_map<std::string, int> messageTypes;
    for (const auto& pair : communications_) {
        messageTypes[pair.second.messageType]++;
    }
    
    ss << "\n--- Message Types ---\n";
    for (const auto& pair : messageTypes) {
        ss << pair.first << ": " << pair.second << "\n";
    }
    
    return ss.str();
}

std::string MultiAgentWorld::getCulturalTransmissionReport() const {
    std::stringstream ss;
    ss << "=== Cultural Transmission Report ===\n";
    ss << "Cultural Traits: " << culturalValues_.size() << "\n";
    
    ss << "\n--- Cultural Values ---\n";
    for (const auto& pair : culturalValues_) {
        ss << pair.first << ": " << pair.second << "\n";
    }
    
    ss << "\n--- Cultural Trends ---\n";
    for (const auto& pair : culturalTrends_) {
        ss << pair.first << ": " << pair.second.size() << " samples\n";
    }
    
    return ss.str();
}

void MultiAgentWorld::updateAgentPopulationDynamics(double timestep) {
    // Update agent population based on birth/death rates
    // Simplified implementation
}

void MultiAgentWorld::evolveAgentBehaviors(double timestep) {
    // Evolve agent behaviors based on performance
    // Simplified implementation
}

void MultiAgentWorld::culturalEvolution(double timestep) {
    // Evolve cultural traits
    // Simplified implementation
}

void MultiAgentWorld::ecologicalAgentCoupling(double timestep) {
    // Coupling between agent behaviors and ecological changes
    // Simplified implementation
}

std::string MultiAgentWorld::getMultiAgentWorldStateSnapshot() const {
    std::stringstream ss;
    ss << "=== Multi-Agent World State ===\n";
    ss << "Agents: " << agents_.size() << "\n";
    ss << "Tasks: " << tasks_.size() << "\n";
    ss << "Communications: " << communications_.size() << "\n";
    ss << "Cooperation Level: " << getAverageCooperationLevel() << "\n";
    ss << "Trust Level: " << getAverageTrustLevel() << "\n";
    ss << "Task Completion Rate: " << getAverageTaskCompletionRate() << "\n";
    
    ss << "\n--- Agent States ---\n";
    for (const auto& pair : agents_) {
        const AgentInfo& agent = pair.second;
        ss << "Agent " << agent.agentId << ":\n";
        ss << "  Personality: explore=" << agent.personality.exploration << 
           " social=" << agent.personality.social << 
           " risk=" << agent.personality.risk << "\n";
        ss << "  State: energy=" << agent.decisionState.energy << 
           " hunger=" << agent.decisionState.hunger << 
           " fatigue=" << agent.decisionState.fatigue << "\n";
        ss << "  Social: energy=" << agent.socialEnergy << 
           " need=" << agent.socialNeed << " stress=" << agent.stressLevel << "\n";
        ss << "  Tasks: " << agent.tasks.size() << "\n";
        ss << "  Connections: " << agent.socialConnections.size() << "\n";
    }
    
    return ss.str();
}

void MultiAgentWorld::visualizeCooperationNetwork() const {
    std::string networkGraph = getCooperationNetworkGraph();
    NLM_LOG_INFO("Cooperation Network Visualization:\n" + networkGraph);
}

void MultiAgentWorld::visualizeCommunicationFlow() const {
    std::string commReport = getCommunicationTrafficReport();
    NLM_LOG_INFO("Communication Flow Visualization:\n" + commReport);
}

void MultiAgentWorld::visualizeCulturalTransmission() const {
    std::string culturalReport = getCulturalTransmissionReport();
    NLM_LOG_INFO("Cultural Transmission Visualization:\n" + culturalReport);
}

// Implementation helper functions
float MultiAgentWorld::calculateExplorationOutcome(const AgentInfo& agent, 
                                                  const AgentDecisionState& state) {
    // Simplified exploration outcome calculation
    return agent.personality.exploration * state.energy / 20.0f;
}

float MultiAgentWorld::calculateSocialInteractionOutcome(const AgentInfo& agent,
                                                       const AgentDecisionState& state) {
    // Simplified social interaction outcome calculation
    return agent.personality.social * agent.socialEnergy / 200.0f;
}

float MultiAgentWorld::calculateForageOutcome(const AgentInfo& agent,
                                             const AgentDecisionState& state) {
    // Simplified foraging outcome calculation
    return (1.0f - state.hunger) * state.energy / 30.0f;
}

float MultiAgentWorld::calculateRestOutcome(const AgentInfo& agent,
                                          const AgentDecisionState& state) {
    // Simplified rest outcome calculation
    return (1.0f - state.fatigue) * (agent.personality.exploration * 0.5f);
}

void MultiAgentWorld::updateAgentExploration(AgentInfo& agent, float outcome) {
    // Update agent exploration based on outcome
    if (outcome > 0.5f) {
        agent.personality.exploration = std::min(1.0f, agent.personality.exploration + 0.05f);
    } else if (outcome < -0.5f) {
        agent.personality.exploration = std::max(0.0f, agent.personality.exploration - 0.05f);
    }
}

void MultiAgentWorld::updateAgentSocialInteraction(AgentInfo& agent, float outcome) {
    // Update agent social interaction based on outcome
    if (outcome > 0.5f) {
        agent.personality.social = std::min(1.0f, agent.personality.social + 0.05f);
        agent.socialEnergy += 1.0f;
    } else if (outcome < -0.5f) {
        agent.personality.social = std::max(0.0f, agent.personality.social - 0.05f);
        agent.stressLevel += 0.05f;
    }
}

void MultiAgentWorld::updateAgentForaging(AgentInfo& agent, float outcome) {
    // Update agent foraging based on outcome
    if (outcome > 0.5f) {
        agent.decisionState.hunger = std::max(0.0f, agent.decisionState.hunger - 0.2f);
        agent.decisionState.energy = std::min(20.0f, agent.decisionState.energy + 2.0f);
    } else {
        agent.decisionState.hunger = std::min(1.0f, agent.decisionState.hunger + 0.1f);
    }
}

void MultiAgentWorld::updateAgentRest(AgentInfo& agent, float outcome) {
    // Update agent rest based on outcome
    if (outcome > 0.5f) {
        agent.decisionState.fatigue = std::max(0.0f, agent.decisionState.fatigue - 0.3f);
        agent.decisionState.energy = std::min(20.0f, agent.decisionState.energy + 3.0f);
    } else {
        agent.decisionState.fatigue = std::min(1.0f, agent.decisionState.fatigue + 0.1f);
    }
}

void MultiAgentWorld::recordActionOutcome(uint64_t agentId, const std::string& action, float outcome) {
    // Record agent action outcome for learning
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto agentIt = agents_.find(agentId);
    if (agentIt == agents_.end()) {
        return;
    }
    
    AgentInfo& agent = agentIt->second;
    agent.learnedActions.push_back(action);
    
    if (outcome > 0.5f) {
        agent.culturalBeliefs["success"] = std::min(1.0f, agent.culturalBeliefs["success"] + 0.05f);
    } else if (outcome < -0.5f) {
        agent.culturalBeliefs["failure"] = std::max(0.0f, agent.culturalBeliefs["failure"] - 0.05f);
    }
}

void MultiAgentWorld::updateAgentTasks(uint64_t agentId, const std::vector<std::string>& executedActions) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto agentIt = agents_.find(agentId);
    if (agentIt == agents_.end()) {
        return;
    }
    
    AgentInfo& agent = agentIt->second;
    
    // Update tasks based on executed actions
    for (auto taskIt = agent.tasks.begin(); taskIt != agent.tasks.end(); ++taskIt) {
        if (taskIt->status == "in_progress") {
            // Simple task completion logic
            taskIt->currentProgress += 0.1f;
            
            if (taskIt->currentProgress >= 1.0f) {
                taskIt->status = "completed";
            }
        }
    }
}

void MultiAgentWorld::updateAgentPerception(AgentInfo& agent, double timestep) {
    // Update agent's perception of the world
    // Simplified implementation
    agent.decisionState.perception.clear();
    
    // Add environmental perception
    agent.decisionState.perception.push_back(agent.socialEnergy);
    agent.decisionState.perception.push_back(agent.socialNeed);
    agent.decisionState.perception.push_back(agent.stressLevel);
    
    // Add task-related perception
    for (const auto& task : agent.tasks) {
        if (task.status == "in_progress") {
            agent.decisionState.perception.push_back(task.currentProgress);
        }
    }
}

void MultiAgentWorld::processProposalMessage(AgentInfo& agent, const CommunicationMessage& message) {
    // Process proposal message
    // Simplified implementation
}

void MultiAgentWorld::processRequestMessage(AgentInfo& agent, const CommunicationMessage& message) {
    // Process request message
    // Simplified implementation
}

void MultiAgentWorld::processInformationMessage(AgentInfo& agent, const CommunicationMessage& message) {
    // Process information message
    // Simplified implementation
}

void MultiAgentWorld::processCoordinationMessage(AgentInfo& agent, const CommunicationMessage& message) {
    // Process coordination message
    // Simplified implementation
}

// Add missing method for agent body management
def addAgentBody(float x, float y, float radius) {
    // Add an agent body to the world
    // This would integrate with the parent SimpleWorld class
    // For now, just a placeholder
    NLM_LOG_DEBUG("Adding multi-agent body at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
}

def removeAgentBody() {
    // Remove the agent body from the world
    // This would integrate with the parent SimpleWorld class
    // For now, just a placeholder
    NLM_LOG_DEBUG("Removing multi-agent body");
}

} // namespace nlm
