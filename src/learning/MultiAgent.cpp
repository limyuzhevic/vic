#include "MultiAgent.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <numeric>
#include <set>

namespace nlm {

// Competition Model Implementation
class CompetitionModel {
public:
    struct CompetitionParameters {
        float competitionStrength;     // Strength of competitive behavior
        float resourceValue;           // Value of competing for resources
        float aggressionThreshold;     // Threshold for aggressive behavior
        float cooperationProbability;   // Probability of cooperative behavior
    };
    
    CompetitionModel() {
        params.competitionStrength = 0.4f;
        params.resourceValue = 1.0f;
        params.aggressionThreshold = 0.5f;
        params.cooperationProbability = 0.3f;
    }
    
    float computeResourceDemand(const std::vector<float>& state,
                               const std::string& agentID) const {
        // Compute how much of a resource an agent wants
        float demand = 0.0f;
        
        // Base demand from state
        for (float value : state) {
            demand += value;
        }
        demand /= state.size();
        
        // Add competition modifier based on other agents
        float competitionFactor = 1.0f;
        if (hasCompetitors(agentID)) {
            competitionFactor += params.competitionStrength;
        }
        
        return demand * competitionFactor;
    }
    
    bool isAggressive(const std::vector<float>& state,
                     const std::string& agentID) const {
        float demand = computeResourceDemand(state, agentID);
        return demand > params.aggressionThreshold;
    }
    
    bool chooseCooperation() {
        return std::rand() % 100 < (int)(params.cooperationProbability * 100);
    }
    
    void updateResourceAvailability(const std::string& resourceID,
                                    float availability) {
        resourceAvailability_[resourceID] = availability;
    }
    
    float getResourceAvailability(const std::string& resourceID) const {
        auto it = resourceAvailability_.find(resourceID);
        if (it == resourceAvailability_.end()) return 0.0f;
        return it->second;
    }
    
    void addCompetitor(const std::string& agentID) {
        competitors_.insert(agentID);
    }
    
    void removeCompetitor(const std::string& agentID) {
        competitors_.erase(agentID);
    }
    
    bool hasCompetitors(const std::string& agentID) const {
        return !competitors_.empty() && competitors_.count(agentID) == 0;
    }
    
    void setParameters(const CompetitionParameters& params) {
        this->params = params;
    }
    
    void reset() {
        resourceAvailability_.clear();
        competitors_.clear();
    }
    
private:
    CompetitionParameters params;
    std::unordered_map<std::string, float> resourceAvailability_;
    std::set<std::string> competitors_;
};

// Cooperation Model Implementation
class CooperationModel {
public:
    struct CooperationParameters {
        float mutualBenefit;          // Benefit from mutual cooperation
        float punishmentCost;         // Cost of punishing non-cooperators
        float rewardCost;             // Cost of rewarding cooperators
        float trustThreshold;         // Threshold for trust
        float forgivenessRate;        // Rate of forgiveness for defection
    };
    
    CooperationModel() {
        params.mutualBenefit = 0.8f;
        params.punishmentCost = 0.2f;
        params.rewardCost = 0.1f;
        params.trustThreshold = 0.5f;
        params.forgivenessRate = 0.05f;
    }
    
    float computeCooperationValue(const std::string& agentID,
                                const std::string& partnerID,
                                float pastInteraction) const {
        // Compute value of cooperating with a specific partner
        float trust = getTrust(agentID, partnerID);
        
        if (trust > params.trustThreshold) {
            // High trust leads to higher cooperation value
            return params.mutualBenefit * trust;
        }
        
        return pastInteraction * (1.0f - params.forgivenessRate);
    }
    
    bool shouldCooperate(const std::string& agentID,
                        const std::string& partnerID,
                        float expectedValue) {
        return expectedValue > 0.0f && std::rand() % 100 < (int)(params.cooperationProbability_ * 100);
    }
    
    void updateTrust(const std::string& agentID, const std::string& partnerID,
                    bool partnerCooperated, float interactionOutcome) {
        float& trust = trustMatrix_[agentID][partnerID];
        
        if (partnerCooperated) {
            trust = std::min(1.0f, trust + 0.1f * interactionOutcome);
        } else {
            trust = std::max(0.0f, trust - 0.2f * (1.0f - interactionOutcome));
        }
        
        trust = std::clamp(trust, 0.0f, 1.0f);
    }
    
