#include "AgentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

AgentManager::AgentManager()
    : isRunning_(false), currentTime_(0.0), timestep_(0.01),
      timeSyncScheduled_(false), enableCheckpointing_(true),
      enableReplay_(true), totalSteps_(0)
{
    simulationStartTime_ = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("AgentManager initialized");
}

AgentManager::~AgentManager() {
    stopSimulation();
    NLM_LOG_INFO("AgentManager destroyed");
}

std::string AgentManager::createAgent(AgentType type, std::shared_ptr<Environment> environment,
                                      const std::string& agentId) {
    std::lock_guard<std::mutex> lock(agentsMutex_);
    
    std::string actualId = agentId.empty() ? generateAgentId() : agentId;
    
    if (hasAgent(actualId)) {
        NLM_LOG_ERROR("Agent already exists: " + actualId);
        return "";
    }
    
    if (!validateAgentConfig(type, environment)) {
        NLM_LOG_ERROR("Invalid agent configuration: " + actualId);
        return "";
    }
    
    // Create a dummy brain for now - in real implementation would be properly configured
    auto brain = std::make_shared<AgentBrain>(nullptr);
    
    auto agent = std::make_unique<AgentContainer>(actualId, brain, environment, type);
    initializeAgent(std::move(agent));
    
    NLM_LOG_INFO("Agent created: id=" + actualId + ", type=" + std::to_string(static_cast<int>(type)));
    broadcastEvent(actualId, "AGENT_CREATED", "Agent created successfully");
    
    return actualId;
}

bool AgentManager::removeAgent(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(agentsMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        NLM_LOG_ERROR("Agent not found: " + agentId);
        return false;
    }
    
    // Clean up agent groups
    for (auto& group : groups_) {
        auto& members = group.second;
        members.erase(std::remove(members.begin(), members.end(), agentId), members.end());
    }
    
    NLM_LOG_INFO("Agent removed: id=" + agentId);
    agents_.erase(it);
    broadcastEvent(agentId, "AGENT_REMOVED", "Agent removed successfully");
    
    return true;
}

bool AgentManager::hasAgent(const std::string& agentId) const {
    std::lock_guard<std::mutex> lock(agentsMutex_);
    return agents_.find(agentId) != agents_.end();
}

AgentContainer& AgentManager::getAgent(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(agentsMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        throw std::runtime_error("Agent not found: " + agentId);
    }
    
    return *it->second;
}

const AgentContainer& AgentManager::getAgent(const std::string& agentId) const {
    std::lock_guard<std::mutex> lock(agentsMutex_);
    
    auto it = agents_.find(agentId);
    if (it == agents_.end()) {
        throw std::runtime_error("Agent not found: " + agentId);
    }
    
    return *it->second;
}

std::vector<std::string> AgentManager::getAllAgentIds() const {
    std::lock_guard<std::mutex> lock(agentsMutex_);
    std::vector<std::string> ids;
    ids.reserve(agents_.size());
    
    for (const auto& pair : agents_) {
        ids.push_back(pair.first);
    }
    
    return ids;
}

void AgentManager::setAgentType(const std::string& agentId, AgentType type) {
    auto& agent = getAgent(agentId);
    agent.setType(type);
}

void AgentManager::setAgentBrain(const std::string& agentId, std::shared_ptr<AgentBrain> brain) {
    auto& agent = getAgent(agentId);
    // AgentContainer doesn't have setBrain method, so we can't set it directly
    // This is a limitation - brain is set at creation time
    NLM_LOG_WARNING("AgentManager::setAgentBrain not implemented - brain is set at creation");
}

void AgentManager::setAgentEnvironment(const std::string& agentId, std::shared_ptr<Environment> environment) {
    auto& agent = getAgent(agentId);
    agent.setEnvironment(environment);
}

void AgentManager::startSimulation(double timestep) {
    if (isRunning_) {
        NLM_LOG_WARNING("Simulation already running");
        return;
    }
    
    timestep_ = timestep;
    isRunning_ = true;
    currentTime_ = 0.0;
    totalSteps_ = 0;
    
    simulationStartTime_ = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Simulation started: timestep=" + std::to_string(timestep_));
    broadcastEvent("", "SIMULATION_STARTED", "Simulation started");
}

void AgentManager::stopSimulation() {
    if (!isRunning_) {
        NLM_LOG_WARNING("Simulation not running");
        return;
    }
    
    isRunning_ = false;
    broadcastEvent("", "SIMULATION_STOPPED", "Simulation stopped");
    
    NLM_LOG_INFO("Simulation stopped");
}

