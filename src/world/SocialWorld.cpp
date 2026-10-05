#include "SocialWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

SocialWorld::SocialWorld()
    : socialRadius_(5.0f)
    , bondDecayRate_(0.95f)
    , socialEnergyDecayRate_(0.98f)
    , socialNeedGrowthRate_(0.1f)
    , conflictResolutionRate_(0.8f)
    , timeSinceCulturalUpdate_(0.0)
    , lastSocialStatsUpdate_(0.0)
    , avgSocialBond_(0.0f)
    , avgTrustLevel_(0.0f)
    , maxEventLogSize_(1000)
{
    NLM_LOG_INFO("SocialWorld initialized with enhanced social dynamics");
}

SocialWorld::~SocialWorld() {
    NLM_LOG_INFO("SocialWorld destroyed");
}

void SocialWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight,
                          float socialRadius, float bondDecayRate) {
    SimpleWorld::configure(width, height, visionWidth, visionHeight);
    socialRadius_ = socialRadius;
    bondDecayRate_ = bondDecayRate;
    
    NLM_LOG_INFO("SocialWorld configured: " + std::to_string(width) + "x" + std::to_string(height) +
                 " social radius: " + std::to_string(socialRadius_) +
                 " bond decay rate: " + std::to_string(bondDecayRate_));
}

void SocialWorld::addAgent(uint64_t agentId, float x, float y, float radius) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    // Check if agent already exists
    if (agents_.find(agentId) != agents_.end()) {
        NLM_LOG_WARNING("Agent " + std::to_string(agentId) + " already exists");
        return;
    }
    
    // Create new agent social state
    AgentSocialState state;
    state.agentId = agentId;
    state.socialEnergy = 10.0f;  // Initial social energy
    state.socialNeed = 0.5f;     // Medium social need
    state.stressLevel = 0.0f;
    state.lastSocialAction = 0.0;
    state.isActive = false;
    
    // Add agent to world
    addAgentBody(x, y, radius);
    agents_[agentId] = state;
    
    // Initialize social bonds with all existing agents
    for (const auto& pair : agents_) {
        if (pair.first != agentId) {
            SocialBond bond;
            bond.agentId = pair.first;
            bond.bondStrength = 0.1f;  // Initial weak bond
            bond.trust = 0.5f;
            bond.reciprocity = 0.5f;
            bond.lastInteraction = 0.0;
            bond.bondType = "initial_contact";
            
            state.bonds[pair.first] = bond;
            pair.second.bonds[agentId] = bond;
        }
    }
    
    NLM_LOG_INFO("Added social agent " + std::to_string(agentId) + " at position (" +
                 std::to_string(x) + ", " + std::to_string(y) + ")");
}

void SocialWorld::removeAgent(uint64_t agentId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(agentId) == agents_.end()) {
        NLM_LOG_WARNING("Cannot remove agent " + std::to_string(agentId) + ": agent not found");
        return;
    }
    
    // Remove all social bonds with this agent
    AgentSocialState& state = agents_[agentId];
    for (const auto& bond : state.bonds) {
        uint64_t otherId = bond.first;
        if (agents_.find(otherId) != agents_.end()) {
            agents_[otherId].bonds.erase(agentId);
        }
    }
    
    // Remove agent from world (using parent class method)
    removeAgentBody();  // Note: This is a simplification, would need to be more sophisticated
    
    // Remove from social state
    agents_.erase(agentId);
    
    NLM_LOG_INFO("Removed social agent " + std::to_string(agentId));
}

AgentBody* SocialWorld::getAgent(uint64_t agentId) {
    // Simplified implementation - would need proper agent body management
    // For now, just return the main agent body
    return &SimpleWorld::getAgentBody();
}

const AgentBody* SocialWorld::getAgent(uint64_t agentId) const {
    // Simplified implementation - would need proper agent body management
    // For now, just return the main agent body
    return &SimpleWorld::getAgentBody();
}

