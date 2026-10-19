// NLM Enhanced AgentBrain Implementation
// Provides comprehensive agent functionality with advanced features

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>

namespace nlm {

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    stepsExecuted_ = 0;
    totalReward_ = 0.0f;
    actionCounts_.clear();
    actionEfficiencies_.clear();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - include all 8 options including Wait
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;  // Added missing case
            }
        }
    }
    
    return defaultCmd;
}

ActionResult AgentBrain::executeAction(const MotorCommand& command) {
    if (!actionCallback_) {
        // Default action execution
        ActionResult result;
        result.success = false;
        result.message = "No action callback registered";
        return result;
    }
    
    return actionCallback_(command);
}

ActionResult AgentBrain::executeAction(const std::string& actionName) {
    // Convert string to motor command
    MotorCommand cmd = MotorCommand::Wait;
    if (actionName == "MoveForward") cmd = MotorCommand::MoveForward;
    else if (actionName == "MoveBackward") cmd = MotorCommand::MoveBackward;
    else if (actionName == "TurnLeft") cmd = MotorCommand::TurnLeft;
    else if (actionName == "TurnRight") cmd = MotorCommand::TurnRight;
    else if (actionName == "LookLeft") cmd = MotorCommand::LookLeft;
    else if (actionName == "LookRight") cmd = MotorCommand::LookRight;
    else if (actionName == "Interact") cmd = MotorCommand::Interact;
    else if (actionName == "Wait") cmd = MotorCommand::Wait;
    
    return executeAction(cmd);
}

SensoryPercept AgentBrain::observe() const {
    SensoryPercept percept;
    // This would normally get observation from world
    // For now, return empty percept
    return percept;
}

void AgentBrain::updateSensoryExpectations() {
    // Update internal models of sensory input
    // This is part of predictive processing
    if (curiosityEnabled_ && predictionError_ > 0.1f) {
        // Update internal models based on prediction errors
        // This is where learning occurs
    }
}

void AgentBrain::provideReward(float reward) {
    provideIntrinsicReward(reward);
    if (rewardCallback_) {
        rewardCallback_(reward, expectedReward_);
    }
}

void AgentBrain::provideIntrinsicReward(float reward) {
    totalReward_ += reward;
    stepsExecuted_++;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Apply reward modulation
    if (rewardModulationEnabled_) {
        applyRewardModulation(reward, expectedReward_);
    }
}

void AgentBrain::applyCreditAssignment(float reward, float predictedReward) {
    // Apply reward-modulated weight changes
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::learnFromExperience(const std::vector<float>& experience) {
    if (!brain_ || !metaLearningEnabled_) return;
    
    // Learn from experience patterns
    // This is where meta-learning occurs
    // The agent can learn how to learn from the experience
    
    // Example: update learning rate based on experience complexity
    float complexity = 0.0f;
    for (float val : experience) {
        complexity += std::abs(val);
    }
    complexity /= experience.size();
    
    if (complexity > 0.5f) {
        // High complexity - increase learning rate temporarily
        float tempLearningRate = learningRate_ * 2.0f;
        learningRate_ = std::min(tempLearningRate, 0.1f);  // Cap at 0.1
    }
}

void AgentBrain::consolidateMemory() {
    if (!brain_ || !selfMonitoringEnabled_) return;
    
    // Consolidate memory and prune unimportant connections
    // This is part of the agent's self-monitoring system
    
    // Find synapses with low eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            if (std::abs(eligibility) < 0.01f) {
                // Prune this synapse (reduce weight toward zero)
                float delta = -syn->getWeight() * 0.01f;
                syn->addToWeight(delta);
            }
        }
    }
}

void AgentBrain::resetForLearning() {
    if (!brain_) return;
    
    // Reset for new learning episode
    reset();
    
    // Reset development system
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset performance monitoring
    stepsExecuted_ = 0;
    totalReward_ = 0.0f;
    actionCounts_.clear();
    actionEfficiencies_.clear();
    
    // Reset memory systems
    // This would involve more complex memory management
    
    // Reset learning state
    learningRate_ = 0.01f;
    explorationRate_ = 0.1f;
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
}

std::string AgentBrain::serializeState() const {
    std::stringstream ss;
    ss << "AGENT_STATE:" << std::endl;
    ss << "  developmental_age: " << developmentalAge_ << std::endl;
    ss << "  plasticity_modifier: " << plasticityModifier_ << std::endl;
    ss << "  learning_rate: " << learningRate_ << std::endl;
    ss << "  exploration_rate: " << explorationRate_ << std::endl;
    ss << "  curiosity_level: " << curiosityLevel_ << std::endl;
    ss << "  novelty_level: " << noveltyLevel_ << std::endl;
    ss << "  prediction_error: " << predictionError_ << std::endl;
    ss << "  expected_reward: " << expectedReward_ << std::endl;
    ss << "  total_reward: " << totalReward_ << std::endl;
    ss << "  steps_executed: " << stepsExecuted_ << std::endl;
    ss << "  meta_learning_enabled: " << (metaLearningEnabled_ ? "true" : "false") << std::endl;
    ss << "  transfer_learning_enabled: " << (transferLearningEnabled_ ? "true" : "false") << std::endl;
    ss << "  self_monitoring_enabled: " << (selfMonitoringEnabled_ ? "true" : "false") << std::endl;
    
    return ss.str();
}

