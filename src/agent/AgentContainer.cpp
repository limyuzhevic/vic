#include "AgentContainer.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <sstream>

namespace nlm {

AgentContainer::AgentContainer(std::string id, std::shared_ptr<AgentBrain> brain,
                               std::shared_ptr<Environment> environment,
                               AgentType type)
    : id_(std::move(id)), type_(type), status_(AgentStatus::ACTIVE),
      brain_(std::move(brain)), environment_(std::move(environment)),
      leaderId_("")
{
    if (!brain_) {
        NLM_LOG_ERROR("Cannot create AgentContainer: null brain pointer");
        return;
    }
    
    body_ = AgentBody();  // Initialize body
    lastUpdateTime_ = 0.0;
    lastSenseTime_ = 0.0;
    lastThinkTime_ = 0.0;
    lastActTime_ = 0.0;
    lastLearnTime_ = 0.0;
    
    NLM_LOG_INFO("AgentContainer created: id=" + id_ + ", type=" + std::to_string(static_cast<int>(type_)));
}

AgentContainer::~AgentContainer() {
    NLM_LOG_INFO("AgentContainer destroyed: id=" + id_);
}

void AgentContainer::update(double timestep) {
    double currentTime = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if (lastUpdateTime_ > 0) {
        double dt = currentTime - lastUpdateTime_;
        if (dt > 0.1) dt = 0.1; // Cap timestep
        
        if (status_ == AgentStatus::ACTIVE) {
            // Update developmental stage
            brain_->updateDevelopment(dt);
            
            // Sense environment
            sense();
            
            // Process sensory input
            if (brain_) {
                // Get sensory percept from environment
                const auto& percept = environment_->getState();
                // Note: In real implementation, would get proper sensory percept
                // For now, we'll simulate
                brain_->processSensoryInput(SensoryPercept());
                
                // Apply neuromodulation based on environment state
                float reward = environment_->getLastReward();
                float predictedReward = 0.0f; // Would normally be predicted
                brain_->applyRewardModulation(reward, predictedReward);
            }
            
            // Learn from experience
            learn();
            
            // Decide on action
            think();
            
            // Execute action if appropriate
            act();
        }
    }
    
    lastUpdateTime_ = currentTime;
}

void AgentContainer::reset() {
    if (brain_) {
        brain_->reset();
    }
    body_.reset();
    
    // Clear social state
    groups_.clear();
    leaderId_ = "";
    isLeader_ = false;
    
    // Clear messages and observations
    {
        std::lock_guard<std::mutex> lock(messageMutex_);
        while (!messages_.empty()) messages_.pop();
    }
    
    {
        std::lock_guard<std::mutex> lock(observationMutex_);
        observations_.clear();
    }
    
    status_ = AgentStatus::ACTIVE;
    lastUpdateTime_ = 0.0;
    
    NLM_LOG_INFO("AgentContainer reset: id=" + id_);
}

void AgentContainer::sense() {
    if (!environment_) return;
    
    // Get sensory input from environment
    // In real implementation, this would call environment->getSensoryInput()
    // For now, we'll simulate
    
    double currentTime = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Update sense timing
    if (lastSenseTime_ > 0) {
        double dt = currentTime - lastSenseTime_;
    }
    
    lastSenseTime_ = currentTime;
}

void AgentContainer::think() {
    if (!brain_) return;
    
    double currentTime = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if (lastThinkTime_ > 0) {
        double dt = currentTime - lastThinkTime_;
        if (dt > 0.1) dt = 0.1;
    }
    
    lastThinkTime_ = currentTime;
}

void AgentContainer::act() {
    if (!environment_ || !brain_) return;
    
    double currentTime = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Decode motor command from brain
    MotorCommand command = brain_->decodeMotorCommand();
    
    // Apply action to environment
    // In real implementation, this would call environment->applyAction(command)
    
    // Update agent body
    switch (command) {
        case MotorCommand::MoveForward:
            body_.velocityX += 0.1f;
            body_.velocityY += 0.0f;
            body_.isMoving = true;
            break;
        case MotorCommand::MoveBackward:
            body_.velocityX -= 0.1f;
            body_.velocityY -= 0.0f;
            body_.isMoving = true;
            break;
        case MotorCommand::TurnLeft:
            body_.angularVelocity -= 0.05f;
            body_.isTurning = true;
            break;
        case MotorCommand::TurnRight:
            body_.angularVelocity += 0.05f;
            body_.isTurning = true;
            break;
        case MotorCommand::Wait:
            body_.isMoving = false;
            body_.isTurning = false;
            break;
        default:
            break;
    }
    
    body_.lastActionTime = currentTime;
    lastActTime_ = currentTime;
}

void AgentContainer::learn() {
    if (!brain_) return;
    
    double currentTime = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if (lastLearnTime_ > 0) {
        double dt = currentTime - lastLearnTime_;
        if (dt > 0.1) dt = 0.1;
        
        // Development continues during learning
        brain_->updateDevelopment(dt);
    }
    
    lastLearnTime_ = currentTime;
}

void AgentContainer::communicate(const std::string& message, const std::string& targetId) {
    // Queue message for delivery
    std::lock_guard<std::mutex> lock(messageMutex_);
    messages_.push({id_, targetId, "GENERAL", message, 1.0f, true});
}

void AgentContainer::observe(const std::string& agentId) {
    // Record observation
    Observation obs(id_, agentId, "DIRECT_OBSERVATION", "", "", "", 1.0);
    
    std::lock_guard<std::mutex> lock(observationMutex_);
    observations_.push_back(obs);
}

void AgentContainer::imitate(const std::string& agentId, const std::string& behavior) {
    // Create imitation record
    ImitationRecord imitation(id_, agentId, behavior, "", 1.0, 0.0);
    
    // In real implementation, would apply to brain
    // For now, just record
}

void AgentContainer::shareNeuralPatterns(const std::string& targetId) {
    // Would extract neural patterns from brain and share with target
    std::vector<float> weights;
    if (brain_) {
        // Get neural weights from brain regions
        for (const auto& region : brain_->getBrain()->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto& neuron : pop->getNeurons()) {
                    for (const auto& synapse : neuron->getSynapses()) {
                        weights.push_back(synapse->getWeight());
                    }
                }
            }
        }
    }
    
    addSharedWeights(targetId, weights);
}

