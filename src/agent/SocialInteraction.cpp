#include "SocialInteraction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

SocialInteraction::SocialInteraction()
    : communicationRange_(10.0), observationDecayRate_(0.9),
      imitationThreshold_(0.5), cooperationThreshold_(0.3),
      competitionThreshold_(0.3), maxInteractionAge_(300.0)
{
    NLM_LOG_INFO("SocialInteraction initialized");
}

SocialInteraction::~SocialInteraction() {
    NLM_LOG_INFO("SocialInteraction destroyed");
}

void SocialInteraction::sendMessage(const CommunicationMessage& message) {
    messageQueue_.push(message);
    
    // Update metrics
    metrics_.totalMessagesSent++;
    
    // Store for the recipient
    std::lock_guard<std::mutex> lock(agentMessagesMutex_);
    agentMessages_[message.recipientId].push(message);
    
    NLM_LOG_DEBUG("Message sent: " + message.senderId + " -> " + message.recipientId +
                  ", type=" + message.messageType + ", content=" + message.content);
}

CommunicationMessage SocialInteraction::receiveMessage(const std::string& recipientId) {
    std::lock_guard<std::mutex> lock(agentMessagesMutex_);
    
    auto it = agentMessages_.find(recipientId);
    if (it == agentMessages_.end() || it->second.empty()) {
        return CommunicationMessage("", "", "NONE", "");
    }
    
    CommunicationMessage msg = it->second.front();
    it->second.pop();
    return msg;
}

std::vector<CommunicationMessage> SocialInteraction::getMessagesForAgent(const std::string& agentId) const {
    std::lock_guard<std::mutex> lock(agentMessagesMutex_);
    
    auto it = agentMessages_.find(agentId);
    if (it == agentMessages_.end()) {
        return std::vector<CommunicationMessage>();
    }
    
    std::vector<CommunicationMessage> messages;
    messages.reserve(it->second.size());
    
    // Create a copy of the queue
    std::queue<CommunicationMessage> temp = it->second;
    
    while (!temp.empty()) {
        messages.push_back(temp.front());
        temp.pop();
    }
    
    return messages;
}

void SocialInteraction::broadcastMessage(const std::string& senderId, const std::string& messageType,
                                        const std::string& content, float priority) {
    // In a real implementation, would find all agents within communication range
    // For now, just log and send to a default agent if exists
    NLM_LOG_DEBUG("Broadcast message: sender=" + senderId + ", type=" + messageType +
                  ", content=" + content + ", priority=" + std::to_string(priority));
}

void SocialInteraction::sendPrivateMessage(const std::string& senderId, const std::string& recipientId,
                                          const std::string& messageType, const std::string& content) {
    CommunicationMessage message(senderId, recipientId, messageType, content);
    sendMessage(message);
}

void SocialInteraction::recordObservation(const Observation& observation) {
    observations_.push_back(observation);
    metrics_.totalObservationsRecorded++;
    
    NLM_LOG_DEBUG("Observation recorded: observer=" + observation.observerId +
                  ", observed=" + observation.observedId +
                  ", type=" + observation.observationType);
}

std::vector<Observation> SocialInteraction::getObservationsForAgent(const std::string& agentId) const {
    std::lock_guard<std::mutex> lock(observationsMutex_);
    
    std::vector<Observation> agentObservations;
    for (const auto& obs : observations_) {
        if (obs.observerId == agentId || obs.observedId == agentId) {
            agentObservations.push_back(obs);
        }
    }
    
    return agentObservations;
}

std::vector<Observation> SocialInteraction::getObservationsOfAgent(const std::string& agentId) const {
    std::lock_guard<std::mutex> lock(observationsMutex_);
    
    std::vector<Observation> agentObservations;
    for (const auto& obs : observations_) {
        if (obs.observedId == agentId) {
            agentObservations.push_back(obs);
        }
    }
    
    // Filter by age (decay)
    double currentTime = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    auto it = std::remove_if(agentObservations.begin(), agentObservations.end(),
        [this, currentTime](const Observation& obs) {
            double age = currentTime - obs.timestamp;
            return age > maxInteractionAge_;
        });
    
    agentObservations.erase(it, agentObservations.end());
    
    return agentObservations;
}