void SocialWorld::socialInteraction(uint64_t fromAgent, uint64_t toAgent, 
                                   SocialInteractionType type, float strength) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(fromAgent) == agents_.end() || 
        agents_.find(toAgent) == agents_.end()) {
        NLM_LOG_WARNING("Cannot perform social interaction: invalid agent IDs");
        return;
    }
    
    AgentSocialState& fromState = agents_[fromAgent];
    AgentSocialState& toState = agents_[toAgent];
    
    // Check if interaction type is allowed by social rules
    applySocialRuleEffects(type, fromAgent);
    
    // Calculate bond change based on interaction type
    float bondChange = calculateBondChange(type, strength);
    
    // Update bond strength
    if (fromState.bonds.find(toAgent) != fromState.bonds.end()) {
        fromState.bonds[toAgent].bondStrength = 
            std::clamp(fromState.bonds[toAgent].bondStrength + bondChange, 0.0f, 1.0f);
    } else {
        SocialBond newBond;
        newBond.agentId = toAgent;
        newBond.bondStrength = bondChange;
        fromState.bonds[toAgent] = newBond;
    }
    
    if (toState.bonds.find(fromAgent) != toState.bonds.end()) {
        toState.bonds[fromAgent].bondStrength = 
            std::clamp(toState.bonds[fromAgent].bondStrength + bondChange, 0.0f, 1.0f);
    } else {
        SocialBond newBond;
        newBond.agentId = fromAgent;
        newBond.bondStrength = bondChange;
        toState.bonds[fromAgent] = newBond;
    }
    
    // Calculate trust change
    float trustChange = calculateTrustChange(type, strength);
    
    if (fromState.bonds.find(toAgent) != fromState.bonds.end()) {
        fromState.bonds[toAgent].trust = 
            std::clamp(fromState.bonds[toAgent].trust + trustChange, 0.0f, 1.0f);
    }
    
    if (toState.bonds.find(fromAgent) != toState.bonds.end()) {
        toState.bonds[fromAgent].trust = 
            std::clamp(toState.bonds[fromAgent].trust + trustChange, 0.0f, 1.0f);
    }
    
    // Update last interaction time
    double currentTime = getSimulationTime();
    fromState.lastSocialAction = currentTime;
    toState.lastSocialAction = currentTime;
    
    // Record social event
    recordSocialEvent("social_interaction", fromAgent,
                     {{ "to_agent", static_cast<float>(toAgent)},
                      { "interaction_type", static_cast<float>(type)},
                      { "strength", strength}});
    
    NLM_LOG_DEBUG("Social interaction: agent " + std::to_string(fromAgent) + " -> " +
                 std::to_string(toAgent) + " type: " + std::to_string(static_cast<int>(type)) +
                 " strength: " + std::to_string(strength));
}

float SocialWorld::getSocialBondStrength(uint64_t agentId, uint64_t otherAgentId) const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it1 = agents_.find(agentId);
    auto it2 = agents_.find(otherAgentId);
    
    if (it1 == agents_.end() || it2 == agents_.end()) {
        return 0.0f;
    }
    
    const auto& bonds1 = it1->second.bonds;
    const auto& bonds2 = it2->second.bonds;
    
    float bond1 = 0.0f, bond2 = 0.0f;
    
    if (bonds1.find(otherAgentId) != bonds1.end()) {
        bond1 = bonds1.at(otherAgentId).bondStrength;
    }
    
    if (bonds2.find(agentId) != bonds2.end()) {
        bond2 = bonds2.at(agentId).bondStrength;
    }
    
    // Average the two directional bond strengths
    return (bond1 + bond2) / 2.0f;
}

float SocialWorld::getSocialTrust(uint64_t agentId, uint64_t otherAgentId) const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it1 = agents_.find(agentId);
    auto it2 = agents_.find(otherAgentId);
    
    if (it1 == agents_.end() || it2 == agents_.end()) {
        return 0.0f;
    }
    
    const auto& bonds1 = it1->second.bonds;
    const auto& bonds2 = it2->second.bonds;
    
    float trust1 = 0.0f, trust2 = 0.0f;
    
    if (bonds1.find(otherAgentId) != bonds1.end()) {
        trust1 = bonds1.at(otherAgentId).trust;
    }
    
    if (bonds2.find(agentId) != bonds2.end()) {
        trust2 = bonds2.at(agentId).trust;
    }
    
    // Average the two directional trust levels
    return (trust1 + trust2) / 2.0f;
}

void SocialWorld::addSocialRule(const SocialRule& rule) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    socialRules_[rule.ruleId] = rule;
    
    NLM_LOG_DEBUG("Added social rule: " + rule.ruleId + " - " + rule.description);
}

void SocialWorld::removeSocialRule(const std::string& ruleId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (socialRules_.find(ruleId) != socialRules_.end()) {
        socialRules_.erase(ruleId);
        NLM_LOG_DEBUG("Removed social rule: " + ruleId);
    }
}

void SocialWorld::applySocialRules(uint64_t agentId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(agentId) == agents_.end()) {
        return;
    }
    
    AgentSocialState& state = agents_[agentId];
    
    for (const auto& pair : socialRules_) {
        const SocialRule& rule = pair.second;
        applySocialRuleEffects(rule, agentId);
    }
}