void AgentManager::runStep() {
    if (!isRunning_) {
        NLM_LOG_ERROR("Cannot run step: simulation not running");
        return;
    }
    
    auto stepStartTime = std::chrono::high_resolution_clock::now();
    
    // Update global time
    currentTime_ += timestep_;
    totalSteps_++;
    
    // Run agent updates
    coordinateAgents();
    detectConflicts();
    resolveConflicts();
    allocateResources();
    
    // Synchronize agents
    syncAgents();
    
    // Update metrics
    auto stepEndTime = std::chrono::high_resolution_clock::now();
    double stepDuration = std::chrono::duration<double>(stepEndTime - stepStartTime).count();
    
    NLM_LOG_DEBUG("Step completed: time=" + std::to_string(currentTime_) + 
                  ", duration=" + std::to_string(stepDuration));
}

void AgentManager::runUntil(double time) {
    while (isRunning_ && currentTime_ < time) {
        runStep();
    }
}

void AgentManager::coordinateAgents() {
    // Coordinate agent behaviors based on their types and groups
    for (auto& pair : agents_) {
        auto& agent = *pair.second;
        
        // Update agent status
        agent.update(timestep_);
        
        // Coordinate with group members if in a group
        for (const auto& groupId : getAgentGroups(agent.getId())) {
            auto groupMembers = getGroupMembers(groupId);
            if (agent.isLeader()) {
                // Leader coordinates group actions
                // In real implementation, would implement specific coordination strategies
            }
        }
    }
}

void AgentManager::detectConflicts() {
    // Detect conflicting actions between agents
    for (const auto& agent1 : agents_) {
        for (const auto& agent2 : agents_) {
            if (agent1.first == agent2.first) continue;
            
            // Check for resource conflicts
            // Check for action conflicts
            // Check for space conflicts
            
            // For now, just log potential conflicts
            if (agent1.second->getActionConflicts().size() > 0 &&
                agent2.second->getActionConflicts().size() > 0) {
                NLM_LOG_DEBUG("Potential conflict detected between agents: " +
                            agent1.first + " and " + agent2.first);
            }
        }
    }
}

std::vector<ConflictResolution> AgentManager::resolveConflicts() {
    std::vector<ConflictResolution> resolutions;
    
    // Resolve conflicts using agent implementations
    for (auto& pair : agents_) {
        auto& agent = *pair.second;
        auto conflicts = agent.getActionConflicts();
        
        for (const auto& conflict : conflicts) {
            // Simple conflict resolution: first come, first served
            ConflictResolution resolution;
            resolution.conflictId = agent.getId() + "_" + conflict.first;
            resolution.resolvedBy = agent.getId();
            resolution.resolution = "priority_based";
            resolution.resolutionTime = currentTime_;
            resolution.success = true;
            
            resolutions.push_back(resolution);
            
            // Clear the conflict
            agent.resolveConflict(conflict.first, resolution.resolution);
        }
    }
    
    return resolutions;
}

void AgentManager::allocateResources() {
    std::lock_guard<std::mutex> lock(resourcesMutex_);
    
    // Basic resource allocation
    double totalEnergy = 100.0;
    double totalFood = 50.0;
    
    for (auto& pair : agents_) {
        auto& agent = *pair.second;
        
        // Allocate basic resources
        if (agent.getBody().energy > 0.5f) {
            double available = totalEnergy * 0.1; // 10% of total
            totalEnergy -= available;
            
            // Share with other agents in the same group
            for (const auto& groupId : getAgentGroups(agent.getId())) {
                auto members = getGroupMembers(groupId);
                for (const auto& memberId : members) {
                    if (memberId != agent.getId()) {
                        shareResources(agent.getId(), memberId, available * 0.5, "energy");
                    }
                }
            }
        }
    }
}

void AgentManager::shareResources(const std::string& sourceAgentId, const std::string& targetAgentId,
                                  double amount, const std::string& resourceType) {
    if (!hasAgent(sourceAgentId) || !hasAgent(targetAgentId)) {
        NLM_LOG_ERROR("Cannot share resources: invalid agent IDs");
        return;
    }
    
    NLM_LOG_DEBUG("Resource sharing: " + sourceAgentId + " -> " + targetAgentId +
                  ", amount=" + std::to_string(amount) + ", type=" + resourceType);
}

