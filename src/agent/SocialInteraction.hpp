#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <queue>
#include <mutex>
#include <chrono>
#include <functional>
#include <unordered_set>

namespace nlm {

// Social interaction types
enum class InteractionType {
    DIRECT_COMMUNICATION,
    INDIRECT_COMMUNICATION,
    OBSERVATION,
    IMITATION,
    COOPERATION,
    COMPETITION,
    CONFLICT_RESOLUTION,
    NEURAL_SHARING,
    MEMORY_SHARING
};

// Communication message structure
struct CommunicationMessage {
    std::string senderId;
    std::string recipientId;
    std::string messageType;
    std::string content;
    double timestamp;
    float priority;
    bool isPrivate;
    
    CommunicationMessage(const std::string& sender, const std::string& recipient,
                         const std::string& type, const std::string& content,
                         float priority = 1.0f, bool private = false)
        : senderId(sender), recipientId(recipient), messageType(type),
          content(content), timestamp(std::chrono::duration<double>
              (std::chrono::high_resolution_clock::now().time_since_epoch()).count()),
          priority(priority), isPrivate(private) {}
};

// Observation record structure
struct Observation {
    std::string observerId;
    std::string observedId;
    double timestamp;
    std::string observationType;
    std::string sensoryData;
    std::string actionData;
    std::string environmentalContext;
    double observationValue;  // For quantitative observations
    