void SocialWorld::socialLearning(uint64_t learnerAgent, uint64_t observedAgent,
                                float observationQuality) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(learnerAgent) == agents_.end() ||
        agents_.find(observedAgent) == agents_.end()) {
        NLM_LOG_WARNING("Cannot perform social learning: invalid agent IDs");
        return;
    }
    
    // Extract knowledge from observed agent
    std::vector<float> knowledge = extractKnowledgeFromAgent(observedAgent);
    
    // Apply knowledge to learner
    distributeKnowledge(learnerAgent, observedAgent, knowledge);
    
    // Record learning event
    recordSocialEvent("social_learning", learnerAgent,
                     {{ "observed_agent", static_cast<float>(observedAgent)},
                      { "observation_quality", observationQuality},
                      { "knowledge_size", static_cast<float>(knowledge.size())}});
    
    NLM_LOG_DEBUG("Social learning: agent " + std::to_string(learnerAgent) + 
                 " learned from agent " + std::to_string(observedAgent) +
                 " quality: " + std::to_string(observationQuality));
}

void SocialWorld::teaching(uint64_t teacherAgent, uint64_t studentAgent,
                          const std::vector<float>& knowledgeTransfer) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(teacherAgent) == agents_.end() ||
        agents_.find(studentAgent) == agents_.end()) {
        NLM_LOG_WARNING("Cannot perform teaching: invalid agent IDs");
        return;
    }
    
    // Transfer knowledge from teacher to student
    distributeKnowledge(studentAgent, teacherAgent, knowledgeTransfer);
    
    // Record teaching event
    recordSocialEvent("teaching", teacherAgent,
                     {{ "student_agent", static_cast<float>(studentAgent)},
                      { "knowledge_size", static_cast<float>(knowledgeTransfer.size())}});
    
    NLM_LOG_DEBUG("Teaching: agent " + std::to_string(teacherAgent) + " taught agent " +
                 std::to_string(studentAgent) + " knowledge size: " +
                 std::to_string(knowledgeTransfer.size()));
}

void SocialWorld::formAlliance(const std::vector<uint64_t>& agentIds) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    // Validate agent IDs
    std::vector<uint64_t> validIds;
    for (uint64_t agentId : agentIds) {
        if (agents_.find(agentId) != agents_.end()) {
            validIds.push_back(agentId);
        }
    }
    
    if (validIds.size() < 2) {
        NLM_LOG_WARNING("Cannot form alliance: need at least 2 valid agents");
        return;
    }
    
    // Create alliance bonds between all agents in the alliance
    for (size_t i = 0; i < validIds.size(); ++i) {
        for (size_t j = i + 1; j < validIds.size(); ++j) {
            uint64_t agent1 = validIds[i];
            uint64_t agent2 = validIds[j];
            
            // Set strong bond for alliance
            if (agents_[agent1].bonds.find(agent2) != agents_[agent1].bonds.end()) {
                agents_[agent1].bonds[agent2].bondStrength = 1.0f;
                agents_[agent1].bonds[agent2].trust = 0.8f;
                agents_[agent1].bonds[agent2].reciprocity = 0.9f;
                agents_[agent1].bonds[agent2].bondType = "alliance";
            }
            
            if (agents_[agent2].bonds.find(agent1) != agents_[agent2].bonds.end()) {
                agents_[agent2].bonds[agent1].bondStrength = 1.0f;
                agents_[agent2].bonds[agent1].trust = 0.8f;
                agents_[agent2].bonds[agent1].reciprocity = 0.9f;
                agents_[agent2].bonds[agent1].bondType = "alliance";
            }
        }
    }
    
    // Record alliance formation
    std::stringstream ss;
    for (size_t i = 0; i < validIds.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << validIds[i];
    }
    
    recordSocialEvent("alliance_formed", validIds[0],
                     {{ "alliance_members", static_cast<float>(validIds.size())},
                      { "member_ids", std::stoi(ss.str())}});
    
    NLM_LOG_DEBUG("Alliance formed between agents: " + ss.str());
}

