// Social world with multi-agent interactions
#pragma once

#include "SimpleWorld.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <queue>
#include <mutex>

namespace nlm {

// Social interaction types
enum class SocialInteractionType {
    Cooperation,
    Competition,
    Avoidance,
    Alliance,
    Dominance,
    Submission,
    Grooming,
    Play,
    Teaching,
    Learning
};

// Social rule for agent behavior
struct SocialRule {
    std::string ruleId;
    SocialInteractionType interactionType;
    float threshold;      // Trigger threshold (0.0-1.0)
    float strength;        // Rule strength (0.0-1.0)
    bool isReciprocal;      // Is the rule reciprocal?
    std::string description;
    
    SocialRule() : threshold(0.5f), strength(1.0f), isReciprocal(true) {}
    SocialRule(const std::string& id, SocialInteractionType type, 
              float thresh, float str, bool reciprocal, const std::string& desc)
        : ruleId(id), interactionType(type), threshold(thresh), strength(str),
          isReciprocal(reciprocal), description(desc) {}
};

// Social bond between agents
struct SocialBond {
    uint64_t agentId;        // Agent identifier
    float bondStrength;      // Bond strength (0.0-1.0)
    float trust;             // Trust level (0.0-1.0)
    float reciprocity;      // Reciprocity level (0.0-1.0)
    float lastInteraction;   // Time since last interaction
    std::string bondType;   // Type of bond (friendship, hierarchy, etc.)
    
    SocialBond() : agentId(0), bondStrength(0.0f), trust(0.5f), 
                   reciprocity(0.5f), lastInteraction(0.0f) {}
};

// Social world with multi-agent interactions
class SocialWorld : public SimpleWorld {
public:
    SocialWorld();
    ~SocialWorld();
    
    // Social configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight,
                   float socialRadius, float bondDecayRate);
    
    // Agent management
    void addAgent(uint64_t agentId, float x, float y, float radius = 0.3f);
    void removeAgent(uint64_t agentId);
    AgentBody* getAgent(uint64_t agentId);
    const AgentBody* getAgent(uint64_t agentId) const;
    
    // Social interactions
    void socialInteraction(uint64_t fromAgent, uint64_t toAgent, 
                          SocialInteractionType type, float strength = 1.0f);
    float getSocialBondStrength(uint64_t agentId, uint64_t otherAgentId) const;
    float getSocialTrust(uint64_t agentId, uint64_t otherAgentId) const;
    
    // Social rule management
    void addSocialRule(const SocialRule& rule);
    void removeSocialRule(const std::string& ruleId);
    void applySocialRules(uint64_t agentId);
    
    // Social learning
    void socialLearning(uint64_t learnerAgent, uint64_t observedAgent,
                       float observationQuality = 1.0f);
    void teaching(uint64_t teacherAgent, uint64_t studentAgent,
                 const std::vector<float>& knowledgeTransfer);
    
    // Alliance formation
    void formAlliance(const std::vector<uint64_t>& agentIds);
    void breakAlliance(uint64_t agentId, uint64_t otherAgentId);
    bool isAllied(uint64_t agentId, uint64_t otherAgentId) const;
    
    // Social statistics
    size_t getAgentCount() const { return agents_.size(); }
    float getAverageSocialBond() const;
    float getAverageTrustLevel() const;
    
    // Social simulation
    void updateSocialDynamics(double timestep);
    void processSocialEvents(double timestep);
    
    // Conflict resolution
    void resolveConflict(uint64_t agent1, uint64_t agent2, 
                        SocialInteractionType conflictType);
    void enforceSocialNorms(uint64_t agentId);
    
    // Social memory and culture
    void recordSocialEvent(const std::string& eventType, uint64_t agentId,
                          const std::unordered_map<std::string, float>& eventData);
    void transmitCulture(const std::string& culturalTrait, float strength = 1.0f);
    float getCulturalValue(const std::string& culturalTrait) const;
    
    // Social visualization
    std::string getSocialStateSnapshot() const;
    void visualizeSocialNetwork() const;
    
private:
    // Agent social state
    struct AgentSocialState {
        uint64_t agentId;
        std::unordered_map<uint64_t, SocialBond> bonds;
        float socialEnergy;        // Energy for social interactions
        float socialNeed;          // Need for social interaction (0.0-1.0)
        float stressLevel;         // Stress level (0.0-1.0)
        std::queue<SocialInteractionType> socialQueue;  // Pending social interactions
        float lastSocialAction;    // Time since last social action
        bool isActive;            // Is agent currently active in social interactions?
    };
    
    // Social dynamics
    void updateSocialBonds(double timestep);
    void processSocialQueue(double timestep);
    void applySocialRuleEffects(const SocialRule& rule, uint64_t agentId);
    void handleSocialConflict(uint64_t agent1, uint64_t agent2, 
                            SocialInteractionType conflictType);
    
    // Social learning and teaching
    void distributeKnowledge(uint64_t learnerAgent, uint64_t sourceAgent,
                            const std::vector<float>& knowledge);
    void evolveSocialBehavior(uint64_t agentId, double timestep);
    
    // Cultural transmission
    void transmitCulturalKnowledge(const std::string& trait, float strength);
    void adoptCulturalTrait(uint64_t agentId, const std::string& trait, float strength);
    
    // Visualization helpers
    std::string generateSocialNetworkGraph() const;
    std::string generateBondHeatmap() const;
    
    // Core data
    std::unordered_map<uint64_t, AgentSocialState> agents_;
    std::unordered_map<std::string, SocialRule> socialRules_;
    
    // Social dynamics parameters
    float socialRadius_;
    float bondDecayRate_;
    float socialEnergyDecayRate_;
    float socialNeedGrowthRate_;
    float conflictResolutionRate_;
    
    // Cultural system
    std::unordered_map<std::string, float> culturalValues_;
    std::unordered_map<std::string, std::vector<float>> culturalTrends_;
    double timeSinceCulturalUpdate_;
    
    // Social statistics
    mutable std::mutex socialMutex_;
    mutable double lastSocialStatsUpdate_;
    mutable float avgSocialBond_;
    mutable float avgTrustLevel_;
    
    // Helper functions
    static float calculateBondChange(SocialInteractionType interaction, float strength);
    static float calculateTrustChange(SocialInteractionType interaction, float strength);
    static bool isReciprocalInteraction(SocialInteractionType interaction);
    static std::string getReciprocalInteraction(SocialInteractionType interaction);
    
    // Event recording
    struct SocialEvent {
        double timestamp;
        std::string eventType;
        uint64_t agentId;
        std::unordered_map<std::string, float> data;
        std::string description;
    };
    
    std::vector<SocialEvent> socialEventLog_;
    size_t maxEventLogSize_;
};

} // namespace nlm
