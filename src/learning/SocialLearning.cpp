#include "SocialLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <numeric>

namespace nlm {

// Imitation Learning Implementation
class ImitationLearner {
public:
    struct ImitationParameters {
        float observationThreshold;    // Minimum observation count to imitate
        float similarityThreshold;     // Minimum similarity to copy behavior
        float rewardPredictionThreshold; // Minimum predicted reward to imitate
        float learningRate;            // Rate of imitation learning
    };
    
    ImitationLearner() {
        params.observationThreshold = 3.0f;
        params.similarityThreshold = 0.7f;
        params.rewardPredictionThreshold = 0.5f;
        params.learningRate = 0.1f;
    }
    
    float computeActionSimilarity(const std::vector<float>& state1,
                                 const std::vector<float>& state2) const {
        if (state1.size() != state2.size() || state1.empty()) return 0.0f;
        
        float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
        for (size_t i = 0; i < state1.size(); ++i) {
            dot += state1[i] * state2[i];
            norm1 += state1[i] * state1[i];
            norm2 += state2[i] * state2[i];
        }
        
        if (norm1 < 0.0001f || norm2 < 0.0001f) return 0.0f;
        return dot / (std::sqrt(norm1) * std::sqrt(norm2));
    }
    
    bool canImitate(const std::string& agentID, const std::vector<float>& currentState,
                    float predictedReward) const {
        // Check if we have enough observations
        auto it = observationCounts_.find(agentID);
        if (it == observationCounts_.end() || it->second < params.observationThreshold)
            return false;
        
        // Check predicted reward
        if (predictedReward < params.rewardPredictionThreshold)
            return false;
        
        return true;
    }
    
    ActionType selectImitationAction(const std::string& agentID,
                                    const std::vector<float>& currentState) {
        ActionType bestAction = ActionType::Wait;
        float bestSimilarity = 0.0f;
        
        auto it = agentBehaviors_.find(agentID);
        if (it == agentBehaviors_.end()) return bestAction;
        
        for (const auto& behavior : it->second) {
            float similarity = computeActionSimilarity(currentState, behavior.state);
            if (similarity > bestSimilarity && similarity > params.similarityThreshold) {
                bestSimilarity = similarity;
                bestAction = behavior.action;
            }
        }
        
        return bestAction;
    }
    
    void recordObservedBehavior(const std::string& agentID, ActionType action,
                               const std::vector<float>& state,
                               const std::vector<float>& nextState,
                               float reward) {
        BehaviorRecord record;
        record.action = action;
        record.state = state;
        record.nextState = nextState;
        record.predictedReward = reward;
        record.observationCount = 1;
        
        // Update existing record or create new one
        auto it = agentBehaviors_.find(agentID);
        if (it == agentBehaviors_.end()) {
            std::vector<BehaviorRecord> behaviors;
            behaviors.push_back(record);
            agentBehaviors_[agentID] = behaviors;
            observationCounts_[agentID] = 1;
        } else {
            bool found = false;
            for (auto& existing : it->second) {
                if (existing.action == action && 
                    computeActionSimilarity(existing.state, state) > 0.8f) {
                    existing.observationCount++;
                    existing.predictedReward = 
                        (existing.predictedReward * (existing.observationCount - 1) + reward) / 
                        existing.observationCount;
                    found = true;
                    break;
                }
            }
            if (!found) {
                it->second.push_back(record);
                observationCounts_[agentID]++;
            }
        }
    }
    
    void setParameters(const ImitationParameters& params) {
        this->params = params;
    }
    
    void reset() {
        agentBehaviors_.clear();
        observationCounts_.clear();
    }
    
private:
    ImitationParameters params;
    
    struct BehaviorRecord {
        ActionType action;
        std::vector<float> state;
        std::vector<float> nextState;
        float predictedReward;
        int observationCount;
    };
    
    std::unordered_map<std::string, std::vector<BehaviorRecord>> agentBehaviors_;
    std::unordered_map<std::string, int> observationCounts_;
};

// Cultural Transmission Implementation
class CulturalTransmission {
public:
    struct TransmissionParameters {
        float transmissionRate;       // Rate of cultural transmission
        float innovationRate;         // Rate of new behavior emergence
        float conformityPressure;     // Pressure to conform to group behavior
        float prestigeWeight;         // Weight of prestige in transmission
    };
    
    CulturalTransmission() {
        params.transmissionRate = 0.05f;
        params.innovationRate = 0.01f;
        params.conformityPressure = 0.8f;
        params.prestigeWeight = 0.5f;
    }
    
