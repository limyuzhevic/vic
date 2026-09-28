#include "TrainingLoop.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <iomanip>
#include <sstream>

namespace nlm {

// ============= TRAINING LOOP CLASS IMPLEMENTATION =============

TrainingLoop::TrainingLoop(std::shared_ptr<AgentBrain> agent,
                          std::shared_ptr<SimpleWorld> environment,
                          std::shared_ptr<Config> config,
                          std::shared_ptr<Brain> policy,
                          std::shared_ptr<ReinforcementLearner> optimizer)
    : agent_(agent), environment_(environment), config_(config), policy_(policy),
      optimizer_(optimizer), episodeCount_(0), totalSteps_(0), currentDifficulty_(0),
      running_(false), episodeCountCallback_(nullptr), episodeCallback_(nullptr),
      stepCallback_(nullptr), trainingCompleteCallback_(nullptr) {
    
    // Initialize state processor and reward function
    stateProcessor_ = std::make_shared<DefaultStateProcessor>();
    rewardFunction_ = std::make_shared<DefaultRewardFunction>();
    
    // Set up random number generator
    std::random_device rd;
    rng_.seed(rd());
    
    // Initialize metrics
    metrics_.episodesCompleted = 0;
    metrics_.totalReward = 0.0;
}

TrainingLoop::~TrainingLoop() {
    stop();
}

bool TrainingLoop::runEpisode() {
    if (!running_) {
        throw NLMRuntimeError("Training loop is not running. Call start() first.");
    }
    
    try {
        // Reset environment and agent for new episode
        if (environment_) {
            environment_->reset();
        }
        if (agent_) {
            agent_->reset();
        }
        if (policy_) {
            policy_->reset();
        }
        
        double episodeStartTime = getCurrentTime();
        float totalReward = 0.0f;
        size_t episodeSteps = 0;
        int maxSteps = config_ ? config_->getOr<int>("max_steps_per_episode", 1000) : 1000;
        
        // Get initial state
        std::vector<float> state;
        if (stateProcessor_ && environment_ && agent_) {
            state = stateProcessor_->process(policy_, environment_, agent_);
        }
        
        // Episode loop
        for (size_t step = 0; step < maxSteps && running_; ++step) {
            episodeSteps = step;
            totalSteps_++;
            
            // Call step callback if set
            if (stepCallback_) {
                stepCallback_(step, state, totalReward);
            }
            
            // Get action from policy
            int action = -1;
            if (optimizer_) {
                action = optimizer_->predict(policy_, state);
            } else if (policy_) {
                // Direct policy-based action selection
                action = selectActionFromPolicy(state);
            }
            
            if (action < 0) {
                break; // Episode ended
            }
            
            // Apply action in environment
            float reward = 0.0f;
            bool done = false;
            std::vector<float> nextState;
            
            if (environment_) {
                auto actionResult = environment_->applyAction(static_cast<MotorCommand>(action), getCurrentTime());
                reward = actionResult.reward;
                
                // Compute reward using reward function
                if (rewardFunction_) {
                    reward = rewardFunction_->computeReward(policy_, environment_, agent_, state);
                }
                
                // Get next state
                if (stateProcessor_) {
                    nextState = stateProcessor_->process(policy_, environment_, agent_);
                }
                
                done = (environment_->getAgentBody().health <= 0.0f || 
                       step >= maxSteps - 1);
            }
            
            totalReward += reward;
            
            // Store experience in optimizer if available
            if (optimizer_) {
                TrainingExperience experience;
                experience.brainState = policy_;
                experience.worldState = environment_;
                experience.agent = agent_;
                experience.state = state;
                experience.action = action;
                experience.reward = reward;
                experience.nextState = nextState;
                experience.done = done;
                experience.timestamp = getCurrentTime();
                experience.step = totalSteps_;
                experience.difficultyLevel = currentDifficulty_;
                
                optimizer_->storeExperience(experience);
            }
            
            // Update current state
            state = nextState;
            
            // Call step callback if set
            if (stepCallback_) {
                stepCallback_(step, state, totalReward);
            }
            
            if (done) {
                break;
            }
        }
        
        double episodeDuration = getCurrentTime() - episodeStartTime;
        
        // Record episode metrics
        metrics_.episodeRewards.push_back(totalReward);
        metrics_.episodeLengths.push_back(static_cast<double>(episodeSteps));
        metrics_.totalReward += totalReward;
        metrics_.episodesCompleted++;
        
        // Update average reward
        metrics_.avgReward = metrics_.totalReward / metrics_.episodesCompleted;
        
        // Update best reward
        if (totalReward > metrics_.bestReward) {
            metrics_.bestReward = totalReward;
        }
        
        // Update average steps
        metrics_.avgSteps = metrics_.totalReward / metrics_.episodesCompleted;
        metrics_.trainingTime += episodeDuration;
        
        // Call episode callback if set
        if (episodeCallback_) {
            episodeCallback_(episodeCount_, totalReward, episodeSteps);
        }
        
        episodeCount_++;
        
        return true;
        
    } catch (const std::exception& e) {
        throw NLMRuntimeError(std::string("Error in runEpisode: ") + e.what());
    }
}

bool TrainingLoop::runTraining() {
    if (!running_) {
        running_ = true;
    }
    
    try {
        int maxEpisodes = config_ ? config_->getOr<int>("max_episodes", 100) : 100;
        bool saveCheckpoints = config_ ? config_->getOr<bool>("enable_checkpointing", true) : true;
        std::string checkpointPath = config_ ? config_->getOr<std::string>("checkpoint_path", "") : "";
        
        NLM_LOG_INFO("Starting training with " + std::to_string(maxEpisodes) + " episodes");
        
        for (int episode = 0; episode < maxEpisodes && running_; ++episode) {
            NLM_LOG_INFO("Episode " + std::to_string(episode + 1) + "/" + std::to_string(maxEpisodes));
            
            bool success = runEpisode();
            if (!success) {
                NLM_LOG_WARN("Episode " + std::to_string(episode + 1) + " failed, continuing");
            }
            
            // Perform optimization after each episode
            if (optimizer_ && episode % 1 == 0) { // Update every episode
                if (!updatePolicy()) {
                    NLM_LOG_WARN("Policy update failed in episode " + std::to_string(episode + 1));
                }
            }
            
            // Evaluate policy periodically
            if (episode % 10 == 0) {
                evaluatePolicy();
            }
            
            // Save checkpoint if requested
            if (saveCheckpoints && !checkpointPath.empty() && 
                (episode + 1) % 10 == 0) {
                if (policy_) {
                    policy_->save(checkpointPath + "_episode_" + 
                                 std::to_string(episode + 1) + ".bin");
                }
            }
            
            // Call training complete callback if all episodes completed
            if (episode == maxEpisodes - 1 && trainingCompleteCallback_) {
                trainingCompleteCallback_(metrics_);
            }
        }
        
        running_ = false;
        
        NLM_LOG_INFO("Training completed. Total episodes: " + 
                    std::to_string(metrics_.episodesCompleted) +
                    ", Best reward: " + std::to_string(metrics_.bestReward));
        
        return true;
        
    } catch (const std::exception& e) {
        running_ = false;
        throw NLMRuntimeError(std::string("Error in runTraining: ") + e.what());
    }
}

bool TrainingLoop::collectExperience() {
    // Experience collection is handled within runEpisode
    // and stored in the optimizer
    if (!optimizer_) {
        NLM_LOG_WARN("No optimizer available for experience collection");
        return false;
    }
    
    return optimizer_->canLearn();
}

bool TrainingLoop::updatePolicy() {
    if (!optimizer_) {
        NLM_LOG_WARN("No optimizer available for policy update");
        return false;
    }
    
    try {
        return optimizer_->learn(policy_);
    } catch (const std::exception& e) {
        throw NLMRuntimeError(std::string("Error in updatePolicy: ") + e.what());
    }
}

bool TrainingLoop::evaluatePolicy() {
    if (!optimizer_) {
        NLM_LOG_WARN("No optimizer available for policy evaluation");
        return false;
    }
    
    try {
        return optimizer_->evaluate(policy_);
    } catch (const std::exception& e) {
        throw NLMRuntimeError(std::string("Error in evaluatePolicy: ") + e.what());
    }
}

bool TrainingLoop::savePolicy(const std::string& filepath) {
    if (!policy_) {
        NLM_LOG_WARN("No policy available to save");
        return false;
    }
    
    try {
        return policy_->save(filepath);
    } catch (const std::exception& e) {
        throw NLMRuntimeError(std::string("Error in savePolicy: ") + e.what());
    }
}

bool TrainingLoop::loadPolicy(const std::string& filepath) {
    if (!policy_) {
        NLM_LOG_WARN("No policy available to load");
        return false;
    }
    
    try {
        return policy_->load(filepath);
    } catch (const std::exception& e) {
        throw NLMRuntimeError(std::string("Error in loadPolicy: ") + e.what());
    }
}

int TrainingLoop::selectActionFromPolicy(const std::vector<float>& state) {
    if (state.empty() || stateProcessor_->getActionSpaceSize() <= 0) {
        return 0;
    }
    
    // Simple epsilon-greedy action selection
    float epsilon = config_ ? config_->getOr<float>("exploration_rate", 0.1f) : 0.1f;
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    if (dist(rng_) < epsilon) {
        // Explore: random action
        std::uniform_int_distribution<int> actionDist(0, stateProcessor_->getActionSpaceSize() - 1);
        return actionDist(rng_);
    } else {
        // Exploit: use policy to select action
        return selectBestAction(state);
    }
}

int TrainingLoop::selectBestAction(const std::vector<float>& state) {
    // Simple heuristic-based action selection
    // This is a placeholder - should be replaced with actual policy-based selection
    int bestAction = 0;
    float bestValue = -1000.0f;
    
    // Get agent's current state for decision making
    if (environment_) {
        const auto& agentBody = environment_->getAgentBody();
        
        // Simple rules for action selection
        if (agentBody.energy < 0.3f) {
            bestAction = static_cast<int>(MotorCommand::Rest); // Rest if low energy
        } else if (agentBody.health < 0.5f) {
            bestAction = static_cast<int>(MotorCommand::MoveForward); // Move toward resources
        } else {
            bestAction = static_cast<int>(MotorCommand::MoveForward); // Default action
        }
    }
    
    return bestAction;
}

void TrainingLoop::start() {
    if (running_) {
        NLM_LOG_WARN("Training loop is already running");
        return;
    }
    
    running_ = true;
    NLM_LOG_INFO("Training loop started");
}

void TrainingLoop::stop() {
    running_ = false;
    NLM_LOG_INFO("Training loop stopped");
}

double TrainingLoop::getCurrentTime() const {
    return std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

std::string TrainingLoop::getMetricsSummary() const {
    std::stringstream ss;
    ss << "=== Training Metrics Summary ===\n";
    ss << "Episodes completed: " << metrics_.episodesCompleted << "\n";
    ss << "Total steps: " << totalSteps_ << "\n";
    ss << "Average reward: " << metrics_.avgReward << "\n";
    ss << "Best reward: " << metrics_.bestReward << "\n";
    ss << "Average episode length: " << metrics_.avgSteps << "\n";
    ss << "Training time: " << metrics_.trainingTime << " seconds\n";
    ss << "Current difficulty: " << currentDifficulty_ << "\n";
    ss << "Running: " << (running_ ? "Yes" : "No") << "\n";
    
    return ss.str();
}

void TrainingLoop::reset() {
    episodeCount_ = 0;
    totalSteps_ = 0;
    currentDifficulty_ = 0;
    
    // Reset metrics
    metrics_ = TrainingMetrics();
    
    if (optimizer_) {
        optimizer_->clearMemory();
    }
    
    NLM_LOG_INFO("Training loop reset");
}

TrainingLoop& TrainingLoop::setStateProcessor(std::shared_ptr<StateProcessor> processor) {
    stateProcessor_ = processor;
    return *this;
}

TrainingLoop& TrainingLoop::setRewardFunction(std::shared_ptr<RewardFunction> rewardFunc) {
    rewardFunction_ = rewardFunc;
    return *this;
}

TrainingLoop& TrainingLoop::setEpisodeCountCallback(std::function<void(size_t, float, size_t)> callback) {
    episodeCountCallback_ = callback;
    return *this;
}

TrainingLoop& TrainingLoop::setEpisodeCallback(std::function<void(size_t, float, size_t)> callback) {
    episodeCallback_ = callback;
    return *this;
}

TrainingLoop& TrainingLoop::setStepCallback(std::function<void(size_t, const std::vector<float>&, float)> callback) {
    stepCallback_ = callback;
    return *this;
}

TrainingLoop& TrainingLoop::setTrainingCompleteCallback(std::function<void(const TrainingMetrics&)> callback) {
    trainingCompleteCallback_ = callback;
    return *this;
}

AgentBrain* TrainingLoop::getAgent() const {
    return agent_.get();
}

SimpleWorld* TrainingLoop::getEnvironment() const {
    return environment_.get();
}

Config* TrainingLoop::getConfig() const {
    return config_.get();
}

Brain* TrainingLoop::getPolicy() const {
    return policy_.get();
}

ReinforcementLearner* TrainingLoop::getOptimizer() const {
    return optimizer_.get();
}

TrainingMetrics& TrainingLoop::getMetrics() {
    return metrics_;
}

const TrainingMetrics& TrainingLoop::getMetrics() const {
    return metrics_;
}

} // namespace nlm
