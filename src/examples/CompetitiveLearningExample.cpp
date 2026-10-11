#pragma once

#include "AgentManager.hpp"
#include "Environment.hpp"
#include "SocialInteraction.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>

namespace nlm {

// Competitive learning states
enum class LearningState {
    EXPLORATION,
    EXPLOITATION,
    COOPERATION,
    COMPETITION
};

// CompetitiveLearner agent for competitive and cooperative learning
class CompetitiveLearnerAgent : public AgentContainer {
public:
    CompetitiveLearnerAgent(std::string id, std::shared_ptr<Environment> environment)
        : AgentContainer(std::move(id), nullptr, environment, AgentType::HYBRID) {
        // Initialize competitive learner state
        body_.x = (std::rand() % 150) / 10.0f - 15.0f;
        body_.y = (std::rand() % 150) / 10.0f - 15.0f;
        body_.orientation = (std::rand() % 360) * 0.01745f;
        
        // Learning state
        learningState_ = LearningState::EXPLORATION;
        explorationRate_ = 0.3f;
        exploitationRate_ = 0.0f;
        knowledgeBase_.clear();
        competitorKnowledge_.clear();
        rewardHistory_.clear();
        
        // Competitive attributes
        competitiveness_ = 0.5f;
        cooperationTendency_ = 0.5f;
        reputation_ = 0.5f;
        alliancePartners_.clear();
        rivals_.clear();
        
        // Performance tracking
        totalLearning_ = 0;
        successfulCompetitions_ = 0;
        successfulCooperatives_ = 0;
    }
    
    void update(double timestep) override {
        double currentTime = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Sense environment
        sense();n
        // Update learning state based on performance
        updateLearningState();
        
        // Execute based on current state
        executeBehavior();
        
        // Learn from experience
        learnFromExperience();n
        // Update position
        body_.x += body_.velocityX * timestep * 100.0f;
        body_.y += body_.velocityY * timestep * 100.0f;
        
        // Clamp to world bounds
        body_.x = std::max(-100.0f, std::min(100.0f, body_.x));
        body_.y = std::max(-100.0f, std::min(100.0f, body_.y));
        
        body_.isMoving = (body_.velocityX != 0.0f || body_.velocityY != 0.0f);
        
        // Record competitive learning behavior
        std::string stateName = "Exploration";
        switch (learningState_) {
            case LearningState::EXPLOITATION: stateName = "Exploitation"; break;
            case LearningState::COOPERATION: stateName = "Cooperation"; break;
            case LearningState::COMPETITION: stateName = "Competition"; break;
        }
        
        recordBehavior("CompetitiveLearning_" + stateName, timestep,
                      "exploration=" + std::to_string(explorationRate_) +
                      ",exploit=" + std::to_string(exploitationRate_) +
                      ",reputation=" + std::to_string(reputation_));
    }
    
private:
    void updateLearningState() {
        double currentTime = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Update exploration and exploitation based on learning history
        if (learningState_ == LearningState::EXPLORATION) {
            explorationRate_ = std::min(1.0f, explorationRate_ * 1.05f);
            exploitationRate_ = std::max(0.0f, exploitationRate_ * 0.95f);
            
            // Switch to exploitation if confidence is high
            if (explorationRate_ > 0.8f && totalLearning_ > 10) {
                learningState_ = LearningState::EXPLOITATION;
            }
        } else if (learningState_ == LearningState::EXPLOITATION) {
            explorationRate_ = std::max(0.1f, explorationRate_ * 0.9f);
            exploitationRate_ = std::min(1.0f, exploitationRate_ * 1.05f);
            
            // Switch to cooperation if competitors are strong
            if (competitors_.size() > 2 && reputation_ < 0.3f) {
                learningState_ = LearningState::COOPERATION;
            } else if (exploitationRate_ > 0.8f && totalLearning_ > 20) {
                // Could switch to competition
                if (competitiveness_ > 0.7f) {
                    learningState_ = LearningState::COMPETITION;
                }
            }
        } else if (learningState_ == LearningState::COOPERATION) {
            // Maintain cooperation if beneficial
            if (alliancePartners_.size() > 0) {
                float avgPartnerReputation = 0.0f;
                for (const auto& partnerId : alliancePartners_) {
                    avgPartnerReputation += getReputation(partnerId);
                }
                avgPartnerReputation /= alliancePartners_.size();
                
                if (avgPartnerReputation > 0.6f && cooperationTendency_ > 0.3f) {
                    // Continue cooperation
                    exploitationRate_ = std::min(0.8f, exploitationRate_ * 1.1f);
                } else {
                    // May switch to competition
                    if (competitiveness_ > 0.6f) {
                        learningState_ = LearningState::COMPETITION;
                    }
                }
            } else {
                // No allies, may switch to competition
                if (competitiveness_ > 0.5f) {
                    learningState_ = LearningState::COMPETITION;
                }
            }
        } else if (learningState_ == LearningState::COMPETITION) {
            // Maintain competition if beneficial
            if (rivals_.size() > 0) {
                float totalRivalReputation = 0.0f;
                for (const auto& rivalId : rivals_) {
                    totalRivalReputation += getReputation(rivalId);
                }
                totalRivalReputation /= rivals_.size();
                
                if (totalRivalReputation > 0.7f && competitiveness_ > 0.3f) {
                    // Continue competition
                    explorationRate_ = std::min(0.8f, explorationRate_ * 1.1f);
                } else {
                    // May switch to cooperation
                    if (cooperationTendency_ > 0.3f) {
                        learningState_ = LearningState::COOPERATION;
                    }
                }
            } else {
                // No rivals, may switch to exploitation
                if (exploitationRate_ > 0.5f) {
                    learningState_ = LearningState::EXPLOITATION;
                }
            }
        }
    }
    