void AgentContainer::shareMemory(const std::string& targetId) {
    // Share learned concepts and knowledge
    // In real implementation, would extract from brain memory systems
}

void AgentContainer::formGroup(const std::string& groupId) {
    // Create or join group
    groups_[groupId].push_back(id_);
    
    // If this is the first member, make them leader
    if (groups_[groupId].size() == 1) {
        setLeader(true);
        leaderId_ = id_;
    }
}

void AgentContainer::leaveGroup(const std::string& groupId) {
    auto it = groups_.find(groupId);
    if (it != groups_.end()) {
        auto& members = it->second;
        members.erase(std::remove(members.begin(), members.end(), id_), members.end());
        
        if (members.empty()) {
            groups_.erase(it);
        }
        
        // If we're leaving and we were leader, promote next member
        if (isLeader_ && id_ == leaderId_) {
            setLeader(false);
            if (!members.empty()) {
                leaderId_ = members[0];
                // In real implementation, would notify new leader
            }
        }
    }
}

bool AgentContainer::isInGroup(const std::string& groupId) const {
    auto it = groups_.find(groupId);
    return it != groups_.end() && 
           std::find(it->second.begin(), it->second.end(), id_) != it->second.end();
}

std::vector<std::string> AgentContainer::getGroupMembers() const {
    std::vector<std::string> allMembers;
    for (const auto& group : groups_) {
        allMembers.insert(allMembers.end(), group.second.begin(), group.second.end());
    }
    // Remove duplicates
    std::sort(allMembers.begin(), allMembers.end());
    allMembers.erase(std::unique(allMembers.begin(), allMembers.end()), allMembers.end());
    return allMembers;
}