void SocialWorld::breakAlliance(uint64_t agentId, uint64_t otherAgentId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(agentId) == agents_.end() ||
        agents_.find(otherAgentId) == agents_.end()) {
        return;
    }
    
    // Check if agents are allied
    bool isAllied1 = isAllied(agentId, otherAgentId);
    
    if (isAllied1) {
        // Break alliance bond
        if (agents_[agentId].bonds.find(otherAgentId) != agents_[agentId].bonds.end()) {
            agents_[agentId].bonds[otherAgentId].bondStrength = 0.3f;
            agents_[agentId].bonds[otherAgentId].bondType = "former_alliance";
        }
        
        if (agents_[otherAgentId].bonds.find(agentId) != agents_[otherAgentId].bonds.end()) {
            agents_[otherAgentId].bonds[agentId].bondStrength = 0.3f;
            agents_[otherAgentId].bonds[agentId].bondType = "former_alliance";
        }
        
        // Record alliance breaking
        recordSocialEvent("alliance_broken", agentId,
                         {{ "other_agent", static_cast<float>(otherAgentId)}});
        
        NLM_LOG_DEBUG("Alliance broken between agents " + std::to_string(agentId) +
                     " and " + std::to_string(otherAgentId));
    }
}

bool SocialWorld::isAllied(uint64_t agentId, uint64_t otherAgentId) const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    auto it1 = agents_.find(agentId);
    auto it2 = agents_.find(otherAgentId);
    
    if (it1 == agents_.end() || it2 == agents_.end()) {
        return false;
    }
    
    const auto& bonds1 = it1->second.bonds;
    
    if (bonds1.find(otherAgentId) == bonds1.end()) {
        return false;
    }
    
    const SocialBond& bond = bonds1.at(otherAgentId);
    return bond.bondStrength > 0.8f && bond.bondType == "alliance";
}

float SocialWorld::getAverageSocialBond() const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.empty()) {
        return 0.0f;
    }
    
    float totalBond = 0.0f;
    int bondCount = 0;
    
    for (const auto& pair : agents_) {
        const auto& bonds = pair.second.bonds;
        for (const auto& bondPair : bonds) {
            totalBond += bondPair.second.bondStrength;
            bondCount++;
        }
    }
    
    avgSocialBond_ = (bondCount > 0) ? totalBond / bondCount : 0.0f;
    return avgSocialBond_;
}

float SocialWorld::getAverageTrustLevel() const {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.empty()) {
        return 0.0f;
    }
    
    float totalTrust = 0.0f;
    int trustCount = 0;
    
    for (const auto& pair : agents_) {
        const auto& bonds = pair.second.bonds;
        for (const auto& bondPair : bonds) {
            totalTrust += bondPair.second.trust;
            trustCount++;
        }
    }
    
    avgTrustLevel_ = (trustCount > 0) ? totalTrust / trustCount : 0.0f;
    return avgTrustLevel_;
}

void SocialWorld::updateSocialDynamics(double timestep) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    // Update social bonds based on decay
    updateSocialBonds(timestep);
    
    // Process social queues
    processSocialQueue(timestep);
    
    // Update social statistics
    double currentTime = getSimulationTime();
    if (currentTime - lastSocialStatsUpdate_ > 1.0) {
        // Update statistics every second
        updateSocialStatistics();
        lastSocialStatsUpdate_ = currentTime;
    }
    
    // Update cultural transmission
    timeSinceCulturalUpdate_ += timestep;
    if (timeSinceCulturalUpdate_ > 10.0) {
        transmitCulturalKnowledge();
        timeSinceCulturalUpdate_ = 0.0;
    }
}

void SocialWorld::processSocialEvents(double timestep) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    // Process any pending social events
    for (auto& pair : agents_) {
        AgentSocialState& state = pair.second;
        
        // Update social need based on current state
        if (state.isActive) {
            state.socialNeed = std::min(1.0f, state.socialNeed + socialNeedGrowthRate_ * timestep);
        }
        
        // Decay social energy
        state.socialEnergy *= std::pow(socialEnergyDecayRate_, timestep);
        
        // Update stress level based on social interactions
        state.stressLevel = std::min(1.0f, state.stressLevel + 0.01f * timestep);
        
        // Check for social conflicts
        checkSocialConflicts(state, timestep);
        
        // Process social queue
        processSocialQueueForAgent(state, timestep);
    }
}