    Observation(const std::string& observer, const std::string& observed,
                const std::string& type, const std::string& sensory,
                const std::string& action, const std::string& context,
                double value = 0.0)
        : observerId(observer), observedId(observed), observationType(type),
          sensoryData(sensory), actionData(action),
          environmentalContext(context), observationValue(value) {
        timestamp = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

// Imitation record structure
struct ImitationRecord {
    std::string imitatorId;
    std::string modelId;
    std::string behaviorType;
    std::string behaviorDetails;
    double imitationStrength;  // 0.0 to 1.0
    double successRate;
    double timestamp;
    
    ImitationRecord(const std::string& imitator, const std::string& model,
                    const std::string& behavior, const std::string& details,
                    double strength = 1.0, double success = 0.0)
        : imitatorId(imitator), modelId(model), behaviorType(behavior),
          behaviorDetails(details), imitationStrength(strength),
          successRate(success) {
        timestamp = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

// Cooperation record structure
struct CooperationRecord {
    std::string agent1Id;
    std::string agent2Id;
    std::string taskType;
    std::string taskDetails;
    double coordinationLevel;  // 0.0 to 1.0
    double outcome;  // Reward or success measure
    double timestamp;
    bool wasSuccessful;
    
    CooperationRecord(const std::string& a1, const std::string& a2,
                      const std::string& task, const std::string& details,
                      double coordination = 0.0, double outcome = 0.0,
                      bool success = false)
        : agent1Id(a1), agent2Id(a2), taskType(task), taskDetails(details),
          coordinationLevel(coordination), outcome(outcome), wasSuccessful(success) {
        timestamp = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

// Competition record structure
struct CompetitionRecord {
    std::string agent1Id;
    std::string agent2Id;
    std::string resourceType;
    double resourceAmount1;
    double resourceAmount2;
    double competitionIntensity;  // 0.0 to 1.0
    double outcome;  // Winner's advantage
    double timestamp;
    std::string winnerId;
    
    CompetitionRecord(const std::string& a1, const std::string& a2,
                      const std::string& resource, double amount1, double amount2,
                      double intensity = 0.0, double outcome = 0.0,
                      const std::string& winner = "")
        : agent1Id(a1), agent2Id(a2), resourceType(resource),
          resourceAmount1(amount1), resourceAmount2(amount2),
          competitionIntensity(intensity), outcome(outcome),
          winnerId(winner) {
        timestamp = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

// SocialInteraction manages all social behaviors between agents
class SocialInteraction {
public:
    SocialInteraction();
    ~SocialInteraction();
    
    // Communication systems
    void sendMessage(const CommunicationMessage& message);
    CommunicationMessage receiveMessage(const std::string& recipientId);
    std::vector<CommunicationMessage> getMessagesForAgent(const std::string& agentId) const;
    void broadcastMessage(const std::string& senderId, const std::string& messageType,
                         const std::string& content, float priority = 1.0f);
    void sendPrivateMessage(const std::string& senderId, const std::string& recipientId,
                           const std::string& messageType, const std::string& content);
    
    // Observation systems
    void recordObservation(const Observation& observation);
    std::vector<Observation> getObservationsForAgent(const std::string& agentId) const;
    std::vector<Observation> getObservationsOfAgent(const std::string& agentId) const;
    std::vector<Observation> getObservationsSince(double sinceTime) const;
    void clearAgentObservations(const std::string& agentId);
    
    // Imitation systems
    void recordImitation(const ImitationRecord& imitation);
    std::vector<ImitationRecord> getImitationsByModel(const std::string& modelId) const;
    std::vector<ImitationRecord> getImitationsByImitator(const std::string& imitatorId) const;
    double calculateImitationStrength(const std::string& imitatorId, const std::string& modelId) const;
    void updateImitationSuccess(const std::string& imitatorId, const std::string& modelId,
                               bool success, double outcome);
    
    // Cooperation systems
    void recordCooperation(const CooperationRecord& cooperation);
    std::vector<CooperationRecord> getCooperationsBetween(const std::string& agent1Id,
                                                        const std::string& agent2Id) const;
    double calculateCooperationLevel(const std::string& agent1Id, const std::string& agent2Id) const;
    std::vector<CooperationRecord> getRecentCooperatives(double sinceTime) const;
    
    // Competition systems
    void recordCompetition(const CompetitionRecord& competition);
    std::vector<CompetitionRecord> getCompetitionsBetween(const std::string& agent1Id,
                                                          const std::string& agent2Id) const;
    std::string determineCompetitionWinner(const std::string& agent1Id,
                                           const std::string& agent2Id) const;
    double calculateCompetitionIntensity(const std::string& agent1Id, const std::string& agent2Id) const;
    
    // Conflict resolution systems
    void registerConflict(const std::string& agent1Id, const std::string& agent2Id,
                         const std::string& conflictType, const std::string& details);
    void resolveConflict(const std::string& agentId, const std::string& resolution);
    std::vector<std::string> getPendingConflicts(const std::string& agentId) const;
    void clearConflicts(const std::string& agentId);
    
    // Neural sharing systems
    void shareNeuralPattern(const std::string& sourceAgentId, const std::string& targetAgentId,
                           const std::vector<float>& neuralPattern, double strength = 1.0);
    std::vector<float> getSharedNeuralPattern(const std::string& sourceAgentId) const;
    double getNeuralSharingStrength(const std::string& sourceAgentId, const std::string& targetAgentId) const;
    
    // Memory sharing systems
    void shareMemory(const std::string& sourceAgentId, const std::string& targetAgentId,
                    const std::string& memoryKey, const std::string& memoryValue,
                    double strength = 1.0);
    std::string getSharedMemory(const std::string& sourceAgentId, const std::string& memoryKey,
                               const std::string& defaultValue = "") const;
    std::unordered_map<std::string, std::string> getAllSharedMemory(const std::string& agentId) const;
    void updateSharedMemory(const std::string& sourceAgentId, const std::string& memoryKey,
                           const std::string& memoryValue, double strength = 1.0);
    
    // Emergent behavior detection
    struct EmergentBehavior {
        std::string behaviorType;
        std::vector<std::string> participatingAgents;
        double emergenceTime;
        double strength;
        std::string description;
        
        EmergentBehavior(const std::string& type, const std::vector<std::string>& agents,
                        double time, double str, const std::string& desc)
            : behaviorType(type), participatingAgents(agents),
              emergenceTime(time), strength(str), description(desc) {}
    };
    
    void detectEmergentBehavior();
    std::vector<EmergentBehavior> getEmergentBehaviors() const;
    void clearEmergentBehaviors();
    
    // Social learning mechanisms
    double calculateSocialLearningPotential(const std::string& agentId) const;
    std::vector<std::string> getEffectiveTeachers(const std::string& agentId) const;
    void processSocialLearning(const std::string& learnerId);
    
    // Social network analysis
    struct SocialNetworkMetrics {
        double networkDensity;
        double averageClusteringCoefficient;
        double averagePathLength;
        std::unordered_map<std::string, int> centralityScores;
        std::vector<std::string> influentialAgents;
    };
    
    SocialNetworkMetrics analyzeSocialNetwork() const;
    
    // Cleanup and maintenance
    void cleanupOldInteractions(double maxAge);
    void reset();
    
    // Statistics and metrics
    struct SocialMetrics {
        int totalMessagesSent;
        int totalObservationsRecorded;
        int totalImitationsRecorded;
        int totalCooperativesRecorded;
        int totalCompetitionsRecorded;
        int totalConflictsResolved;
        double averageImitationSuccess;
        double averageCooperationLevel;
        double averageCompetitionIntensity;
    };
    
    SocialMetrics getSocialMetrics() const;
    
    // Configuration
    void setCommunicationRange(double range) { communicationRange_ = range; }
    void setObservationDecayRate(double rate) { observationDecayRate_ = rate; }
    void setImitationThreshold(double threshold) { imitationThreshold_ = threshold; }
    void setCooperationThreshold(double threshold) { cooperationThreshold_ = threshold; }
    void setCompetitionThreshold(double threshold) { competitionThreshold_ = threshold; }

private:
    // Communication queues
    std::queue<CommunicationMessage> messageQueue_;
    std::mutex messageMutex_;
    std::unordered_map<std::string, std::queue<CommunicationMessage>> agentMessages_;
    std::mutex agentMessagesMutex_;
    
    // Observation storage
    std::vector<Observation> observations_;
    std::mutex observationsMutex_;
    
    // Imitation storage
    std::vector<ImitationRecord> imitations_;
    std::mutex imitationsMutex_;
    
    // Cooperation storage
    std::vector<CooperationRecord> cooperations_;
    std::mutex cooperationsMutex_;
    
    // Competition storage
    std::vector<CompetitionRecord> competitions_;
    std::mutex competitionsMutex_;
    
    // Conflict storage
    std::unordered_map<std::string, std::vector<std::string>> conflicts_;
    std::mutex conflictsMutex_;
    
    // Neural sharing
    std::unordered_map<std::string, std::vector<float>> sharedNeuralPatterns_;
    std::unordered_map<std::string, double> neuralSharingStrengths_;
    std::mutex neuralSharingMutex_;
    
    // Memory sharing
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sharedMemories_;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> memorySharingStrengths_;
    std::mutex memorySharingMutex_;
    
    // Emergent behavior tracking
    std::vector<EmergentBehavior> emergentBehaviors_;
    std::mutex emergentBehaviorsMutex_;
    
    // Configuration parameters
    double communicationRange_;
    double observationDecayRate_;
    double imitationThreshold_;
    double cooperationThreshold_;
    double competitionThreshold_;
    double maxInteractionAge_;
    
    // Statistics
    mutable SocialMetrics metrics_;
};

} // namespace nlm