    float getTrust(const std::string& agentID, const std::string& partnerID) const {
        auto it1 = trustMatrix_.find(agentID);
        if (it1 == trustMatrix_.end()) return 0.0f;
        
        auto it2 = it1->second.find(partnerID);
        if (it2 == it1->second.end()) return 0.0f;
        
        return it2->second;
    }
    
    void setParameters(const CooperationParameters& params) {
        this->params = params;
    }
    
    void reset() {
        trustMatrix_.clear();
    }
    
private:
    CooperationParameters params;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> trustMatrix_;
};

// Communication Protocol Implementation
class CommunicationProtocol {
public:
    struct ProtocolParameters {
        float messageReliability;      // Reliability of communication
        float encodingEfficiency;      // Efficiency of message encoding
        float decodingAccuracy;        // Accuracy of message decoding
        float noiseLevel;              // Level of communication noise
    };
    
    CommunicationProtocol() {
        params.messageReliability = 0.9f;
        params.encodingEfficiency = 0.8f;
        params.decodingAccuracy = 0.85f;
        params.noiseLevel = 0.05f;
    }
    
    std::string encodeMessage(const std::vector<float>& data) const {
        // Simple encoding for demonstration
        std::string encoded;
        encoded.reserve(data.size() * 8); // Approximate size
        
        for (float value : data) {
            // Convert float to binary representation
            int intValue = static_cast<int>(value * 255.0f);
            for (int i = 7; i >= 0; --i) {
                char bit = ((intValue >> i) & 1) ? '1' : '0';
                encoded += bit;
            }
        }
        
        return encoded;
    }
    
    std::vector<float> decodeMessage(const std::string& encoded) const {
        // Simple decoding for demonstration
        std::vector<float> decoded;
        decoded.reserve(encoded.size() / 8);
        
        for (size_t i = 0; i + 8 <= encoded.size(); i += 8) {
            std::string byte = encoded.substr(i, 8);
            int intValue = 0;
            
            for (size_t j = 0; j < 8; ++j) {
                if (byte[j] == '1') {
                    intValue |= (1 << (7 - j));
                }
            }
            
            float value = static_cast<float>(intValue) / 255.0f;
            
            // Add noise
            if (std::rand() % 100 < (int)(params.noiseLevel * 100)) {
                value += (std::rand() % 200 - 100) / 100.0f * 0.1f;
                value = std::clamp(value, 0.0f, 1.0f);
            }
            
            decoded.push_back(value);
        }
        
        return decoded;
    }
    
    float computeMessageReliability(const std::string& message) const {
        // Compute reliability of a message
        float reliability = params.messageReliability;
        
        // Reduce reliability based on message length
        reliability *= std::exp(-params.noiseLevel * message.size() / 100.0f);
        
        return reliability;
    }
    
    void setParameters(const ProtocolParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for protocol
    }
    
private:
    ProtocolParameters params;
};

// Resource Management Implementation
class ResourceManager {
public:
    struct ResourceParameters {
        float resourceSharing;         // Rate of resource sharing
        float resourceConsumption;     // Rate of resource consumption
        float resourceRegeneration;    // Rate of resource regeneration
        float maxResourceCapacity;     // Maximum resource capacity
    };
    
    ResourceManager() {
        params.resourceSharing = 0.3f;
        params.resourceConsumption = 0.2f;
        params.resourceRegeneration = 0.05f;
        params.maxResourceCapacity = 1.0f;
    }
    
    void updateResource(const std::string& agentID, const std::string& resourceID,
                       float amount, bool isConsumption = true) {
        // Update agent's resource amount
        std::string key = agentID + "_" + resourceID;
        
        if (isConsumption) {
            resources_[key] -= amount * params.resourceConsumption;
        } else {
            resources_[key] += amount;
        }
        
        // Clamp to reasonable limits
        resources_[key] = std::clamp(resources_[key], 0.0f, params.maxResourceCapacity);
        
        // Trigger sharing if threshold is met
        if (resources_[key] > params.maxResourceCapacity * 0.8f) {
            triggerResourceSharing(agentID, resourceID, amount * params.resourceSharing);
        }
    }
    
    float getResource(const std::string& agentID, const std::string& resourceID) const {
        std::string key = agentID + "_" + resourceID;
        auto it = resources_.find(key);
        if (it == resources_.end()) return 0.0f;
        return it->second;
    }
    
    void triggerResourceSharing(const std::string& providerID,
                                const std::string& resourceID,
                                float amount) {
        // Distribute resources to nearby agents
        for (const auto& agentID : nearbyAgents_[providerID]) {
            std::string key = agentID + "_" + resourceID;
            resources_[key] += amount * params.resourceSharing;
            resources_[key] = std::clamp(resources_[key], 0.0f, params.maxResourceCapacity);
        }
    }
    