void SocialWorld::resolveConflict(uint64_t agent1, uint64_t agent2, 
                                  SocialInteractionType conflictType) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(agent1) == agents_.end() ||
        agents_.find(agent2) == agents_.end()) {
        return;
    }
    
    // Handle conflict based on type
    handleSocialConflict(agent1, agent2, conflictType);
    
    // Apply conflict resolution
    if (conflictType == SocialInteractionType::Competition) {
        // Competition resolution: increase stress for both agents
        agents_[agent1].stressLevel = std::min(1.0f, agents_[agent1].stressLevel + 0.1f);
        agents_[agent2].stressLevel = std::min(1.0f, agents_[agent2].stressLevel + 0.1f);
    } else if (conflictType == SocialInteractionType::Cooperation) {
        // Cooperation resolution: decrease stress and increase bond
        agents_[agent1].stressLevel = std::max(0.0f, agents_[agent1].stressLevel - 0.05f);
        agents_[agent2].stressLevel = std::max(0.0f, agents_[agent2].stressLevel - 0.05f);
        
        // Increase bond strength
        float bondChange = 0.05f;
        if (agents_[agent1].bonds.find(agent2) != agents_[agent1].bonds.end()) {
            agents_[agent1].bonds[agent2].bondStrength = 
                std::min(1.0f, agents_[agent1].bonds[agent2].bondStrength + bondChange);
        }
    }
    
    // Record conflict resolution
    recordSocialEvent("conflict_resolved", agent1,
                     {{ "other_agent", static_cast<float>(agent2)},
                      { "conflict_type", static_cast<float>(conflictType)}});
    
    NLM_LOG_DEBUG("Conflict resolved between agents " + std::to_string(agent1) +
                 " and " + std::to_string(agent2) + " type: " +
                 std::to_string(static_cast<int>(conflictType)));
}

void SocialWorld::enforceSocialNorms(uint64_t agentId) {
    std::lock_guard<std::mutex> lock(socialMutex_);
    
    if (agents_.find(agentId) == agents_.end()) {
        return;
    }
    
    // Apply social norms based on agent's behavior
    AgentSocialState& state = agents_[agentId];
    
    // Example: discourage excessive aggression
    if (state.stressLevel > 0.8f) {
        // Apply penalty for high stress
        state.socialEnergy *= 0.9f;
        NLM_LOG_DEBUG("Agent " + std::to_string(agentId) + " penalized for high stress level");
    }
    
    // Example: reward cooperation
    checkForCooperativeBehavior(state);
}

void SocialWorld::recordSocialEvent(const std::string& eventType, uint64_t agentId,
                                   const std::unordered_map<std::string, float>& eventData) {
    SocialEvent event;
    event.timestamp = getSimulationTime();
    event.eventType = eventType;
    event.agentId = agentId;
    event.data = eventData;
    event.description = generateEventDescription(eventType, agentId, eventData);
    
    socialEventLog_.push_back(event);
    
    // Maintain event log size
    if (socialEventLog_.size() > maxEventLogSize_) {
        socialEventLog_.erase(socialEventLog_.begin());
    }
}

void SocialWorld::transmitCulture(const std::string& culturalTrait, float strength) {
    culturalValues_[culturalTrait] += strength;
    culturalTrends_[culturalTrait].push_back(strength);
    
    // Keep trends manageable
    if (culturalTrends_[culturalTrait].size() > 100) {
        culturalTrends_[culturalTrait].erase(culturalTrends_[culturalTrait].begin());
    }
    
    NLM_LOG_DEBUG("Culture transmitted: " + culturalTrait + " strength: " + std::to_string(strength));
}

float SocialWorld::getCulturalValue(const std::string& culturalTrait) const {
    auto it = culturalValues_.find(culturalTrait);
    return (it != culturalValues_.end()) ? it->second : 0.0f;
}

std::string SocialWorld::getSocialStateSnapshot() const {
    std::stringstream ss;
    ss << "=== Social World State ===\n";
    ss << "Agents: " << agents_.size() << "\n";
    ss << "Social Bonds: " << getAverageSocialBond() << " (avg)\n";
    ss << "Trust Level: " << getAverageTrustLevel() << " (avg)\n";
    ss << "Social Rules: " << socialRules_.size() << "\n";
    ss << "Cultural Traits: " << culturalValues_.size() << "\n";
    ss << "Recent Events: " << socialEventLog_.size() << "\n";
    
    ss << "\n--- Agent Details ---\n";
    for (const auto& pair : agents_) {
        const AgentSocialState& state = pair.second;
        ss << "Agent " << pair.first << ":\n";
        ss << "  Social Energy: " << state.socialEnergy << "\n";
        ss << "  Social Need: " << state.socialNeed << "\n";
        ss << "  Stress Level: " << state.stressLevel << "\n";
        ss << "  Bonds: " << state.bonds.size() << "\n";
    }
    
    ss << "\n--- Cultural Values ---\n";
    for (const auto& pair : culturalValues_) {
        ss << pair.first << ": " << pair.second << "\n";
    }
    
    return ss.str();
}