    void executeBehavior() {
        switch (learningState_) {
            case LearningState::EXPLORATION:
                exploration();
                break;
            case LearningState::EXPLOITATION:
                exploitation();
                break;
            case LearningState::COOPERATION:
                cooperation();
                break;
            case LearningState::COMPETITION:
                competition();
                break;
        }
    }
    
    void exploration() {
        // Explore new strategies and resources
        if (std::rand() % 100 < (explorationRate_ * 100)) {
            // Try a new action/behavior
            float angle = body_.orientation + ((std::rand() % 40) - 20) * 0.01745f;
            body_.velocityX = 1.5f * std::cos(angle);
            body_.velocityY = 1.5f * std::sin(angle);
            body_.orientation = angle;
            
            // Record learning
            totalLearning_++;
            std::string strategy = "exploration_" + std::to_string(totalLearning_);
            recordBehavior("Exploration", std::chrono::duration<double>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count(), strategy);
        }
        
        // Also gather information about environment
        gatherInformation();
    }
    
    void exploitation() {
        // Exploit known good strategies
        if (std::rand() % 100 < (exploitationRate_ * 100)) {
            // Use best known strategy
            float angle = body_.orientation;
            body_.velocityX = 1.5f * std::cos(angle);
            body_.velocityY = 1.5f * std::sin(angle);
            
            // Record exploitation
            totalLearning_++;
            std::string strategy = "exploitation_" + std::to_string(totalLearning_);
            recordBehavior("Exploitation", std::chrono::duration<double>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count(), strategy);
        }
    }
    