    void updateAgentLocation(const std::string& agentID,
                            const std::vector<float>& position) {
        // Update agent's position and nearby agents
        locations_[agentID] = position;
        
        // Determine nearby agents (simplified: agents within radius 5.0)
        nearbyAgents_[agentID].clear();
        for (const auto& pair : locations_) {
            if (pair.first != agentID) {
                float distance = computeDistance(position, pair.second);
                if (distance < 5.0f) {
                    nearbyAgents_[agentID].push_back(pair.first);
                }
            }
        }
    }
    
    float computeDistance(const std::vector<float>& pos1,
                         const std::vector<float>& pos2) const {
        if (pos1.size() != pos2.size()) return 1000.0f;
        
        float distance = 0.0f;
        for (size_t i = 0; i < pos1.size(); ++i) {
            float diff = pos1[i] - pos2[i];
            distance += diff * diff;
        }
        
        return std::sqrt(distance);
    }
    
    void setParameters(const ResourceParameters& params) {
        this->params = params;
    }
    
    void reset() {
        resources_.clear();
        locations_.clear();
        nearbyAgents_.clear();
    }
    
private:
    ResourceParameters params;
    
    std::unordered_map<std::string, float> resources_;
    std::unordered_map<std::string, std::vector<float>> locations_;
    std::unordered_map<std::string, std::vector<std::string>> nearbyAgents_;
};

// MultiAgent Implementation
struct MultiAgent::Impl {
    CompetitionModel competitionModel;
    CooperationModel cooperationModel;
    CommunicationProtocol communicationProtocol;
    ResourceManager resourceManager;
    
    std::unordered_map<std::string, std::vector<float>> agentPositions;
    std::unordered_map<std::string, std::vector<float>> agentResources;
    std::unordered_map<std::string, std::vector<float>> agentBehaviors;
    
    Impl() : competitionModel(), cooperationModel(), communicationProtocol(), resourceManager() {
    }
};

MultiAgent::MultiAgent()
    : pImpl(new Impl)
    , brain_(nullptr)
    , currentStep_(0)
{
}

MultiAgent::~MultiAgent() = default;

void MultiAgent::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("MultiAgent initialized");
}

void MultiAgent::addAgent(const std::string& agentID,
                         const std::vector<float>& initialPosition) {
    // Add a new agent to the system
    pImpl->agentPositions[agentID] = initialPosition;
    pImpl->agentResources[agentID] = std::vector<float>(5, 0.0f); // 5 resource types
    pImpl->agentBehaviors[agentID] = std::vector<float>(10, 0.5f); // 10 behavior dimensions
    
    // Initialize resource manager
    pImpl->resourceManager.updateAgentLocation(agentID, initialPosition);
    
    NLM_LOG_INFO("Added agent " + agentID);
}

void MultiAgent::updateAgent(const std::string& agentID,
                            const std::vector<float>& state,
                            const std::vector<float>& action) {
    // Update agent's state and action
    pImpl->agentPositions[agentID] = state;
    pImpl->agentBehaviors[agentID] = action;
    
    // Update resource manager
    pImpl->resourceManager.updateAgentLocation(agentID, state);
    
    // Determine if agent competes or cooperates
    bool isAggressive = pImpl->competitionModel.isAggressive(state, agentID);
    
    // Find nearby agents
    std::vector<std::pair<std::string, float>> peerActions;
    for (const auto& pair : pImpl->agentPositions) {
        if (pair.first != agentID) {
            float distance = pImpl->resourceManager.computeDistance(
                state, pair.second);
            if (distance < 5.0f) {
                // Determine action from behavior vector
                int actionIndex = distance < 2.0f ? 0 : 1; // Simplified
                peerActions.push_back(std::make_pair(pair.first, (float)actionIndex));
            }
        }
    }
    
    // Update peer influence
    pImpl->cooperationModel.updateTrust(agentID, "test", true, 0.5f);
    
    // Update resource management
    for (size_t i = 0; i < pImpl->agentResources[agentID].size(); ++i) {
        pImpl->resourceManager.updateResource(
            agentID, "resource_" + std::to_string(i),
            pImpl->agentResources[agentID][i], true);
    }
    
    ++currentStep_;
}