void SocialWorld::visualizeSocialNetwork() const {
    std::string networkGraph = generateSocialNetworkGraph();
    std::string bondHeatmap = generateBondHeatmap();
    
    NLM_LOG_INFO("Social Network Visualization:\n" + networkGraph);
    NLM_LOG_INFO("Bond Heatmap:\n" + bondHeatmap);
}

void SocialWorld::updateSocialBonds(double timestep) {
    // Decay bonds over time
    for (auto& pair1 : agents_) {
        AgentSocialState& state1 = pair1.second;
        
        for (auto& pair2 : state1.bonds) {
            uint64_t otherId = pair2.first;
            SocialBond& bond = pair2.second;
            
            // Decay bond strength
            bond.bondStrength *= bondDecayRate_;
            bond.trust *= bondDecayRate_;
            bond.reciprocity *= bondDecayRate_;
            
            // Update last interaction
            bond.lastInteraction += timestep;
        }
    }
}

void SocialWorld::processSocialQueue(double timestep) {
    for (auto& pair : agents_) {
        processSocialQueueForAgent(pair.second, timestep);
    }
}

void SocialWorld::applySocialRuleEffects(const SocialRule& rule, uint64_t agentId) {
    if (agents_.find(agentId) == agents_.end()) {
        return;
    }
    
    AgentSocialState& state = agents_[agentId];
    
    switch (rule.interactionType) {
        case SocialInteractionType::Cooperation:
            // Reward cooperative behavior
            if (state.socialEnergy < 5.0f) {
                state.socialEnergy += rule.strength * 2.0f;
            }
            break;
            
        case SocialInteractionType::Competition:
            // Punish lack of competitiveness
            if (state.socialEnergy > 15.0f) {
                state.socialEnergy -= rule.strength;
            }
            break;
            
        case SocialInteractionType::Avoidance:
            // Discourage unwanted social interactions
            state.socialNeed *= (1.0f - rule.strength);
            break;
            
        case SocialInteractionType::Alliance:
            // Encourage alliance formation
            state.socialEnergy += rule.strength * 1.5f;
            break;
            
        default:
            break;
    }
}

void SocialWorld::handleSocialConflict(uint64_t agent1, uint64_t agent2, 
                                      SocialInteractionType conflictType) {
    // Simple conflict handling
    switch (conflictType) {
        case SocialInteractionType::Competition:
            // Competition increases stress
            agents_[agent1].stressLevel = std::min(1.0f, agents_[agent1].stressLevel + 0.1f);
            agents_[agent2].stressLevel = std::min(1.0f, agents_[agent2].stressLevel + 0.1f);
            break;
            
        case SocialInteractionType::Cooperation:
            // Cooperation decreases stress
            agents_[agent1].stressLevel = std::max(0.0f, agents_[agent1].stressLevel - 0.05f);
            agents_[agent2].stressLevel = std::max(0.0f, agents_[agent2].stressLevel - 0.05f);
            break;
            
        case SocialInteractionType::Avoidance:
            // Avoidance has moderate stress impact
            agents_[agent1].stressLevel = std::min(1.0f, agents_[agent1].stressLevel + 0.05f);
            agents_[agent2].stressLevel = std::min(1.0f, agents_[agent2].stressLevel + 0.05f);
            break;
            
        default:
            break;
    }
}

void SocialWorld::distributeKnowledge(uint64_t learnerAgent, uint64_t sourceAgent,
                                     const std::vector<float>& knowledge) {
    if (agents_.find(learnerAgent) == agents_.end() ||
        agents_.find(sourceAgent) == agents_.end()) {
        return;
    }
    
    // Simulate knowledge transfer with decay based on bond strength
    float bondStrength = getSocialBondStrength(learnerAgent, sourceAgent);
    
    // Transfer knowledge with decay based on bond strength
    float transferEfficiency = bondStrength * knowledge.size() / 100.0f;  // Simplified
    
    // In a real implementation, this would update neural weights
    // For now, just record the learning
    agents_[learnerAgent].socialEnergy += transferEfficiency * 0.5f;
}

void SocialWorld::evolveSocialBehavior(uint64_t agentId, double timestep) {
    if (agents_.find(agentId) == agents_.end()) {
        return;
    }
    
    AgentSocialState& state = agents_[agentId];
    
    // Evolve social behavior based on experience
    if (state.socialEnergy > 10.0f) {
        // High energy leads to more social exploration
        state.socialNeed = std::min(1.0f, state.socialNeed + socialNeedGrowthRate_ * timestep);
    }
    
    // Stress reduces social need
    state.socialNeed = std::max(0.0f, state.socialNeed - state.stressLevel * timestep * 0.5f);
}