void AgentContainer::addGroupMember(const std::string& agentId) {
    // This would be called by group leader
    // For now, just log
    NLM_LOG_INFO("Group member added: agentId=" + agentId + " to container=" + id_);
}

void AgentContainer::removeGroupMember(const std::string& agentId) {
    // This would be called by group leader
    for (auto& group : groups_) {
        auto& members = group.second;
        members.erase(std::remove(members.begin(), members.end(), agentId), members.end());
    }
}

void AgentContainer::receiveMessage(const std::string& fromId, const std::string& message) {
    std::lock_guard<std::mutex> lock(messageMutex_);
    messages_.push({fromId, id_, "RECEIVED", message, 1.0f, false});
}

std::vector<std::pair<std::string, std::string>> AgentContainer::getMessages() const {
    std::lock_guard<std::mutex> lock(messageMutex_);
    std::vector<std::pair<std::string, std::string>> result;
    
    // Create a copy to avoid holding lock during processing
    std::queue<std::pair<std::string, std::string>> temp = messages_;
    
    while (!temp.empty()) {
        auto msg = temp.front();
        temp.pop();
        result.push_back(msg);
    }
    
    return result;
}

void AgentContainer::clearMessages() {
    std::lock_guard<std::mutex> lock(messageMutex_);
    while (!messages_.empty()) messages_.pop();
}

std::vector<AgentContainer::ObservationRecord> AgentContainer::getObservations() const {
    std::lock_guard<std::mutex> lock(observationMutex_);
    return observations_;
}

void AgentContainer::clearObservations() {
    std::lock_guard<std::mutex> lock(observationMutex_);
    observations_.clear();
}

void AgentContainer::addSharedWeights(const std::string& sourceId, const std::vector<float>& weights) {
    std::lock_guard<std::mutex> lock(weightsMutex_);
    sharedNeuralWeights_[sourceId] = weights;
}

std::vector<float> AgentContainer::getSharedWeights(const std::string& sourceId) const {
    std::lock_guard<std::mutex> lock(weightsMutex_);
    auto it = sharedNeuralWeights_.find(sourceId);
    if (it != sharedNeuralWeights_.end()) {
        return it->second;
    }
    return std::vector<float>();
}

void AgentContainer::addSharedMemory(const std::string& sourceId, const std::string& memoryKey,
                                     const std::string& memoryValue) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    sharedMemory_[sourceId][memoryKey] = memoryValue;
}

std::string AgentContainer::getSharedMemory(const std::string& sourceId,
                                            const std::string& memoryKey) const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    auto sourceIt = sharedMemory_.find(sourceId);
    if (sourceIt != sharedMemory_.end()) {
        auto keyIt = sourceIt->second.find(memoryKey);
        if (keyIt != sourceIt->second.end()) {
            return keyIt->second;
        }
    }
    return "";
}

void AgentContainer::registerActionConflict(const std::string& conflictingAgentId,
                                           const std::string& action) {
    actionConflicts_.push_back({conflictingAgentId, action});
}

std::vector<std::pair<std::string, std::string>> AgentContainer::getActionConflicts() const {
    return actionConflicts_;
}

void AgentContainer::resolveConflict(const std::string& conflictingAgentId,
                                     const std::string& resolution) {
    // Find and remove the conflict
    auto it = std::remove_if(actionConflicts_.begin(), actionConflicts_.end(),
        [&conflictingAgentId](const auto& conflict) {
            return conflict.first == conflictingAgentId;
        });
    actionConflicts_.erase(it, actionConflicts_.end());
}

void AgentContainer::recordBehavior(const std::string& behaviorType, double timestamp,
                                    const std::string& context) {
    behaviorHistory_.push_back({behaviorType, timestamp, context});
}

std::vector<std::tuple<std::string, double, std::string>> AgentContainer::getRecordedBehaviors() const {
    return behaviorHistory_;
}

} // namespace nlm