bool AgentBrain::deserializeState(const std::string& stateData) {
    // Parse state data
    // This is a simplified implementation
    // In practice, you would use a proper serialization format
    
    // For now, just parse key-value pairs
    size_t pos = 0;
    while (pos < stateData.size()) {
        size_t newlinePos = stateData.find('\n', pos);
        if (newlinePos == std::string::npos) break;
        
        std::string line = stateData.substr(pos, newlinePos - pos);
        pos = newlinePos + 1;
        
        // Parse key-value pair
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Parse value based on key
        if (key == "  developmental_age") {
            developmentalAge_ = std::stod(value);
        } else if (key == "  plasticity_modifier") {
            plasticityModifier_ = std::stof(value);
        } else if (key == "  learning_rate") {
            learningRate_ = std::stof(value);
        } else if (key == "  exploration_rate") {
            explorationRate_ = std::stof(value);
        } else if (key == "  curiosity_level") {
            curiosityLevel_ = std::stof(value);
        } else if (key == "  novelty_level") {
            noveltyLevel_ = std::stof(value);
        } else if (key == "  prediction_error") {
            predictionError_ = std::stof(value);
        } else if (key == "  expected_reward") {
            expectedReward_ = std::stof(value);
        } else if (key == "  total_reward") {
            totalReward_ = std::stof(value);
        } else if (key == "  steps_executed") {
            stepsExecuted_ = std::stoi(value);
        } else if (key == "  meta_learning_enabled") {
            metaLearningEnabled_ = (value == "true" || value == "1");
        } else if (key == "  transfer_learning_enabled") {
            transferLearningEnabled_ = (value == "true" || value == "1");
        } else if (key == "  self_monitoring_enabled") {
            selfMonitoringEnabled_ = (value == "true" || value == "1");
        }
    }
    
    return true;
}

bool AgentBrain::saveStateToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << serializeState();
    return true;
}

bool AgentBrain::loadStateFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return deserializeState(buffer.str());
}

void AgentBrain::startPerformanceMonitoring() {
    performanceMonitoringActive_ = true;
}

void AgentBrain::stopPerformanceMonitoring() {
    performanceMonitoringActive_ = false;
    // Calculate average step time
    if (stepsExecuted_ > 0) {
        averageStepTime_ = totalCPUTime_ / stepsExecuted_;
    }
}

double AgentBrain::getAverageStepTime() const {
    return averageStepTime_;
}

size_t AgentBrain::getStepsExecuted() const {
    return stepsExecuted_;
}

size_t AgentBrain::getActionCount(MotorCommand cmd) const {
    auto it = actionCounts_.find(cmd);
    return it != actionCounts_.end() ? it->second : 0;
}

float AgentBrain::getActionEfficiency(MotorCommand cmd) const {
    auto it = actionEfficiencies_.find(cmd);
    return it != actionEfficiencies_.end() ? it->second : 0.0f;
}

std::string AgentBrain::getDebugInfo() const {
    std::stringstream ss;
    ss << "=== NLM Agent Brain Debug Info ===" << std::endl;
    ss << "Steps executed: " << stepsExecuted_ << std::endl;
    ss << "Total reward: " << totalReward_ << std::endl;
    ss << "Average reward: " << (stepsExecuted_ > 0 ? totalReward_ / stepsExecuted_ : 0.0f) << std::endl;
    ss << "Learning rate: " << learningRate_ << std::endl;
    ss << "Exploration rate: " << explorationRate_ << std::endl;
    ss << "Curiosity level: " << curiosityLevel_ << std::endl;
    ss << "Novelty level: " << noveltyLevel_ << std::endl;
    ss << "Prediction error: " << predictionError_ << std::endl;
    ss << "Expected reward: " << expectedReward_ << std::endl;
    ss << "Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
    ss << "Meta learning enabled: " << (metaLearningEnabled_ ? "yes" : "no") << std::endl;
    ss << "Transfer learning enabled: " << (transferLearningEnabled_ ? "yes" : "no") << std::endl;
    ss << "Self monitoring enabled: " << (selfMonitoringEnabled_ ? "yes" : "no") << std::endl;
    ss << "Memory capacity: " << memoryCapacity_ << std::endl;
    ss << "Planning horizon: " << planningHorizon_ << std::endl;
    ss << "Average step time: " << averageStepTime_ << " ms" << std::endl;
    
    return ss.str();
}

void AgentBrain::printStatus() const {
    std::cout << getDebugInfo() << std::endl;
}

void AgentBrain::printPerformanceStats() const {
    std::cout << "=== NLM Performance Stats ===" << std::endl;
    std::cout << "Steps executed: " << stepsExecuted_ << std::endl;
    std::cout << "Total reward: " << totalReward_ << std::endl;
    std::cout << "Average reward per step: " << (stepsExecuted_ > 0 ? totalReward_ / stepsExecuted_ : 0.0f) << std::endl;
    std::cout << "Average step time: " << averageStepTime_ << " ms" << std::endl;
    std::cout << "Learning rate: " << learningRate_ << std::endl;
    std::cout << "Exploration rate: " << explorationRate_ << std::endl;
    std::cout << "Curiosity level: " << curiosityLevel_ << std::endl;
    std::cout << "Novelty level: " << noveltyLevel_ << std::endl;
}

void AgentBrain::printLearningStats() const {
    std::cout << "=== NLM Learning Stats ===" << std::endl;
    std::cout << "Meta learning enabled: " << (metaLearningEnabled_ ? "yes" : "no") << std::endl;
    std::cout << "Transfer learning enabled: " << (transferLearningEnabled_ ? "yes" : "no") << std::endl;
    std::cout << "Self monitoring enabled: " << (selfMonitoringEnabled_ ? "yes" : "no") << std::endl;
    std::cout << "Memory capacity: " << memoryCapacity_ << std::endl;
    std::cout << "Planning horizon: " << planningHorizon_ << std::endl;
    
    std::cout << "Action statistics:" << std::endl;
    for (const auto& entry : actionCounts_) {
        std::cout << "  " << static_cast<int>(entry.first) << ": " << entry.second << " times" << std::endl;
    }
}

} // namespace nlm