void SocialWorld::transmitCulturalKnowledge() {
    // Transmit cultural knowledge to agents based on their social bonds
    for (auto& pair : agents_) {
        AgentSocialState& state = pair.second;
        
        // Agents with strong bonds are more likely to receive cultural knowledge
        for (const auto& bondPair : state.bonds) {
            uint64_t otherId = bondPair.first;
            float bondStrength = bondPair.second.bondStrength;
            
            if (bondStrength > 0.5f) {
                // Transfer cultural values based on bond strength
                for (const auto& culturalPair : culturalValues_) {
                    if (bondStrength > 0.7f) {
                        // Strong bonds adopt cultural traits more readily
                        adoptCulturalTrait(otherId, culturalPair.first, culturalPair.second * 0.1f);
                    }
                }
            }
        }
    }
}

void SocialWorld::adoptCulturalTrait(uint64_t agentId, const std::string& trait, float strength) {
    if (agents_.find(agentId) == agents_.end()) {
        return;
    }
    
    // Agents with high social energy are more likely to adopt new cultural traits
    float adoptionProbability = agents_[agentId].socialEnergy / 20.0f;
    adoptionProbability = std::min(1.0f, adoptionProbability);
    
    if (rand() / (float)RAND_MAX < adoptionProbability) {
        culturalValues_[trait] += strength;
        NLM_LOG_DEBUG("Agent " + std::to_string(agentId) + " adopted cultural trait: " +
                     trait + " strength: " + std::to_string(strength));
    }
}

std::string SocialWorld::generateSocialNetworkGraph() const {
    std::stringstream ss;
    ss << "=== Social Network Graph ===\n";
    ss << "Nodes (Agents): " << agents_.size() << "\n";
    
    int totalEdges = 0;
    float maxBond = 0.0f;
    
    for (const auto& pair1 : agents_) {
        for (const auto& pair2 : pair1.second.bonds) {
            if (pair2.second.bondStrength > maxBond) {
                maxBond = pair2.second.bondStrength;
            }
            totalEdges++;
        }
    }
    
    ss << "Edges (Bonds): " << totalEdges << "\n";
    ss << "Maximum Bond Strength: " << maxBond << "\n";
    ss << "Average Bond Strength: " << getAverageSocialBond() << "\n";
    
    return ss.str();
}