std::vector<Observation> SocialInteraction::getObservationsSince(double sinceTime) const {
    std::lock_guard<std::mutex> lock(observationsMutex_);
    
    std::vector<Observation> recentObservations;
    for (const auto& obs : observations_) {
        if (obs.timestamp >= sinceTime) {
            recentObservations.push_back(obs);
        }
    }
    
    return recentObservations;
}

void SocialInteraction::clearAgentObservations(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(observationsMutex_);
    
    auto it = std::remove_if(observations_.begin(), observations_.end(),
        [&agentId](const Observation& obs) {
            return obs.observerId == agentId || obs.observedId == agentId;
        });
    
    observations_.erase(it, observations_.end());
}

void SocialInteraction::recordImitation(const ImitationRecord& imitation) {
    imitations_.push_back(imitation);
    metrics_.totalImitationsRecorded++;
    
    // Update imitation success rate
    if (imitation.successRate > 0) {
        metrics_.averageImitationSuccess = (metrics_.averageImitationSuccess * 
                                            (metrics_.totalImitationsRecorded - 1) +
                                            imitation.successRate) / 
                                           metrics_.totalImitationsRecorded;
    }
    
    NLM_LOG_DEBUG("Imitation recorded: imitator=" + imitation.imitatorId +
                  ", model=" + imitation.modelId +
                  ", behavior=" + imitation.behaviorType +
                  ", strength=" + std::to_string(imitation.imitationStrength));
}

std::vector<ImitationRecord> SocialInteraction::getImitationsByModel(const std::string& modelId) const {
    std::lock_guard<std::mutex> lock(imitationsMutex_);
    
    std::vector<ImitationRecord> modelImitations;
    for (const auto& imitation : imitations_) {
        if (imitation.modelId == modelId) {
            modelImitations.push_back(imitation);
        }
    }
    
    return modelImitations;
}

std::vector<ImitationRecord> SocialInteraction::getImitationsByImitator(const std::string& imitatorId) const {
    std::lock_guard<std::mutex> lock(imitationsMutex_);
    
    std::vector<ImitationRecord> imitatorImitations;
    for (const auto& imitation : imitations_) {
        if (imitation.imitatorId == imitatorId) {
            imitatorImitations.push_back(imitation);
        }
    }
    
    return imitatorImitations;
}

double SocialInteraction::calculateImitationStrength(const std::string& imitatorId,
                                                     const std::string& modelId) const {
    std::lock_guard<std::mutex> lock(imitationsMutex_);
    
    double totalStrength = 0.0;
    int count = 0;
    
    for (const auto& imitation : imitations_) {
        if (imitation.imitatorId == imitatorId && imitation.modelId == modelId) {
            totalStrength += imitation.imitationStrength;
            count++;
        }
    }
    
    if (count == 0) return 0.0;
    
    // Apply decay based on observation age
    double decayFactor = observationDecayRate_;
    return totalStrength / count * decayFactor;
}

void SocialInteraction::updateImitationSuccess(const std::string& imitatorId, const std::string& modelId,
                                               bool success, double outcome) {
    std::lock_guard<std::mutex> lock(imitationsMutex_);
    
    // Find and update the most recent imitation by this imitator of this model
    for (auto& imitation : imitations_) {
        if (imitation.imitatorId == imitatorId && imitation.modelId == modelId) {
            // Update success rate
            double currentSuccessRate = imitation.successRate;
            imitation.successRate = (currentSuccessRate + (success ? 1.0 : 0.0)) / 2.0;
            
            // Update imitation strength based on success
            if (success) {
                imitation.imitationStrength = std::min(1.0, imitation.imitationStrength * 1.1);
            } else {
                imitation.imitationStrength = std::max(0.0, imitation.imitationStrength * 0.9);
            }
            
            break;
        }
    }
}