    void updateCulture(const std::string& agentID, const std::vector<float>& culturalTraits) {
        // Update agent's cultural traits based on population culture
        std::vector<float>& traits = individualTraits_[agentID];
        
        // Mix with population culture
        for (size_t i = 0; i < traits.size(); ++i) {
            float populationTrait = 0.0f;
            size_t count = 0;
            
            for (const auto& pair : populationTraits_) {
                if (i < pair.second.size()) {
                    populationTrait += pair.second[i];
                    count++;
                }
            }
            
            if (count > 0) {
                populationTrait /= count;
                traits[i] = traits[i] * (1.0f - params.transmissionRate) + 
                          populationTrait * params.transmissionRate;
            }
        }
        
        // Apply innovation
        if (std::rand() % 100 < (int)(params.innovationRate * 100)) {
            int traitIndex = std::rand() % traits.size();
            traits[traitIndex] = (traits[traitIndex] + 
                                 (std::rand() % 100) / 100.0f * 2.0f - 1.0f) * 0.5f;
            traits[traitIndex] = std::clamp(traits[traitIndex], 0.0f, 1.0f);
        }
    }
    
    float computePrestige(const std::string& agentID) const {
        auto it = prestigeScores_.find(agentID);
        if (it == prestigeScores_.end()) return 0.0f;
        return it->second;
    }
    
    void updatePrestige(const std::string& agentID, float success, float interactionCount) {
        float prestigeUpdate = success * params.prestigeWeight / (interactionCount + 1.0f);
        prestigeScores_[agentID] += prestigeUpdate;
        prestigeScores_[agentID] = std::clamp(prestigeScores_[agentID], 0.0f, 100.0f);
    }
    
    std::vector<float> getCulturalTraits(const std::string& agentID) const {
        auto it = individualTraits_.find(agentID);
        if (it == individualTraits_.end()) {
            std::vector<float> traits(10, 0.5f); // Default traits
            return traits;
        }
        return it->second;
    }
    
    void setParameters(const TransmissionParameters& params) {
        this->params = params;
    }
    
    void reset() {
        individualTraits_.clear();
        prestigeScores_.clear();
        populationTraits_.clear();
    }
    
private:
    TransmissionParameters params;
    
    std::unordered_map<std::string, std::vector<float>> individualTraits_;
    std::unordered_map<std::string, float> prestigeScores_;
    std::unordered_map<std::string, std::vector<float>> populationTraits_;
};

// Peer Influence Implementation
class PeerInfluence {
public:
    struct InfluenceParameters {
        float influenceRadius;         // Spatial influence radius
        float directInfluence;         // Direct influence strength
        float indirectInfluence;       // Indirect influence through peers
        float socialReinforcement;     // Social reinforcement effect
    };
    
    PeerInfluence() {
        params.influenceRadius = 5.0f;
        params.directInfluence = 0.3f;
        params.indirectInfluence = 0.1f;
        params.socialReinforcement = 0.2f;
    }
    
    void updateSocialInfluence(const std::string& agentID,
                             const std::vector<std::pair<std::string, float>>& peerActions) {
        float totalInfluence = 0.0f;
        
        for (const auto& peerPair : peerActions) {
            const std::string& peerID = peerPair.first;
            float peerInfluence = peerPair.second;
            
            // Get peer behavior
            auto it = peerBehaviors_.find(peerID);
            if (it != peerBehaviors_.end()) {
                // Compute influence based on peer similarity
                float influence = params.directInfluence * peerInfluence;
                
                // Add indirect influence through shared experiences
                for (const auto& shared : sharedExperiences_[agentID]) {
                    if (shared.peerID == peerID) {
                        influence += params.indirectInfluence * shared.influenceStrength;
                    }
                }
                
                totalInfluence += influence;
            }
        }
        
        // Update agent's behavior based on social influence
        if (totalInfluence > 0.0f) {
            socialInfluence_[agentID] += totalInfluence * 0.01f;
            socialInfluence_[agentID] = std::clamp(socialInfluence_[agentID], 0.0f, 1.0f);
        }
    }
    
    float getSocialInfluence(const std::string& agentID) const {
        auto it = socialInfluence_.find(agentID);
        if (it == socialInfluence_.end()) return 0.0f;
        return it->second;
    }
    
    void recordPeerBehavior(const std::string& agentID, const std::string& peerID,
                           ActionType action, float influenceStrength) {
        // Record peer behavior for influence calculation
        PeerBehavior behavior;
        behavior.action = action;
        behavior.influenceStrength = influenceStrength;
        behavior.timestamp = simulationStep_++;
        
        peerBehaviors_[peerID].push_back(behavior);
        
        // Record shared experience
        bool shared = false;
        for (auto& exp : sharedExperiences_[agentID]) {
            if (exp.peerID == peerID) {
                exp.influenceStrength += influenceStrength;
                shared = true;
                break;
            }
        }
        
        if (!shared) {
            SharedExperience exp;
            exp.peerID = peerID;
            exp.influenceStrength = influenceStrength;
            sharedExperiences_[agentID].push_back(exp);
        }
    }
    