    void cooperation() {
        // Cooperate with allies
        if (!alliancePartners_.empty() && std::rand() % 100 < 60) {
            // Coordinate with allies
            std::string bestPartner = findBestAlly();
            if (!bestPartner.empty()) {
                // Receive strategy from ally
                std::string allyStrategy = getSharedMemory(bestPartner, "current_strategy");
                if (!allyStrategy.empty()) {
                    // Implement ally's strategy
                    float angle = body_.orientation;
                    body_.velocityX = 1.5f * std::cos(angle);
                    body_.velocityY = 1.5f * std::sin(angle);
                    
                    // Successfully cooperate
                    successfulCooperatives_++;
                    cooperationTendency_ = std::min(1.0f, cooperationTendency_ * 1.1f);
                    reputation_ = std::min(1.0f, reputation_ * 1.05f);
                    
                    // Record cooperation
                    std::string strategy = "cooperation_with_" + bestPartner;
                    recordBehavior("Cooperation", std::chrono::duration<double>
                        (std::chrono::high_resolution_clock::now().time_since_epoch()).count(), strategy);
                }
            }
        } else {
            // No allies, switch to exploration
            learningState_ = LearningState::EXPLORATION;
        }
    }
    
    void competition() {
        // Compete with rivals
        if (!rivals_.empty() && std::rand() % 100 < 70) {
            // Try to outperform rivals
            std::string strongestRival = findStrongestRival();
            if (!strongestRival.empty()) {
                // Adapt strategy to compete
                float angle = body_.orientation + ((std::rand() % 30) - 15) * 0.01745f;
                body_.velocityX = 1.5f * std::cos(angle);
                body_.velocityY = 1.5f * std::sin(angle);
                
                // Try to defeat rival
                bool victory = competeWithRival(strongestRival);
                if (victory) {
                    successfulCompetitions_++;
                    competitiveness_ = std::min(1.0f, competitiveness_ * 1.1f);
                    reputation_ = std::min(1.0f, reputation_ * 1.05f);
                    
                    // Record competition victory
                    std::string strategy = "defeated_" + strongestRival;
                    recordBehavior("Competition_Victory", std::chrono::duration<double>
                        (std::chrono::high_resolution_clock::now().time_since_epoch()).count(), strategy);
                } else {
                    // Learn from loss
                    learnFromLoss(strongestRival);
                }
            }
        } else {
            // No rivals, switch to exploitation
            learningState_ = LearningState::EXPLOITATION;
        }
    }
    
    void learnFromExperience() {
        // Learn from past experiences
        double currentTime = std::chrono::duration<double>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Track learning effectiveness
        float effectiveness = calculateLearningEffectiveness();
        
        if (effectiveness > 0.7f) {
            // Learning is working, maintain strategy
            explorationRate_ = std::min(0.8f, explorationRate_ * 1.1f);
        } else if (effectiveness < 0.3f) {
            // Learning is not working, explore more
            explorationRate_ = std::max(0.3f, explorationRate_ * 0.8f);
        }
        
        // Update reputation based on learning success
        reputation_ = (reputation_ * 0.8f) + (effectiveness * 0.2f);
    }
    
    void gatherInformation() {
        // Gather information about other agents
        auto observations = getObservations();
        
        for (const auto& obs : observations) {
            // Identify competitors and potential allies
            if (obs.observationType.find("agent_") != std::string::npos) {
                std::string otherAgent = obs.observationType.substr(6);
                
                // Evaluate other agent based on observation
                float otherReputation = evaluateAgent(obs);
                
                // Decide if they're competitor or potential ally
                if (otherReputation > 0.6f) {
                    // Strong agent, potential ally or rival
                    if (competitiveness_ > 0.6f) {
                        rivals_.push_back(otherAgent);
                    } else {
                        alliancePartners_.push_back(otherAgent);
                    }
                } else {
                    // Weak agent, potential ally
                    alliancePartners_.push_back(otherAgent);
                }
                
                // Learn from their behavior
                knowledgeBase_[otherAgent] = obs.observationType;
            }
        }
    }
    
    float evaluateAgent(const Observation& obs) {
        // Simple evaluation of other agent
        float score = 0.5f;
        
        // Based on observation type
        if (obs.observationType.find("successful") != std::string::npos) {
            score += 0.3f;
        } else if (obs.observationType.find("defeated") != std::string::npos) {
            score += 0.2f;
        }
        
        // Based on observation value
        score += obs.observationValue * 0.1f;
        
        return std::min(1.0f, score);
    }
    