void SocialInteraction::recordCooperation(const CooperationRecord& cooperation) {
    cooperations_.push_back(cooperation);
    metrics_.totalCooperativesRecorded++;
    
    // Update cooperation level
    metrics_.averageCooperationLevel = (metrics_.averageCooperationLevel * 
                                        (metrics_.totalCooperativesRecorded - 1) +
                                        cooperation.coordinationLevel) / 
                                       metrics_.totalCooperativesRecorded;
    
    NLM_LOG_DEBUG("Cooperation recorded: agents=" + cooperation.agent1Id + "," + cooperation.agent2Id +
                  ", task=" + cooperation.taskType +
                  ", coordination=" + std::to_string(cooperation.coordinationLevel));
}

std::vector<CooperationRecord> SocialInteraction::getCooperationsBetween(const std::string& agent1Id,
                                                                        const std::string& agent2Id) const {
    std::lock_guard<std::mutex> lock(cooperationsMutex_);
    
    std::vector<CooperationRecord> agentCooperations;
    for (const auto& coop : cooperations_) {
        if ((coop.agent1Id == agent1Id && coop.agent2Id == agent2Id) ||
            (coop.agent1Id == agent2Id && coop.agent2Id == agent1Id)) {
            agentCooperations.push_back(coop);
        }
    }
    
    return agentCooperations;
}

double SocialInteraction::calculateCooperationLevel(const std::string& agent1Id,
                                                   const std::string& agent2Id) const {
    auto cooperations = getCooperationsBetween(agent1Id, agent2Id);
    if (cooperations.empty()) return 0.0;
    
    double totalCoordination = 0.0;
    for (const auto& coop : cooperations) {
        totalCoordination += coop.coordinationLevel;
    }
    
    return totalCoordination / cooperations.size();
}

std::vector<CooperationRecord> SocialInteraction::getRecentCooperatives(double sinceTime) const {
    std::lock_guard<std::mutex> lock(cooperationsMutex_);
    
    std::vector<CooperationRecord> recentCooperatives;
    for (const auto& coop : cooperations_) {
        if (coop.timestamp >= sinceTime) {
            recentCooperatives.push_back(coop);
        }
    }
    
    return recentCooperatives;
}

void SocialInteraction::recordCompetition(const CompetitionRecord& competition) {
    competitions_.push_back(competition);
    metrics_.totalCompetitionsRecorded++;
    
    // Update competition intensity
    metrics_.averageCompetitionIntensity = (metrics_.averageCompetitionIntensity * 
                                            (metrics_.totalCompetitionsRecorded - 1) +
                                            competition.competitionIntensity) / 
                                           metrics_.totalCompetitionsRecorded;
    
    NLM_LOG_DEBUG("Competition recorded: agents=" + competition.agent1Id + "," + competition.agent2Id +
                  ", resource=" + competition.resourceType +
                  ", winner=" + competition.winnerId);
}

std::vector<CompetitionRecord> SocialInteraction::getCompetitionsBetween(const std::string& agent1Id,
                                                                        const std::string& agent2Id) const {
    std::lock_guard<std::mutex> lock(competitionsMutex_);
    
    std::vector<CompetitionRecord> agentCompetitions;
    for (const auto& comp : competitions_) {
        if ((comp.agent1Id == agent1Id && comp.agent2Id == agent2Id) ||
            (comp.agent1Id == agent2Id && comp.agent2Id == agent1Id)) {
            agentCompetitions.push_back(comp);
        }
    }
    
    return agentCompetitions;
}

std::string SocialInteraction::determineCompetitionWinner(const std::string& agent1Id,
                                                         const std::string& agent2Id) const {
    // Analyze recent competitions to determine winner
    auto competitions = getCompetitionsBetween(agent1Id, agent2Id);
    if (competitions.empty()) return "";
    
    std::string currentWinner = "";
    double bestOutcome = -1.0;
    
    for (const auto& comp : competitions) {
        if (!comp.winnerId.empty()) {
            double outcome = comp.outcome;
            if (outcome > bestOutcome) {
                bestOutcome = outcome;
                currentWinner = comp.winnerId;
            }
        }
    }
    
    return currentWinner;
}

