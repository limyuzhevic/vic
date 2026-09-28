#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <mutex>
#include <functional>

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../experiments/ExperimentMetrics.hpp"

namespace nlm {

// Forward declarations
class TrainingLoop;
class ReinforcementLearner;
class CurriculumLearner;
class SimulationScheduler;

// Training experience structure for experience replay
struct TrainingExperience {
    std::shared_ptr<Brain> brainState;
    std::shared_ptr<SimpleWorld> worldState;
    std::shared_ptr<AgentBrain> agent;
    
    // State information
    std::vector<float> state;
    int action;
    float reward;
    std::vector<float> nextState;
    bool done;
    
    // Metadata
    double timestamp;
    size_t step;
    int difficultyLevel;
    
    TrainingExperience() : action(-1), reward(0.0f), done(false), 
                          timestamp(0.0), step(0), difficultyLevel(0) {}
};

// Training metrics for monitoring
struct TrainingMetrics {
    std::vector<double> episodeRewards;
    std::vector<double> episodeLengths;
    std::vector<double> lossValues;
    std::vector<double> explorationRates;
    double totalReward;
    double avgReward;
    double bestReward;
    double avgSteps;
    size_t episodesCompleted;
    double trainingTime;
    
    TrainingMetrics() : totalReward(0.0), avgReward(0.0), bestReward(-1000.0),
                       avgSteps(0.0), episodesCompleted(0), trainingTime(0.0) {}
};

// Reward function interface
class RewardFunction {
public:
    virtual ~RewardFunction() = default;
    virtual float computeReward(const std::shared_ptr<Brain>& brain,
                              const std::shared_ptr<SimpleWorld>& world,
                              const std::shared_ptr<AgentBrain>& agent,
                              const std::vector<float>& state) = 0;
    virtual void update(const TrainingExperience& experience) {}
};

// Default reward function implementation
class DefaultRewardFunction : public RewardFunction {
public:
    float computeReward(const std::shared_ptr<Brain>& brain,
                      const std::shared_ptr<SimpleWorld>& world,
                      const std::shared_ptr<AgentBrain>& agent,
                      const std::vector<float>& state) override {
        if (!world) return 0.0f;
        
        float baseReward = world->computeReward(brain.get());
        
        // Add neuromodulation-based rewards
        float novelty = 0.0f;
        float curiosity = 0.0f;
        if (agent) {
            novelty = agent->getNoveltyLevel();
            curiosity = agent->getCuriosityLevel();
        }
        
        return baseReward + novelty * 0.1f + curiosity * 0.05f;
    }
};

// State processor interface
class StateProcessor {
public:
    virtual ~StateProcessor() = default;
    virtual std::vector<float> process(const std::shared_ptr<Brain>& brain,
                                     const std::shared_ptr<SimpleWorld>& world,
                                     const std::shared_ptr<AgentBrain>& agent) = 0;
    virtual int getActionSpaceSize() const = 0;
    virtual std::vector<float> getState() const { return std::vector<float>(); }
};

// Default state processor implementation
class DefaultStateProcessor : public StateProcessor {
public:
    std::vector<float> process(const std::shared_ptr<Brain>& brain,
                             const std::shared_ptr<SimpleWorld>& world,
                             const std::shared_ptr<AgentBrain>& agent) override {
        std::vector<float> state;
        
        if (world) {
            const auto& agentBody = world->getAgentBody();
            state.push_back(agentBody.x);
            state.push_back(agentBody.y);
            state.push_back(agentBody.orientation);
            state.push_back(agentBody.energy);
            state.push_back(agentBody.health);
            
            // Add vision information if available
            const auto& percept = world->getSensoryPercept();
            const auto& vision = percept.getVision();
            if (!vision.empty()) {
                for (float pixel : vision) {
                    state.push_back(pixel);
                }
            }
        }
        
        // Add brain statistics
        if (brain) {
            state.push_back(static_cast<float>(brain->getActiveNeuronCount()) / 
                           std::max(1.0f, static_cast<float>(brain->getTotalNeuronCount())));
            state.push_back(static_cast<float>(brain->getAverageFiringRate()));
        }
        
        return state;
    }
    
    int getActionSpaceSize() const override {
        // Based on ActionType enum (16 possible actions)
        return 16;
    }
};

} // namespace nlm