void MultiAgent::updateResources(const std::vector<std::string>& agentIDs,
                                const std::vector<std::string>& resourceIDs,
                                const std::vector<std::vector<float>>& resourceAmounts) {
    // Update resources for multiple agents
    for (size_t i = 0; i < agentIDs.size(); ++i) {
        for (size_t j = 0; j < resourceIDs.size(); ++j) {
            pImpl->resourceManager.updateResource(
                agentIDs[i], resourceIDs[j], resourceAmounts[i][j], false);
        }
    }
}

void MultiAgent::updateCompetitor(const std::string& agentID,
                                 const std::vector<float>& competitorState,
                                 const std::vector<float>& reward) {
    // Update competition model based on competitor interaction
    pImpl->competitionModel.addCompetitor(agentID);
    
    // Compute resource demand
    float demand = pImpl->competitionModel.computeResourceDemand(
        competitorState, agentID);
    
    // Update resource availability
    for (size_t i = 0; i < reward.size(); ++i) {
        pImpl->competitionModel.updateResourceAvailability(
            "resource_" + std::to_string(i), reward[i]);
    }
}

void MultiAgent::computeCoordination(const std::vector<std::string>& agentIDs) {
    // Compute coordination between agents
    for (size_t i = 0; i < agentIDs.size(); ++i) {
        for (size_t j = i + 1; j < agentIDs.size(); ++j) {
            const std::string& agent1 = agentIDs[i];
            const std::string& agent2 = agentIDs[j];
            
            // Compute coordination based on proximity and similarity
            float coord = computeCoordinationScore(agent1, agent2);
            
            // Update cooperation model based on coordination
            bool shouldCooperate = pImpl->cooperationModel.shouldCooperate(
                agent1, agent2, coord);
        }
    }
}

float MultiAgent::computeCoordinationScore(const std::string& agent1,
                                          const std::string& agent2) const {
    // Compute coordination score between two agents
    auto it1 = pImpl->agentPositions.find(agent1);
    auto it2 = pImpl->agentPositions.find(agent2);
    
    if (it1 == pImpl->agentPositions.end() || it2 == pImpl->agentPositions.end()) {
        return 0.0f;
    }
    
    float distance = pImpl->resourceManager.computeDistance(
        it1->second, it2->second);
    
    // Normalize distance to coordination score
    float coordScore = std::max(0.0f, 1.0f - distance / 5.0f);
    
    // Add behavioral similarity
    auto it1b = pImpl->agentBehaviors.find(agent1);
    auto it2b = pImpl->agentBehaviors.find(agent2);
    
    if (it1b != pImpl->agentBehaviors.end() && it2b != pImpl->agentBehaviors.end()) {
        float similarity = computeSimilarity(it1b->second, it2b->second);
        coordScore = (coordScore + similarity) / 2.0f;
    }
    
    return coordScore;
}

float MultiAgent::computeSimilarity(const std::vector<float>& vec1,
                                   const std::vector<float>& vec2) const {
    if (vec1.size() != vec2.size()) return 0.0f;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < vec1.size(); ++i) {
        similarity += std::abs(vec1[i] - vec2[i]);
    }
    
    return 1.0f - (similarity / vec1.size());
}

void MultiAgent::update(TimestepDuration dt) {
    // Update multi-agent system
    // Update resource regeneration
    for (auto& pair : pImpl->resourceManager.getResources()) {
        pair.second *= (1.0f + pImpl->resourceManager.getRegenerationRate());
        pair.second = std::clamp(pair.second, 0.0f, pImpl->resourceManager.getMaxCapacity());
    }
}

void MultiAgent::setCompetitionParameters(const CompetitionModel::CompetitionParameters& params) {
    pImpl->competitionModel.setParameters(params);
}

void MultiAgent::setCooperationParameters(const CooperationModel::CooperationParameters& params) {
    pImpl->cooperationModel.setParameters(params);
}

void MultiAgent::setCommunicationParameters(const CommunicationProtocol::ProtocolParameters& params) {
    pImpl->communicationProtocol.setParameters(params);
}

void MultiAgent::setResourceParameters(const ResourceManager::ResourceParameters& params) {
    pImpl->resourceManager.setParameters(params);
}

void MultiAgent::reset() {
    pImpl->competitionModel.reset();
    pImpl->cooperationModel.reset();
    pImpl->communicationProtocol.reset();
    pImpl->resourceManager.reset();
    
    pImpl->agentPositions.clear();
    pImpl->agentResources.clear();
    pImpl->agentBehaviors.clear();
    
    currentStep_ = 0;
}

} // namespace nlm