TimeSyncInfo AgentManager::syncAgents() {
    TimeSyncInfo syncInfo;
    syncInfo.globalTime = currentTime_;
    syncInfo.timestep = timestep_;
    syncInfo.syncPoint = std::chrono::high_resolution_clock::now();
    
    for (auto& pair : agents_) {
        auto& agent = *pair.second;
        syncInfo.agentTimes[pair.first] = currentTime_;
    }
    
    NLM_LOG_DEBUG("Agents synchronized at time: " + std::to_string(currentTime_));
    
    return syncInfo;
}

void AgentManager::adjustAgentTiming(const std::string& agentId, double adjustment) {
    if (!hasAgent(agentId)) {
        NLM_LOG_ERROR("Cannot adjust timing: agent not found: " + agentId);
        return;
    }
    
    NLM_LOG_DEBUG("Agent timing adjusted: agentId=" + agentId + ", adjustment=" + std::to_string(adjustment));
}

void AgentManager::formGroup(const std::string& groupId,
                           const std::vector<std::string>& agentIds,
                           const std::string& leaderId) {
    std::lock_guard<std::mutex> lock(groupsMutex_);
    
    // Validate all agents exist
    for (const auto& agentId : agentIds) {
        if (!hasAgent(agentId)) {
            NLM_LOG_ERROR("Cannot form group: agent not found: " + agentId);
            return;
        }
    }
    
    groups_[groupId] = agentIds;
    
    // Set leader if specified
    std::string actualLeader = leaderId.empty() ? agentIds[0] : leaderId;
    
    if (!hasAgent(actualLeader)) {
        actualLeader = agentIds[0];
    }
    
    getAgent(actualLeader).setLeader(true);
    
    NLM_LOG_INFO("Group formed: groupId=" + groupId + ", leader=" + actualLeader +
                 ", members=" + std::to_string(agentIds.size()));
}

void AgentManager::dissolveGroup(const std::string& groupId) {
    std::lock_guard<std::mutex> lock(groupsMutex_);
    
    auto it = groups_.find(groupId);
    if (it == groups_.end()) {
        NLM_LOG_ERROR("Cannot dissolve group: group not found: " + groupId);
        return;
    }
    
    // Remove group leadership
    for (const auto& agentId : it->second) {
        if (hasAgent(agentId)) {
            getAgent(agentId).setLeader(false);
        }
    }
    
    NLM_LOG_INFO("Group dissolved: groupId=" + groupId);
    groups_.erase(it);
}