    void cleanupOldData() {
        // Remove old data to keep memory bounded
        for (auto& pair : peerBehaviors_) {
            while (pair.second.size() > 100) {
                pair.second.erase(pair.second.begin());
            }
        }
        
        for (auto& pair : sharedExperiences_) {
            while (pair.second.size() > 50) {
                pair.second.erase(pair.second.begin());
            }
        }
    }
    
    void setParameters(const InfluenceParameters& params) {
        this->params = params;
    }
    
    void reset() {
        peerBehaviors_.clear();
        socialInfluence_.clear();
        sharedExperiences_.clear();
        simulationStep_ = 0;
    }
    
private:
    InfluenceParameters params;
    
    struct PeerBehavior {
        ActionType action;
        float influenceStrength;
        TimestepDuration timestamp;
    };
    
    struct SharedExperience {
        std::string peerID;
        float influenceStrength;
    };
    
    std::unordered_map<std::string, std::vector<PeerBehavior>> peerBehaviors_;
    std::unordered_map<std::string, float> socialInfluence_;
    std::unordered_map<std::string, std::vector<SharedExperience>> sharedExperiences_;
    TimestepDuration simulationStep_;
};

// Collective Wisdom Implementation
class CollectiveWisdom {
public:
    struct WisdomParameters {
        float aggregationStrength;     // Strength of collective wisdom
        float diversityWeight;         // Weight for behavioral diversity
        float consensusThreshold;      // Threshold for consensus
        float knowledgeDecay;          // Rate of knowledge decay
    };
    
    CollectiveWisdom() {
        params.aggregationStrength = 0.7f;
        params.diversityWeight = 0.3f;
        params.consensusThreshold = 0.6f;
        params.knowledgeDecay = 0.001f;
    }
    
    std::vector<float> aggregateKnowledge(const std::vector<std::string>& agentIDs,
                                          const std::vector<float>& currentState) {
        std::vector<float> aggregatedKnowledge(currentState.size(), 0.0f);
        float totalWeight = 0.0f;
        
        for (const auto& agentID : agentIDs) {
            auto it = knowledgeBase_.find(agentID);
            if (it != knowledgeBase_.end()) {
                const auto& knowledge = it->second;
                
                // Compute similarity to current state
                float similarity = computeSimilarity(currentState, knowledge.state);
                
                // Apply diversity weighting
                float diversity = 1.0f - similarity;
                float weight = params.aggregationStrength + 
                             params.diversityWeight * diversity;
                
                if (weight > 0.0f) {
                    for (size_t i = 0; i < knowledge.action.value.size() && i < aggregatedKnowledge.size(); ++i) {
                        aggregatedKnowledge[i] += weight * knowledge.action.value[i];
                    }
                    totalWeight += weight;
                }
            }
        }
        
        // Normalize
        if (totalWeight > 0.0f) {
            for (size_t i = 0; i < aggregatedKnowledge.size(); ++i) {
                aggregatedKnowledge[i] /= totalWeight;
            }
        }
        
        return aggregatedKnowledge;
    }
    
    bool detectConsensus(const std::vector<float>& aggregatedKnowledge) const {
        // Check if aggregated knowledge represents consensus
        if (knowledgeHistory_.empty()) return false;
        
        float maxConsensus = 0.0f;
        for (const auto& history : knowledgeHistory_) {
            float similarity = computeSimilarity(aggregatedKnowledge, history);
            maxConsensus = std::max(maxConsensus, similarity);
        }
        
        return maxConsensus > params.consensusThreshold;
    }
    
    void storeKnowledge(const std::string& agentID, ActionType action,
                       const std::vector<float>& state,
                       const std::vector<float>& value) {
        KnowledgeEntry entry;
        entry.agentID = agentID;
        entry.action = action;
        entry.state = state;
        entry.value = value;
        entry.timestamp = simulationStep_++;
        
        knowledgeBase_[agentID].push_back(entry);
        knowledgeHistory_.push_back(state);
        
        // Keep history bounded
        while (knowledgeHistory_.size() > 1000) {
            knowledgeHistory_.erase(knowledgeHistory_.begin());
        }
    }
    