double SocialInteraction::calculateCompetitionIntensity(const std::string& agent1Id,
                                                       const std::string& agent2Id) const {
    auto competitions = getCompetitionsBetween(agent1Id, agent2Id);
    if (competitions.empty()) return 0.0;
    
    double totalIntensity = 0.0;
    for (const auto& comp : competitions) {
        totalIntensity += comp.competitionIntensity;
    }
    
    return totalIntensity / competitions.size();
}

void SocialInteraction::registerConflict(const std::string& agent1Id, const std::string& agent2Id,
                                        const std::string& conflictType, const std::string& details) {
    std::lock_guard<std::mutex> lock(conflictsMutex_);
    
    // Create conflict identifier
    std::string conflictId = agent1Id + "_vs_" + agent2Id + "_" + std::to_string(std::chrono::
        duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    
    conflicts_[agent1Id].push_back(conflictId);
    conflicts_[agent2Id].push_back(conflictId);
    
    NLM_LOG_DEBUG("Conflict registered: agents=" + agent1Id + "," + agent2Id +
                  ", type=" + conflictType + ", details=" + details);
}

void SocialInteraction::resolveConflict(const std::string& agentId, const std::string& resolution) {
    std::lock_guard<std::mutex> lock(conflictsMutex_);
    
    auto it = conflicts_.find(agentId);
    if (it != conflicts_.end() && !it->second.empty()) {
        // Clear resolved conflicts
        it->second.clear();
    }
    
    NLM_LOG_DEBUG("Conflict resolved: agentId=" + agentId + ", resolution=" + resolution);
}

std::vector<std::string> SocialInteraction::getPendingConflicts(const std::string& agentId) const {
    std::lock_guard<std::mutex> lock(conflictsMutex_);
    
    auto it = conflicts_.find(agentId);
    if (it == conflicts_.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

void SocialInteraction::clearConflicts(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(conflictsMutex_);
    
    conflicts_.erase(agentId);
}

void SocialInteraction::shareNeuralPattern(const std::string& sourceAgentId, const std::string& targetAgentId,
                                          const std::vector<float>& neuralPattern, double strength) {
    if (sourceAgentId == targetAgentId) return;
    
    std::lock_guard<std::mutex> lock(neuralSharingMutex_);
    
    // Store the neural pattern for the target agent
    sharedNeuralPatterns_[targetAgentId] = neuralPattern;
    neuralSharingStrengths_[targetAgentId] = strength;
    
    // Update metrics
    metrics_.totalMessagesSent++;
    
    NLM_LOG_DEBUG("Neural pattern shared: source=" + sourceAgentId + ", target=" + targetAgentId +
                  ", pattern size=" + std::to_string(neuralPattern.size()) +
                  ", strength=" + std::to_string(strength));
}

std::vector<float> SocialInteraction::getSharedNeuralPattern(const std::string& sourceAgentId) const {
    std::lock_guard<std::mutex> lock(neuralSharingMutex_);
    
    auto it = sharedNeuralPatterns_.find(sourceAgentId);
    if (it != sharedNeuralPatterns_.end()) {
        return it->second;
    }
    
    return std::vector<float>();
}

double SocialInteraction::getNeuralSharingStrength(const std::string& sourceAgentId,
                                                   const std::string& targetAgentId) const {
    std::lock_guard<std::mutex> lock(neuralSharingMutex_);
    
    // In real implementation, would check if target has received from source
    // For now, return a dummy value
    return 0.8; // Default sharing strength
}

void SocialInteraction::shareMemory(const std::string& sourceAgentId, const std::string& targetAgentId,
                                    const std::string& memoryKey, const std::string& memoryValue,
                                    double strength) {
    if (sourceAgentId == targetAgentId) return;
    
    std::lock_guard<std::mutex> lock(memorySharingMutex_);
    
    sharedMemories_[targetAgentId][memoryKey] = memoryValue;
    memorySharingStrengths_[targetAgentId][memoryKey] = strength;
    
    NLM_LOG_DEBUG("Memory shared: source=" + sourceAgentId + ", target=" + targetAgentId +
                  ", key=" + memoryKey + ", value=" + memoryValue);
}

std::string SocialInteraction::getSharedMemory(const std::string& sourceAgentId,
                                               const std::string& memoryKey,
                                               const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(memorySharingMutex_);
    
    // Check if source has memory to share with someone
    auto sourceMemIt = sharedMemories_.find(sourceAgentId);
    if (sourceMemIt != sharedMemories_.end()) {
        auto keyIt = sourceMemIt->second.find(memoryKey);
        if (keyIt != sourceMemIt->second.end()) {
            return keyIt->second;
        }
    }
    
    return defaultValue;
}

std::unordered_map<std::string, std::string> SocialInteraction::getAllSharedMemory(const std::string& agentId) const {
    std::lock_guard<std::mutex> lock(memorySharingMutex_);
    
    auto it = sharedMemories_.find(agentId);
    if (it != sharedMemories_.end()) {
        return it->second;
    }
    
    return std::unordered_map<std::string, std::string>();
}

void SocialInteraction::updateSharedMemory(const std::string& sourceAgentId,
                                           const std::string& memoryKey,
                                           const std::string& memoryValue,
                                           double strength) {
    std::lock_guard<std::mutex> lock(memorySharingMutex_);
    
    sharedMemories_[sourceAgentId][memoryKey] = memoryValue;
    memorySharingStrengths_[sourceAgentId][memoryKey] = strength;
}

void SocialInteraction::detectEmergentBehavior() {
    std::lock_guard<std::mutex> lock(emergentBehaviorsMutex_);
    
    // Analyze agent behaviors to detect emergent patterns
    // In real implementation, would use complex pattern analysis
    
    // Simple example: detect if many agents are forming groups
    // This would require access to agent manager
    
    // For now, just log the detection attempt
    NLM_LOG_DEBUG("Emergent behavior detection running");
}

std::vector<SocialInteraction::EmergentBehavior> SocialInteraction::getEmergentBehaviors() const {
    std::lock_guard<std::mutex> lock(emergentBehaviorsMutex_);
    return emergentBehaviors_;
}

void SocialInteraction::clearEmergentBehaviors() {
    std::lock_guard<std::mutex> lock(emergentBehaviorsMutex_);
    emergentBehaviors_.clear();
}

double SocialInteraction::calculateSocialLearningPotential(const std::string& agentId) const {
    double potential = 0.0;
    
    // Calculate based on:
    // 1. Number of observations recorded
    // 2. Number of social interactions
    // 3. Level of curiosity
    
    auto observations = getObservationsForAgent(agentId);
    potential += observations.size() * 0.1;
    
    auto messages = getMessagesForAgent(agentId);
    potential += messages.size() * 0.05;
    
    // Would add more complex calculations in real implementation
    
    return std::min(1.0, potential);
}

std::vector<std::string> SocialInteraction::getEffectiveTeachers(const std::string& agentId) const {
    std::vector<std::string> teachers;
    
    // Find agents who have been successfully imitated
    auto imitations = getImitationsByImitator(agentId);
    for (const auto& imitation : imitations_) {
        if (imitation.successRate > 0.7) { // 70% success threshold
            teachers.push_back(imitation.modelId);
        }
    }
    
    return teachers;
}

void SocialInteraction::processSocialLearning(const std::string& learnerId) {
    auto teachers = getEffectiveTeachers(learnerId);
    if (teachers.empty()) return;
    
    // Select the best teacher
    std::string bestTeacher = teachers[0];
    double bestStrength = calculateImitationStrength(learnerId, bestTeacher);
    
    for (const auto& teacher : teachers) {
        double strength = calculateImitationStrength(learnerId, teacher);
        if (strength > bestStrength) {
            bestStrength = strength;
            bestTeacher = teacher;
        }
    }
    
    if (bestStrength > imitationThreshold_) {
        // Process social learning
        // In real implementation, would apply teacher's neural patterns/behaviors
        NLM_LOG_DEBUG("Social learning processed: learner=" + learnerId +
                      ", teacher=" + bestTeacher +
                      ", strength=" + std::to_string(bestStrength));
    }
}

SocialInteraction::SocialNetworkMetrics SocialInteraction::analyzeSocialNetwork() const {
    SocialNetworkMetrics metrics;
    
    // Simple network analysis - would be more complex in real implementation
    metrics.networkDensity = 0.3; // Dummy value
    metrics.averageClusteringCoefficient = 0.4;
    metrics.averagePathLength = 2.5;
    
    // Find influential agents (those with most imitations or observations)
    std::unordered_map<std::string, int> influenceScores;
    
    // This would require access to all agents and their interactions
    
    for (auto& pair : influenceScores) {
        metrics.centralityScores[pair.first] = pair.second;
    }
    
    // Determine top influencers
    std::vector<std::pair<std::string, int>> sortedInfluences(
        metrics.centralityScores.begin(), metrics.centralityScores.end());
    std::sort(sortedInfluences.begin(), sortedInfluences.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(size_t(5), sortedInfluences.size()); ++i) {
        metrics.influentialAgents.push_back(sortedInfluences[i].first);
    }
    
    return metrics;
}

void SocialInteraction::cleanupOldInteractions(double maxAge) {
    double currentTime = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Cleanup old observations
    auto obsIt = std::remove_if(observations_.begin(), observations_.end(),
        [this, currentTime](const Observation& obs) {
            return (currentTime - obs.timestamp) > maxAge;
        });
    observations_.erase(obsIt, observations_.end());
    
    // Cleanup old imitations
    auto impIt = std::remove_if(imitations_.begin(), imitations_.end(),
        [currentTime](const ImitationRecord& imp) {
            return (currentTime - imp.timestamp) > maxAge;
        });
    imitations_.erase(impIt, imitations_.end());
    
    // Cleanup old cooperations
    auto coopIt = std::remove_if(cooperations_.begin(), cooperations_.end(),
        [currentTime](const CooperationRecord& coop) {
            return (currentTime - coop.timestamp) > maxAge;
        });
    cooperations_.erase(coopIt, cooperations_.end());
    
    // Cleanup old competitions
    auto compIt = std::remove_if(competitions_.begin(), competitions_.end(),
        [currentTime](const CompetitionRecord& comp) {
            return (currentTime - comp.timestamp) > maxAge;
        });
    competitions_.erase(compIt, competitions_.end());
}

void SocialInteraction::reset() {
    // Clear all data structures
    {
        std::lock_guard<std::mutex> lock(messageMutex_);
        while (!messageQueue_.empty()) messageQueue_.pop();
    }
    
    {
        std::lock_guard<std::mutex> lock(agentMessagesMutex_);
        agentMessages_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(observationsMutex_);
        observations_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(imitationsMutex_);
        imitations_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(cooperationsMutex_);
        cooperations_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(competitionsMutex_);
        competitions_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(conflictsMutex_);
        conflicts_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(neuralSharingMutex_);
        sharedNeuralPatterns_.clear();
        neuralSharingStrengths_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(memorySharingMutex_);
        sharedMemories_.clear();
        memorySharingStrengths_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(emergentBehaviorsMutex_);
        emergentBehaviors_.clear();
    }
    
    // Reset metrics
    metrics_ = SocialMetrics();
    
    NLM_LOG_INFO("SocialInteraction reset");
}

SocialInteraction::SocialMetrics SocialInteraction::getSocialMetrics() const {
    return metrics_;
}

} // namespace nlm