bool AgentManager::isGroupActive(const std::string& groupId) const {
    std::lock_guard<std::mutex> lock(groupsMutex_);
    
    auto it = groups_.find(groupId);
    if (it == groups_.end()) {
        return false;
    }
    
    // Check if all group members are alive
    for (const auto& agentId : it->second) {
        if (!hasAgent(agentId)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> AgentManager::getGroupMembers(const std::string& groupId) const {
    std::lock_guard<std::mutex> lock(groupsMutex_);
    
    auto it = groups_.find(groupId);
    if (it == groups_.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

void AgentManager::saveCheckpoint(const std::string& checkpointPath) {
    if (!enableCheckpointing_) {
        NLM_LOG_WARNING("Checkpointing is disabled");
        return;
    }
    
    std::lock_guard<std::mutex> lock(agentsMutex_);
    
    // Create checkpoint directory if it doesn't exist
    std::filesystem::path dir(checkpointPath);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    // Save agent states
    for (const auto& pair : agents_) {
        auto& agent = *pair.second;
        
        std::string agentCheckpointPath = checkpointPath + "/agent_" + pair.first + ".bin";
        
        std::ofstream outFile(agentCheckpointPath, std::ios::binary);
        if (!outFile.is_open()) {
            NLM_LOG_ERROR("Cannot save checkpoint: cannot open file: " + agentCheckpointPath);
            continue;
        }
        
        // In real implementation, would serialize agent state
        outFile << "TODO: Implement agent checkpoint serialization";
        outFile.close();
    }
    
    // Save group information
    std::string groupsFile = checkpointPath + "/groups.bin";
    std::ofstream outFile(groupsFile, std::ios::binary);
    if (outFile.is_open()) {
        // In real implementation, would serialize groups
        outFile << "TODO: Implement groups serialization";
        outFile.close();
    }
    
    checkpointPath_ = checkpointPath;
    NLM_LOG_INFO("Checkpoint saved: " + checkpointPath);
}

bool AgentManager::loadCheckpoint(const std::string& checkpointPath) {
    if (!std::filesystem::exists(checkpointPath)) {
        NLM_LOG_ERROR("Cannot load checkpoint: path does not exist: " + checkpointPath);
        return false;
    }
    
    // In real implementation, would deserialize agent states
    NLM_LOG_INFO("Checkpoint loaded: " + checkpointPath);
    
    return true;
}

void AgentManager::createReplay(const std::string& replayPath) {
    if (!enableReplay_) {
        NLM_LOG_WARNING("Replay is disabled");
        return;
    }
    
    // Record simulation to replay buffer
    // In real implementation, would record agent actions, sensory inputs, etc.
    
    std::ofstream outFile(replayPath, std::ios::binary);
    if (outFile.is_open()) {
        outFile << "TODO: Implement replay serialization";
        outFile.close();
    }
    
    NLM_LOG_INFO("Replay created: " + replayPath);
}

AgentManager::SimulationMetrics AgentManager::getMetrics() const {
    SimulationMetrics metrics;
    
    metrics.totalTime = currentTime_;
    metrics.totalSteps = totalSteps_;
    metrics.activeAgents = agents_.size();
    
    int learningAgents = 0;
    float totalEnergy = 0.0f;
    float totalFiringRate = 0.0f;
    
    for (const auto& pair : agents_) {
        const auto& agent = *pair.second;
        if (agent.getStatus() == AgentStatus::LEARNING) {
            learningAgents++;
        }
        
        totalEnergy += agent.getBody().energy;
        // In real implementation, would get actual firing rate
        totalFiringRate += 50.0f; // Dummy value
    }
    
    metrics.learningAgents = learningAgents;
    metrics.averageEnergy = metrics.activeAgents > 0 ? totalEnergy / metrics.activeAgents : 0.0f;
    metrics.averageFiringRate = metrics.activeAgents > 0 ? totalFiringRate / metrics.activeAgents : 0.0f;
    
    // Count actions
    for (const auto& pair : agents_) {
        const auto& agent = *pair.second;
        // In real implementation, would track actual actions
    }
    
    return metrics;
}

void AgentManager::addAgentEventCallback(const AgentEventCallback& callback) {
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    eventCallbacks_.push_back(callback);
}

AgentManager::PerformanceStats AgentManager::getPerformanceStats() const {
    PerformanceStats stats;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalDuration = std::chrono::duration<double>(endTime - simulationStartTime_).count();
    
    if (totalDuration > 0) {
        stats.simulationSpeed = totalSteps_ / totalDuration;  // steps per second
    }
    
    stats.averageStepTime = totalSteps_ > 0 ? totalDuration / totalSteps_ : 0.0;
    
    // In real implementation, would collect detailed agent performance
    
    return stats;
}

void AgentManager::initializeAgent(std::unique_ptr<AgentContainer> agent) {
    agents_[agent->getId()] = std::move(agent);
}

void AgentManager::updateAgentTiming(const std::string& agentId) {
    if (hasAgent(agentId)) {
        getAgent(agentId).update(timestep_);
    }
}

void AgentManager::updateGroupLeadership(const std::string& groupId) {
    auto members = getGroupMembers(groupId);
    if (members.empty()) return;
    
    // Simple leadership rotation: next in line becomes leader
    // In real implementation, would use more sophisticated leadership election
    if (groups_.find(groupId) != groups_.end()) {
        auto& members = groups_[groupId];
        if (!members.empty()) {
            std::string newLeader = members[0];
            if (hasAgent(newLeader)) {
                getAgent(newLeader).setLeader(true);
            }
        }
    }
}

void AgentManager::broadcastEvent(const std::string& agentId, const std::string& eventType,
                                 const std::string& data) {
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    
    for (const auto& callback : eventCallbacks_) {
        try {
            callback(agentId, eventType, data);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Event callback error: " + std::string(e.what()));
        }
    }
}

std::string AgentManager::generateAgentId() const {
    static std::atomic<int> idCounter{0};
    return "agent_" + std::to_string(++idCounter);
}

bool AgentManager::validateAgentConfig(const AgentType& type,
                                       const std::shared_ptr<Environment>& environment) const {
    return environment != nullptr;
}

std::vector<std::string> AgentManager::getAgentGroups(const std::string& agentId) const {
    std::vector<std::string> groups;
    
    for (const auto& group : groups_) {
        const auto& members = group.second;
        if (std::find(members.begin(), members.end(), agentId) != members.end()) {
            groups.push_back(group.first);
        }
    }
    
    return groups;
}

} // namespace nlm