std::string SocialWorld::generateBondHeatmap() const {
    std::stringstream ss;
    ss << "=== Bond Strength Heatmap ===\n";
    
    if (agents_.empty()) {
        ss << "No agents in world\n";
        return ss.str();
    }
    
    // Generate simple heatmap representation
    ss << "Bond strength ranges from 0.0 (white) to 1.0 (red)\n";
    
    // Display bond strengths for each agent
    for (const auto& pair : agents_) {
        ss << "Agent " << pair.first << ": ";
        
        if (pair.second.bonds.empty()) {
            ss << "No bonds\n";
            continue;
        }
        
        for (const auto& bondPair : pair.second.bonds) {
            float strength = bondPair.second.bondStrength;
            int intensity = static_cast<int>(strength * 10);
            
            ss << "-> Agent " << bondPair.first << ": ";
            for (int i = 0; i < intensity; ++i) {
                ss << "#";
            }
            ss << " ";
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

void SocialWorld::updateSocialStatistics() {
    // Update social statistics
    avgSocialBond_ = getAverageSocialBond();
    avgTrustLevel_ = getAverageTrustLevel();
}

void SocialWorld::checkSocialConflicts(AgentSocialState& state, double timestep) {
    // Check for potential social conflicts based on behavior
    if (state.socialNeed > 0.8f && state.socialEnergy < 2.0f) {
        // High need but low energy - potential frustration conflict
        recordSocialEvent("frustration_conflict", state.agentId,
                         {{ "social_need", state.socialNeed},
                          { "social_energy", state.socialEnergy}});
    }
}

void SocialWorld::processSocialQueueForAgent(AgentSocialState& state, double timestep) {
    // Process social queue for agent
    if (!state.socialQueue.empty()) {
        SocialInteractionType interaction = state.socialQueue.front();
        state.socialQueue.pop();
        
        // Apply social interaction effect
        applySocialRuleEffects(SocialRule("", interaction, 0.5f, 1.0f, false, "queued_interaction"), state.agentId);
    }
}

void SocialWorld::checkForCooperativeBehavior(AgentSocialState& state) {
    // Check if agent exhibits cooperative behavior
    if (state.socialEnergy > 10.0f && state.socialNeed < 0.3f) {
        // Cooperative behavior detected
        recordSocialEvent("cooperative_behavior", state.agentId,
                         {{ "social_energy", state.socialEnergy},
                          { "social_need", state.socialNeed}});
    }
}

std::string SocialWorld::generateEventDescription(const std::string& eventType, 
                                                 uint64_t agentId,
                                                 const std::unordered_map<std::string, float>& eventData) {
    std::stringstream ss;
    
    if (eventType == "social_interaction") {
        ss << "Agent " << agentId << " interacted with agent " 
           << static_cast<int>(eventData.at("to_agent")) << 
           " (type: " << static_cast<int>(eventData.at("interaction_type")) << ")";
    } else if (eventType == "social_learning") {
        ss << "Agent " << agentId << " learned from agent " 
           << static_cast<int>(eventData.at("observed_agent")) << 
           " with quality " << eventData.at("observation_quality");
    } else if (eventType == "teaching") {
        ss << "Agent " << agentId << " taught agent " 
           << static_cast<int>(eventData.at("student_agent")) << 
           " knowledge size " << eventData.at("knowledge_size");
    } else if (eventType == "alliance_formed") {
        ss << "Alliance formed involving " << eventData.at("alliance_members") << " agents";
    } else if (eventType == "conflict_resolved") {
        ss << "Conflict resolved between agent " << agentId << " and agent "
           << static_cast<int>(eventData.at("other_agent"));
    } else {
        ss << "Event: " << eventType << " involving agent " << agentId;
    }
    
    return ss.str();
}

// Static helper functions
float SocialWorld::calculateBondChange(SocialInteractionType interaction, float strength) {
    switch (interaction) {
        case SocialInteractionType::Cooperation:
            return strength * 0.1f;
        case SocialInteractionType::Competition:
            return strength * (-0.05f);
        case SocialInteractionType::Avoidance:
            return strength * (-0.02f);
        case SocialInteractionType::Alliance:
            return strength * 0.15f;
        case SocialInteractionType::Dominance:
            return strength * 0.08f;
        case SocialInteractionType::Submission:
            return strength * (-0.03f);
        case SocialInteractionType::Grooming:
            return strength * 0.12f;
        case SocialInteractionType::Play:
            return strength * 0.1f;
        case SocialInteractionType::Teaching:
            return strength * 0.1f;
        case SocialInteractionType::Learning:
            return strength * 0.08f;
        default:
            return 0.0f;
    }
}

float SocialWorld::calculateTrustChange(SocialInteractionType interaction, float strength) {
    switch (interaction) {
        case SocialInteractionType::Cooperation:
            return strength * 0.1f;
        case SocialInteractionType::Competition:
            return strength * (-0.05f);
        case SocialInteractionType::Avoidance:
            return strength * (-0.02f);
        case SocialInteractionType::Alliance:
            return strength * 0.2f;
        case SocialInteractionType::Dominance:
            return strength * 0.15f;
        case SocialInteractionType::Submission:
            return strength * (-0.1f);
        case SocialInteractionType::Grooming:
            return strength * 0.15f;
        case SocialInteractionType::Play:
            return strength * 0.1f;
        case SocialInteractionType::Teaching:
            return strength * 0.1f;
        case SocialInteractionType::Learning:
            return strength * 0.08f;
        default:
            return 0.0f;
    }
}

bool SocialWorld::isReciprocalInteraction(SocialInteractionType interaction) {
    return (interaction == SocialInteractionType::Cooperation ||
            interaction == SocialInteractionType::Alliance ||
            interaction == SocialInteractionType::Grooming ||
            interaction == SocialInteractionType::Play ||
            interaction == SocialInteractionType::Teaching);
}

std::string SocialWorld::getReciprocalInteraction(SocialInteractionType interaction) {
    switch (interaction) {
        case SocialInteractionType::Cooperation:
            return "cooperation";
        case SocialInteractionType::Alliance:
            return "alliance";
        case SocialInteractionType::Grooming:
            return "grooming";
        case SocialInteractionType::Play:
            return "play";
        case SocialInteractionType::Teaching:
            return "teaching";
        default:
            return "none";
    }
}

// Add missing method for agent body management
def addAgentBody(float x, float y, float radius) {
    // Add an agent body to the world
    // This would integrate with the parent SimpleWorld class
    // For now, just a placeholder
    NLM_LOG_DEBUG("Adding agent body at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
}

def removeAgentBody() {
    // Remove the agent body from the world
    // This would integrate with the parent SimpleWorld class
    // For now, just a placeholder
    NLM_LOG_DEBUG("Removing agent body");
}

} // namespace nlm