    void updateKnowledgeDecay() {
        // Decay old knowledge
        for (auto& pair : knowledgeBase_) {
            std::vector<KnowledgeEntry>& entries = pair.second;
            for (auto& entry : entries) {
                entry.value *= (1.0f - params.knowledgeDecay);
            }
        }
    }
    
    void setParameters(const WisdomParameters& params) {
        this->params = params;
    }
    
    void reset() {
        knowledgeBase_.clear();
        knowledgeHistory_.clear();
        simulationStep_ = 0;
    }
    
private:
    WisdomParameters params;
    
    struct KnowledgeEntry {
        std::string agentID;
        ActionType action;
        std::vector<float> state;
        std::vector<float> value;
        TimestepDuration timestamp;
    };
    
    std::unordered_map<std::string, std::vector<KnowledgeEntry>> knowledgeBase_;
    std::vector<std::vector<float>> knowledgeHistory_;
    TimestepDuration simulationStep_;
};

// Social Learning Implementation
struct SocialLearning::Impl {
    ImitationLearner imitationLearner;
    CulturalTransmission culturalTransmission;
    PeerInfluence peerInfluence;
    CollectiveWisdom collectiveWisdom;
    
    Impl() : imitationLearner(), culturalTransmission(), peerInfluence(), collectiveWisdom() {
    }
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
    , signalMeaning_(0.0f)
{
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                        const std::vector<float>& observerState,
                                        const std::vector<float>& resultingState) {
    // Record observation for imitation learning
    pImpl->imitationLearner.recordObservedBehavior(
        "observer", observedAction, observerState, resultingState, 0.0f);
    
    // Update cultural transmission
    pImpl->culturalTransmission.updateCulture("observer", observerState);
    
    // Social coordination through collective wisdom
    std::vector<float> culturalTraits = pImpl->culturalTransmission.getCulturalTraits("observer");
    std::vector<float> aggregated = pImpl->collectiveWisdom.aggregateKnowledge(
        std::vector<std::string>{"observer"}, observerState);
    
    ++observationCount_;
    
    // Keep memory bounded
    if (observedActions_.size() > 500) {
        observedActions_.erase(observedActions_.begin());
    }
}

void SocialLearning::imitate(const std::string& agentID,
                           const std::vector<float>& currentState,
                           float predictedReward) {
    // Imitation learning based on observations
    if (pImpl->imitationLearner.canImitate(agentID, currentState, predictedReward)) {
        // Record successful imitation
        imitationSuccesses_[agentID]++;
        
        // Update cultural transmission based on imitation success
        pImpl->culturalTransmission.updatePrestige(agentID, 1.0f, imitationSuccesses_[agentID]);
    }
}

void SocialLearning::updatePeerInfluence(const std::string& agentID,
                                        const std::vector<std::pair<std::string, float>>& peerActions) {
    // Update peer influence effects
    pImpl->peerInfluence.updateSocialInfluence(agentID, peerActions);
    
    // Record peer behavior
    for (const auto& peerPair : peerActions) {
        const std::string& peerID = peerPair.first;
        float influence = peerPair.second;
        
        // For now, use default action - in real implementation would track actual actions
        ActionType action = ActionType::Wait;
        pImpl->peerInfluence.recordPeerBehavior(agentID, peerID, action, influence);
    }
}

void SocialLearning::updateCollectiveWisdom(const std::vector<std::string>& agentIDs,
                                           const std::vector<float>& currentState) {
    // Update collective wisdom based on group observations
    std::vector<float> aggregated = pImpl->collectiveWisdom.aggregateKnowledge(
        agentIDs, currentState);
    
    bool consensus = pImpl->collectiveWisdom.detectConsensus(aggregated);
    
    // Store new collective knowledge
    pImpl->collectiveWisdom.storeKnowledge("collective", ActionType::Wait, aggregated, aggregated);
}

void SocialLearning::update(TimestepDuration dt) {
    // Update all social learning mechanisms
    pImpl->peerInfluence.cleanupOldData();
    pImpl->culturalTransmission.updateKnowledgeDecay();
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                              float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    if (signalPattern_.empty()) return false;
    
    float similarity = computeSimilarity(neuralPattern, signalPattern_);
    return similarity > 0.7f;
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update value of observed actions based on whether interaction was beneficial
    for (auto& pair : observedActions_) {
        // Adjust reward estimate based on interaction outcome
        pair.second.reward = pair.second.reward * 0.9f + interactionReward * 0.1f;
    }
}

void SocialLearning::clear() {
    pImpl->imitationLearner.reset();
    pImpl->culturalTransmission.reset();
    pImpl->peerInfluence.reset();
    pImpl->collectiveWisdom.reset();
    
    observedActions_.clear();
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
    observationCount_ = 0;
}

float SocialLearning::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