    std::string findBestAlly() {
        std::string bestAlly = "";
        float bestReputation = 0.0f;
        
        for (const auto& allyId : alliancePartners_) {
            float reputation = getReputation(allyId);
            if (reputation > bestReputation) {
                bestReputation = reputation;
                bestAlly = allyId;
            }
        }
        
        return bestAlly;
    }
    
    std::string findStrongestRival() {
        std::string strongestRival = "";
        float strongestReputation = 0.0f;
        
        for (const auto& rivalId : rivals_) {
            float reputation = getReputation(rivalId);
            if (reputation > strongestReputation) {
                strongestReputation = reputation;
                strongestRival = rivalId;
            }
        }
        
        return strongestRival;
    }
    
    float getReputation(const std::string& agentId) {
        // Get reputation from memory sharing or calculation
        std::string repStr = getSharedMemory(agentId, "reputation");
        if (!repStr.empty()) {
            return std::stof(repStr);
        }
        
        // Default reputation calculation
        float agentReputation = 0.5f;
        if (knowledgeBase_.find(agentId) != knowledgeBase_.end()) {
            // Adjust based on knowledge
            if (knowledgeBase_[agentId].find("successful") != std::string::npos) {
                agentReputation += 0.2f;
            }
        }
        
        return std::min(1.0f, agentReputation);
    }
    
    bool competeWithRival(const std::string& rivalId) {
        // Try to compete with a specific rival
        float rivalReputation = getReputation(rivalId);
        
        // Chance of victory based on reputation difference
        float victoryChance = (reputation_ - rivalReputation) / 2.0f + 0.5f;
        victoryChance = std::max(0.1f, std::min(0.9f, victoryChance));
        
        bool victory = (std::rand() % 100) < (victoryChance * 100);
        
        if (victory) {
            // Share victory with allies
            for (const auto& allyId : alliancePartners_) {
                shareMemory(allyId, "victory", rivalId + "_defeated");
            }
        }
        
        return victory;
    }
    
    void learnFromLoss(const std::string& rivalId) {
        // Learn from losing to a rival
        float rivalReputation = getReputation(rivalId);
        
        // Adapt strategy
        if (rivalReputation > reputation_) {
            // Learn from better opponent
            shareMemory(rivalId, "learned_strategy", "adapt_to_beater");
            
            // Increase competitiveness
            competitiveness_ = std::min(1.0f, competitiveness_ * 1.1f);
        }
        
        // Update reputation down slightly
        reputation_ = std::max(0.1f, reputation_ * 0.9f);
    }
    
    float calculateLearningEffectiveness() {
        // Calculate how effective learning has been
        if (totalLearning_ == 0) return 0.0f;
        
        float totalSuccess = successfulCompetitions_ + successfulCooperatives_;
        return totalSuccess / static_cast<float>(totalLearning_);
    }
    
    // Agent attributes
    LearningState learningState_;
    float explorationRate_;
    float exploitationRate_;
    std::unordered_map<std::string, std::string> knowledgeBase_;
    std::unordered_map<std::string, std::string> competitorKnowledge_;
    std::vector<double> rewardHistory_;
    
    float competitiveness_;
    float cooperationTendency_;
    float reputation_;
    std::vector<std::string> alliancePartners_;
    std::vector<std::string> rivals_;
    
    int totalLearning_;
    int successfulCompetitions_;
    int successfulCooperatives_;
};

// Competitive Learning World
class CompetitiveLearningWorld : public Environment {
public:
    CompetitiveLearningWorld(int width = 200, int height = 200) : width_(width), height_(height) {
        NLM_LOG_INFO("CompetitiveLearningWorld created: " + std::to_string(width_) + "x" + std::to_string(height_));
        
        // Initialize with competitive dynamics
        learningNodes_ = generateLearningNodes();
    }
    
    std::vector<std::pair<float, float>> generateLearningNodes() {
        std::vector<std::pair<float, float>> nodes;
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
        for (int i = 0; i < 6; ++i) {
            float x = (std::rand() % 140) / 10.0f - 14.0f;
            float y = (std::rand() % 140) / 10.0f - 14.0f;
            nodes.push_back(std::make_pair(x, y));
        }
        
        return nodes;
    }
    
    std::unique_ptr<SensoryInput> reset() override {
        return std::make_unique<SensoryInput>();
    }
    
    std::unique_ptr<SensoryInput> step(const Action& action) override {
        return std::make_unique<SensoryInput>();
    }
    
    const EnvironmentState& getState() const override {
        static EnvironmentState state;
        return state;
    }
    
    float getLastReward() const override {
        return lastReward_;
    }
    
    bool isDone() const override {
        return done_;
    }
    
    const char* getName() const override {
        return "CompetitiveLearningWorld";
    }
    
    size_t getActionSpaceSize() const override {
        return 6;
    }
    
    size_t getObservationSpaceSize() const override {
        return 250;
    }

private:
    int width_;
    int height_;
    std::vector<std::pair<float, float>> learningNodes_;
    mutable EnvironmentState state_;
    float lastReward_ = 0.0f;
    bool done_ = false;
};

// Simple Competitive Learning Simulation
class CompetitiveLearningSimulation {
public:
    CompetitiveLearningSimulation() {
        NLM_LOG_INFO("CompetitiveLearningSimulation initialized");
    }
    
    void run(int numAgents = 20, int steps = 200) {
        std::cout << "===========================================\n";
        std::cout << "COMPETITIVE LEARNING SIMULATION\n";
        std::cout << "===========================================\n\n";
        
        // Create world
        CompetitiveLearningWorld world(200, 200);
        
        // Create agent manager
        AgentManager manager;
        
        // Create competitive learning agents
        std::vector<std::string> agentIds;
        for (int i = 0; i < numAgents; ++i) {
            std::string agentId = manager.createAgent(
                AgentType::HYBRID,
                std::make_shared<CompetitiveLearningWorld>(world)
            );
            
            if (!agentId.empty()) {
                agentIds.push_back(agentId);
                std::cout << "Created agent: " << agentId << "\n";
            }
        }
        
        // Start simulation
        manager.startSimulation(0.1);
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            std::cout << "--- Step " << step << " ---\n";
            
            // Run manager step
            manager.runStep();
            
            // Print statistics every 50 steps
            if (step % 50 == 0) {
                auto metrics = manager.getMetrics();
                std::cout << "  Total agents: " << metrics.activeAgents << "\n";
                std::cout << "  Total steps: " << metrics.totalSteps << "\n";
                std::cout << "  Average energy: " << metrics.averageEnergy << "\n";
            }
            
            // Print agent information
            std::cout << "  Agent status:\n";
            int count = 0;
            for (const auto& agentId : agentIds) {
                if (manager.hasAgent(agentId) && count < 3) {
                    auto& agent = manager.getAgent(agentId);
                    std::cout << "    " << agentId << ": (" 
                              << agent.getBody().x << ", " << agent.getBody().y << ")\n";
                    count++;
                }
            }
            
            std::cout << "\n";
        }
        
        // Stop simulation
        manager.stopSimulation();
        
        // Print final statistics
        auto finalMetrics = manager.getMetrics();
        std::cout << "===========================================\n";
        std::cout << "SIMULATION COMPLETE\n";
        std::cout << "===========================================\n";
        std::cout << "Final Statistics:\n";
        std::cout << "  Total simulation time: " << finalMetrics.totalTime << "\n";
        std::cout << "  Total steps: " << finalMetrics.totalSteps << "\n";
        std::cout << "  Active agents: " << finalMetrics.activeAgents << "\n";
        
        std::cout << "\nCompetitive learning simulation complete.\n";
        std::cout << "Agents balanced exploration, exploitation, cooperation, and competition.\n";
    }
};

} // namespace nlm