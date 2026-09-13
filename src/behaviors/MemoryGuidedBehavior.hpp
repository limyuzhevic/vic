#pragma once

#include "../agent/AgentBrain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// MemoryGuidedBehavior: Uses episodic memory to influence behavior selection
class MemoryGuidedBehavior : public Behavior {
public:
    MemoryGuidedBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Retrieve relevant episodic memories
        retrieveRelevantMemories();
        
        // Select action based on memory influence
        return selectActionFromMemory();
    }
    
    std::string getName() const override { return "MemoryGuided"; }
    
    // Store current experience in episodic memory
    void recordExperience(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward,
                         float energy = 1.0f) {
        EpisodicMemoryItem item;
        
        // Generate simple timestamp from current state
        item.timestamp = 0; // Would use simulation clock in full implementation
        item.action = action;
        item.reward = reward;
        item.energy = energy;
        item.novelty = 0.0f; // Would compute from state difference
        item.beforeState = beforeState;
        item.afterState = afterState;
        
        // Store in episodic memory if available
        if (memory_) {
            memory_->storeEpisode(item);
        }
        
        // Update confidence based on learning
        updateConfidence(0.1f);
    }
    
    // Search for similar past experiences
    std::vector<const EpisodicMemoryItem*> findSimilarExperiences(
        const std::vector<float>& currentState,
        ActionType preferredAction = ActionType::Wait,
        size_t maxResults = 5) const {
        std::vector<const EpisodicMemoryItem*> similar;
        
        if (!memory_) return similar;
        
        // Retrieve similar episodes from memory
        auto allEpisodes = memory_->retrieveSimilar(currentState, maxResults * 2);
        
        for (const auto* episode : allEpisodes) {
            // Check if episode matches preferred action
            if (preferredAction == ActionType::Wait || episode->action == preferredAction) {
                // Compute similarity between current state and episode's before state
                float similarity = computeSimilarity(currentState, episode->beforeState);
                
                if (similarity > 0.6f) { // Threshold for considering similar enough
                    similar.push_back(episode);
                    if (similar.size() >= maxResults) break;
                }
            }
        }
        
        return similar;
    }
    
    // Get best action recommendation from memory patterns
    MotorCommand recommendAction(const std::vector<float>& currentState,
                                ActionType lastAction = ActionType::Wait,
                                float explorationFactor = 0.1f) const {
        // Find similar past experiences
        auto similar = findSimilarExperiences(currentState, lastAction, 3);
        
        if (similar.empty()) {
            return MotorCommand::Wait;
        }
        
        // Count occurrences of each action in similar experiences
        std::unordered_map<ActionType, int> actionCounts;
        float totalConfidence = 0.0f;
        
        for (const auto* episode : similar) {
            ActionType action = episode->action;
            actionCounts[action]++;
            
            // Weight by reward and confidence
            float weight = (episode->reward + 1.0f) * 0.5f; // Normalize reward to 0-1 range
            totalConfidence += weight;
        }
        
        // Select most frequent action, with exploration randomness
        ActionType bestAction = ActionType::Wait;
        int bestCount = 0;
        
        for (const auto& pair : actionCounts) {
            if (pair.second > bestCount) {
                bestCount = pair.second;
                bestAction = pair.first;
            }
        }
        
        // Add exploration - occasionally choose random action
        float exploreRoll = 1.0f - explorationFactor;
        if (exploreRoll < 0.0f) exploreRoll = 0.0f;
        
        if (exploreRoll > 0.0f) {
            // Generate random choice between memory-based and random action
            float r = (agentBrain_ && agentBrain_->getBrain()) ? 
                agentBrain_->getBrain()->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
            
            if (r < (1.0f - exploreRoll)) {
                // Choose random action from available actions
                std::vector<ActionType> availableActions = {
                    ActionType::MoveForward, ActionType::MoveBackward,
                    ActionType::TurnLeft, ActionType::TurnRight,
                    ActionType::Interact, ActionType::Wait
                };
                
                if (!availableActions.empty()) {
                    int choice = (agentBrain_ && agentBrain_->getBrain()) ?
                        agentBrain_->getBrain()->getRandomGenerator()->uniformInt(0, availableActions.size()) : 0;
                    bestAction = availableActions[choice];
                }
            }
        }
        
        // Convert ActionType to MotorCommand
        MotorCommand cmd;
        switch (bestAction) {
            case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
            case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
            case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
            case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
            case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
            case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
            case ActionType::Interact: cmd = MotorCommand::Interact; break;
            case ActionType::Wait: cmd = MotorCommand::Wait; break;
            default: cmd = MotorCommand::Wait;
        }
        
        return cmd;
    }
    
    // Get memory influence score (0-1)
    float getMemoryInfluence() const {
        if (!memory_) return 0.0f;
        
        float episodeCount = static_cast<float>(memory_->getEpisodeCount());
        float influence = std::min(1.0f, episodeCount / 50.0f); // Saturate at 50 episodes
        
        // Boost influence if agent has good confidence
        influence *= (getConfidence() + 0.1f);
        
        return influence;
    }

private:
    float computeSimilarity(const std::vector<float>& state1,
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
    
    void retrieveRelevantMemories() {
        if (!agentBrain_ || !memory_) return;
        
        // In a full implementation, this would actively retrieve and process
        // relevant episodic memories based on current state and goals
    }
    
    MotorCommand selectActionFromMemory() {
        if (!agentBrain_) return MotorCommand::Wait;
        
        // Get current sensory state as pattern
        auto state = agentBrain_->getSensoryInputSize() > 0 ?
            std::vector<float>(agentBrain_->getSensoryInputSize(), 0.0f) : 
            std::vector<float>();
        
        // Use last action if available
        ActionType lastAction = ActionType::Wait;
        
        return recommendAction(state, lastAction, 0.2f);
    }
};

// SocialLearningBehavior: Implements agent-to-agent learning and imitation
class SocialLearningBehavior : public Behavior {
public:
    SocialLearningBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Process social observations
        processSocialObservations(timestep);
        
        // Select action based on social learning
        return selectSocialAction();
    }
    
    std::string getName() const override { return "SocialLearning"; }
    
    // Record observation of another agent's action and effect
    void recordObservation(ActionType observedAction,
                          const std::vector<float>& observerState,
                          const std::vector<float>& resultingState,
                          float reward = 0.0f) {
        SocialObservation obs;
        obs.action = observedAction;
        obs.observerState = observerState;
        obs.resultingState = resultingState;
        obs.reward = reward;
        obs.timestamp = observationCount_++;
        
        observations_.push_back(obs);
        
        // Keep memory bounded
        if (observations_.size() > 1000) {
            observations_.erase(observations_.begin());
        }
        
        // Update confidence based on learning
        updateConfidence(0.05f);
        
        // Check if we can imitate this action
        if (canImitate(observedAction)) {
            // Add to imitation pool
            if (std::find(imitationActions_.begin(), imitationActions_.end(), observedAction) == imitationActions_.end()) {
                imitationActions_.push_back(observedAction);
            }
        }
    }
    
    // Can we imitate this action based on our observations?
    bool canImitate(ActionType observedAction) const {
        // Check if we've seen this action before
        for (const auto& obs : observations_) {
            if (obs.action == observedAction) {
                return true;
            }
        }
        return false;
    }
    
    // Learn simple communication signal from another agent
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                 float signalReward) {
        CommunicationSignal sig;
        sig.pattern = signalPattern;
        sig.reward = signalReward;
        sig.confidence = 0.5f;
        
        communicationSignals_.push_back(sig);
        
        // Update confidence
        updateConfidence(0.1f);
    }
    
    // Detect if another agent is signaling
    bool detectSignal(const std::vector<float>& neuralPattern) const {
        if (communicationSignals_.empty()) return false;
        
        for (const auto& sig : communicationSignals_) {
            float similarity = computeSimilarity(neuralPattern, sig.pattern);
            if (similarity > 0.7f) { // Threshold for signal detection
                return true;
            }
        }
        return false;
    }
    
    // Get learned signal pattern
    std::vector<float> getSignalPattern() const {
        if (communicationSignals_.empty()) return std::vector<float>();
        
        // Return the signal with highest confidence
        float bestConfidence = 0.0f;
        std::vector<float> bestPattern;
        
        for (const auto& sig : communicationSignals_) {
            if (sig.confidence > bestConfidence) {
                bestConfidence = sig.confidence;
                bestPattern = sig.pattern;
            }
        }
        
        return bestPattern;
    }
    
    // Get signal meaning (associated reward)
    float getSignalMeaning() const {
        if (communicationSignals_.empty()) return 0.0f;
        
        // Return average signal reward
        float totalReward = 0.0f;
        for (const auto& sig : communicationSignals_) {
            totalReward += sig.reward;
        }
        
        return totalReward / communicationSignals_.size();
    }
    
    // Update social knowledge based on interactions
    void updateSocialKnowledge(float interactionReward) {
        // Update value of observed actions based on interaction outcome
        for (auto& obs : observations_) {
            // Adjust reward estimate based on interaction outcome
            obs.reward = obs.reward * 0.9f + interactionReward * 0.1f;
        }
        
        // Update communication signal values
        for (auto& sig : communicationSignals_) {
            sig.reward = sig.reward * 0.95f + interactionReward * 0.05f;
            sig.confidence = std::min(1.0f, sig.confidence + 0.02f);
        }
        
        // Update confidence based on social learning
        updateConfidence(0.03f * interactionReward);
    }
    
    // Get imitation action based on current state
    ActionType getImitationAction(const std::vector<float>& currentState) const {
        ActionType bestAction = ActionType::Wait;
        float bestScore = 0.0f;
        
        for (const auto& obs : observations_) {
            float similarity = computeSimilarity(currentState, obs.observerState);
            
            // Score by similarity and reward
            float score = similarity * (obs.reward + 1.0f) * 0.5f;
            
            if (score > bestScore) {
                bestScore = score;
                bestAction = obs.action;
            }
        }
        
        return bestAction;
    }
    
    // Get number of observations
    size_t getObservationCount() const { return observations_.size(); }

private:
    struct SocialObservation {
        ActionType action;
        std::vector<float> observerState;
        std::vector<float> resultingState;
        float reward;
        size_t timestamp;
    };
    
    struct CommunicationSignal {
        std::vector<float> pattern;
        float reward;
        float confidence;
    };
    
    std::vector<SocialObservation> observations_;
    std::vector<CommunicationSignal> communicationSignals_;
    std::vector<ActionType> imitationActions_;
    size_t observationCount_ = 0;
    
    float computeSimilarity(const std::vector<float>& state1,
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
    
    void processSocialObservations(double timestep) {
        // In a full implementation, this would process and consolidate
        // social observations to extract patterns and learned behaviors
    }
    
    MotorCommand selectSocialAction() {
        if (!agentBrain_) return MotorCommand::Wait;
        
        // Check if we're observing or interacting
        if (observationCount_ > 0 && observationCount_ < 10) {
            // Early observation phase - use imitation
            ActionType imitationAction = getImitationAction(
                std::vector<float>(agentBrain_->getSensoryInputSize(), 0.0f)
            );
            
            if (imitationAction != ActionType::Wait) {
                // Convert to MotorCommand
                MotorCommand cmd;
                switch (imitationAction) {
                    case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                    case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                    case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                    case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                    case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
                    case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
                    case ActionType::Interact: cmd = MotorCommand::Interact; break;
                    case ActionType::Wait: cmd = MotorCommand::Wait; break;
                    default: cmd = MotorCommand::Wait;
                }
                return cmd;
            }
        }
        
        return MotorCommand::Wait;
    }
};

// MetaCognitionBehavior: Implements self-monitoring and confidence estimation
class MetaCognitionBehavior : public Behavior {
public:
    MetaCognitionBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Update metacognitive awareness
        updateMetacognition(timestep);
        
        // Select action based on metacognitive assessment
        return selectMetacognitiveAction();
    }
    
    std::string getName() const override { return "MetaCognition"; }
    
    // Record prediction outcome for confidence calibration
    void recordPredictionOutcome(ActionType predictedAction,
                               ActionType actualAction,
                               float predictedReward,
                               float actualReward,
                               const std::vector<float>& state) {
        PredictionRecord record;
        record.predictedAction = predictedAction;
        record.actualAction = actualAction;
        record.predictedReward = predictedReward;
        record.actualReward = actualReward;
        record.state = state;
        record.timestamp = predictionRecordCount_++;
        
        // Compute prediction error
        record.rewardPredictionError = actualReward - predictedReward;
        
        // Compute action selection error
        record.actionPredictionError = (actualAction == predictedAction) ? 0.0f : 1.0f;
        
        predictionRecords_.push_back(record);
        
        // Update confidence based on prediction accuracy
        float accuracy = (actualAction == predictedAction) ? 1.0f : 0.0f;
        updateConfidence(accuracy * 0.2f);
        
        // Update metacognitive confidence for this action
        updateActionConfidence(predictedAction, accuracy * 0.5f);
    }
    
    // Get confidence in action selection for a given action
    float getActionConfidence(ActionType action) const {
        auto it = actionConfidence_.find(action);
        if (it != actionConfidence_.end()) {
            return it->second;
        }
        return 0.5f; // Default confidence
    }
    
    // Get overall metacognitive confidence
    float getMetacognitiveConfidence() const {
        if (predictionRecords_.empty()) return 0.5f;
        
        float totalConfidence = 0.0f;
        for (const auto& record : predictionRecords_) {
            totalConfidence += getActionConfidence(record.predictedAction);
        }
        
        return totalConfidence / predictionRecords_.size();
    }
    
    // Get prediction calibration (how well confidence matches accuracy)
    float getPredictionCalibration() const {
        if (predictionRecords_.empty()) return 0.5f;
        
        float totalCalibration = 0.0f;
        for (const auto& record : predictionRecords_) {
            float confidence = getActionConfidence(record.predictedAction);
            float accuracy = (record.actualAction == record.predictedAction) ? 1.0f : 0.0f;
            
            // Calibration = accuracy - confidence (should be near 0 for well-calibrated)
            totalCalibration += accuracy - confidence;
        }
        
        return totalCalibration / predictionRecords_.size();
    }
    
    // Get number of prediction records
    size_t getPredictionRecordCount() const { return predictionRecords_.size(); }
    
    // Get self-monitoring score (awareness of one's own uncertainty)
    float getSelfMonitoringScore() const {
        float totalMonitoring = 0.0f;
        size_t count = 0;
        
        for (const auto& record : predictionRecords_) {
            float confidence = getActionConfidence(record.predictedAction);
            
            // Self-monitoring = 1 - confidence (higher when uncertain)
            totalMonitoring += 1.0f - confidence;
            count++;
        }
        
        return count > 0 ? totalMonitoring / count : 0.5f;
    }
    
    // Reset metacognitive state
    void reset() {
        predictionRecords_.clear();
        actionConfidence_.clear();
        predictionRecordCount_ = 0;
        confidenceCalibration_ = 0.5f;
        updateConfidence(-0.1f);
    }

private:
    struct PredictionRecord {
        ActionType predictedAction;
        ActionType actualAction;
        float predictedReward;
        float actualReward;
        float rewardPredictionError;
        float actionPredictionError;
        std::vector<float> state;
        size_t timestamp;
    };
    
    std::vector<PredictionRecord> predictionRecords_;
    std::unordered_map<ActionType, float> actionConfidence_;
    size_t predictionRecordCount_ = 0;
    float confidenceCalibration_ = 0.5f;
    
    void updateMetacognition(double timestep) {
        // Update metacognitive awareness based on prediction history
        if (predictionRecords_.size() > 10) {
            // Analyze prediction accuracy over recent history
            size_t recentCount = std::min<size_t>(predictionRecords_.size(), 20);
            
            float totalAccuracy = 0.0f;
            for (size_t i = 0; i < recentCount; ++i) {
                size_t idx = predictionRecords_.size() - 1 - i;
                float accuracy = (predictionRecords_[idx].actualAction == predictionRecords_[idx].predictedAction) ? 1.0f : 0.0f;
                totalAccuracy += accuracy;
            }
            
            float recentAccuracy = totalAccuracy / recentCount;
            
            // Adjust overall confidence based on recent accuracy
            float delta = (recentAccuracy - 0.5f) * 0.1f;
            updateConfidence(delta);
            
            // Update calibration based on pattern of over/under-confidence
            float calibration = getPredictionCalibration();
            confidenceCalibration_ = confidenceCalibration_ * 0.9f + calibration * 0.1f;
        }
    }
    
    void updateActionConfidence(ActionType action, float delta) {
        actionConfidence_[action] += delta;
        actionConfidence_[action] = std::clamp(actionConfidence_[action], 0.0f, 1.0f);
    }
    
    MotorCommand selectMetacognitiveAction() {
        if (!agentBrain_) return MotorCommand::Wait;
        
        // Check if we have good metacognitive confidence
        float metacognitiveConfidence = getMetacognitiveConfidence();
        
        if (metacognitiveConfidence > 0.7f) {
            // High confidence - use default motor decoding
            return MotorCommand::Wait; // Would return from agentBrain_ in full implementation
        }
        else if (metacognitiveConfidence < 0.3f) {
            // Low confidence - increase exploration
            float exploreChance = 0.5f - metacognitiveConfidence * 0.3f;
            
            float r = (agentBrain_ && agentBrain_->getBrain()) ?
                agentBrain_->getBrain()->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
            
            if (r < exploreChance) {
                // Choose random action for exploration
                std::vector<ActionType> availableActions = {
                    ActionType::MoveForward, ActionType::MoveBackward,
                    ActionType::TurnLeft, ActionType::TurnRight,
                    ActionType::Interact, ActionType::Wait
                };
                
                if (!availableActions.empty()) {
                    int choice = (agentBrain_ && agentBrain_->getBrain()) ?
                        agentBrain_->getBrain()->getRandomGenerator()->uniformInt(0, availableActions.size()) : 0;
                    
                    // Convert to MotorCommand
                    MotorCommand cmd;
                    switch (availableActions[choice]) {
                        case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                        case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                        case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                        case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                        case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
                        case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
                        case ActionType::Interact: cmd = MotorCommand::Interact; break;
                        case ActionType::Wait: cmd = MotorCommand::Wait; break;
                        default: cmd = MotorCommand::Wait;
                    }
                    return cmd;
                }
            }
        }
        
        // Medium confidence - use a more cautious approach
        // Check self-monitoring - if low, we're uncertain
        float selfMonitoring = getSelfMonitoringScore();
        
        if (selfMonitoring > 0.7f) {
            // High self-monitoring - we're aware of uncertainty, explore more
            return MotorCommand::Wait; // Would use curiosity in full implementation
        }
        
        // Default action
        return MotorCommand::Wait;
    }
};

// CuriosityDrivenBehavior: Enhanced exploration based on uncertainty
class CuriosityDrivenBehavior : public Behavior {
public:
    CuriosityDrivenBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Update curiosity based on prediction errors and novelty
        updateCuriosity(timestep);
        
        // Select action based on curiosity-driven exploration
        return selectCuriosityDrivenAction();
    }
    
    std::string getName() const override { return "CuriosityDriven"; }
    
    // Update curiosity level based on prediction errors and novelty
    void updateCuriosity(double timestep) {
        // Base curiosity on prediction error and novelty
        float predictionErrorCuriosity = std::abs(getPredictionError()) * 0.5f;
        
        // In a full implementation, this would access agent's curiosity state
        // For now, use a simple approximation
        
        // Add uncertainty-based curiosity
        float uncertainty = 1.0f - getConfidence();
        
        // Time-based exploration bonus
        float timeBonus = 0.0f;
        if (curiosityTimer_ > curiosityCooldown_) {
            timeBonus = 0.2f * std::min(1.0f, (curiosityTimer_ - curiosityCooldown_) / 10.0f);
            curiosityTimer_ = 0.0;
        }
        
        // Update curiosity level
        float newCuriosity = predictionErrorCuriosity * 0.4f + uncertainty * 0.4f + timeBonus * 0.2f;
        newCuriosity = std::clamp(newCuriosity, 0.0f, 1.0f);
        
        // Decay curiosity over time
        currentCuriosity_ = newCuriosity * curiosityDecay_;
        
        // Update timer
        curiosityTimer_ += timestep;
        
        // Update confidence based on exploration
        if (currentCuriosity_ > 0.7f) {
            updateConfidence(0.05f * timestep);
        }
    }
    
    // Get current curiosity level
    float getCuriosityLevel() const {
        return currentCuriosity_;
    }
    
    // Set curiosity parameters
    void setCuriosityParameters(float decayRate, float cooldownTime) {
        curiosityDecay_ = decayRate;
        curiosityCooldown_ = cooldownTime;
    }
    
    // Get exploration bonus for a given state-action pair
    float getExplorationBonus(const std::vector<float>& state, ActionType action) const {
        // Curiosity-driven exploration based on uncertainty
        float uncertainty = 1.0f - getConfidence();
        
        // Bonus for novel state-action combinations
        float noveltyBonus = 0.0f;
        
        // Check if we've seen this state-action combination before
        bool seenBefore = false;
        for (const auto& experienced : experiencedActions_) {
            if (experienced.state == state && experienced.action == action) {
                seenBefore = true;
                break;
            }
        }
        
        if (!seenBefore) {
            // Novel combination gets exploration bonus
            noveltyBonus = 0.5f * (1.0f - uncertainty); // Higher when less certain
        }
        
        // Bonus based on curiosity level
        float curiosityBonus = currentCuriosity_ * 0.3f * (1.0f - uncertainty);
        
        // Temporal difference bonus (explore different at different times)
        float temporalBonus = 0.0f;
        if (curiosityTimer_ > curiosityCooldown_) {
            temporalBonus = 0.1f * (curiosityTimer_ / (curiosityCooldown_ * 2.0f));
        }
        
        return noveltyBonus + curiosityBonus + temporalBonus;
    }
    
    // Record that we've experienced a state-action combination
    void recordExperience(const std::vector<float>& state, ActionType action) {
        ExperiencedAction exp;
        exp.state = state;
        exp.action = action;
        exp.timestamp = experienceCount_++;
        
        experiencedActions_.push_back(exp);
        
        // Keep memory bounded
        if (experiencedActions_.size() > 500) {
            experiencedActions_.erase(experiencedActions_.begin());
        }
    }
    
    // Get number of experienced actions
    size_t getExperienceCount() const { return experiencedActions_.size(); }

private:
    struct ExperiencedAction {
        std::vector<float> state;
        ActionType action;
        size_t timestamp;
    };
    
    std::vector<ExperiencedAction> experiencedActions_;
    size_t experienceCount_ = 0;
    
    // Curiosity dynamics
    float currentCuriosity_ = 0.0f;
    float curiosityDecay_ = 0.95f;
    float curiosityCooldown_ = 10.0f;
    double curiosityTimer_ = 0.0;
    
    float getPredictionError() const {
        if (agentBrain_) {
            return agentBrain_->getPredictionError();
        }
        return 0.0f;
    }
    
    MotorCommand selectCuriosityDrivenAction() {
        if (!agentBrain_) return MotorCommand::Wait;
        
        // High curiosity -> more exploration
        if (currentCuriosity_ > 0.8f) {
            // Max exploration - choose random action
            std::vector<ActionType> availableActions = {
                ActionType::MoveForward, ActionType::MoveBackward,
                ActionType::TurnLeft, ActionType::TurnRight,
                ActionType::Interact, ActionType::Wait
            };
            
            if (!availableActions.empty()) {
                int choice = (agentBrain_ && agentBrain_->getBrain()) ?
                    agentBrain_->getBrain()->getRandomGenerator()->uniformInt(0, availableActions.size()) : 0;
                
                // Convert to MotorCommand
                MotorCommand cmd;
                switch (availableActions[choice]) {
                    case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                    case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                    case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                    case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                    case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
                    case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
                    case ActionType::Interact: cmd = MotorCommand::Interact; break;
                    case ActionType::Wait: cmd = MotorCommand::Wait; break;
                    default: cmd = MotorCommand::Wait;
                }
                return cmd;
            }
        }
        
        else if (currentCuriosity_ > 0.5f) {
            // Medium curiosity - explore options that have exploration bonuses
            std::vector<std::pair<ActionType, float>> scoredActions;
            
            std::vector<ActionType> availableActions = {
                ActionType::MoveForward, ActionType::MoveBackward,
                ActionType::TurnLeft, ActionType::TurnRight,
                ActionType::Interact, ActionType::Wait
            };
            
            // Get current state
            auto state = agentBrain_->getSensoryInputSize() > 0 ?
                std::vector<float>(agentBrain_->getSensoryInputSize(), 0.0f) : 
                std::vector<float>();
            
            // Score each action by exploration bonus
            for (const auto& action : availableActions) {
                float bonus = getExplorationBonus(state, action);
                scoredActions.push_back({action, bonus});
            }
            
            // Select action with highest exploration bonus
            if (!scoredActions.empty()) {
                std::sort(scoredActions.begin(), scoredActions.end(),
                         [](const auto& a, const auto& b) { return a.second > b.second; });
                
                ActionType bestAction = scoredActions[0].first;
                
                // With some probability, choose lower-scoring action to ensure exploration
                float r = (agentBrain_ && agentBrain_->getBrain()) ?
                    agentBrain_->getBrain()->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
                
                if (r < 0.2f && scoredActions.size() > 1) {
                    // Choose from bottom 20% of actions
                    int startIdx = std::max(0, static_cast<int>(scoredActions.size() * 0.8f));
                    int choice = (agentBrain_ && agentBrain_->getBrain()) ?
                        agentBrain_->getBrain()->getRandomGenerator()->uniformInt(startIdx, scoredActions.size()) : startIdx;
                    bestAction = scoredActions[choice].first;
                }
                
                // Convert to MotorCommand
                MotorCommand cmd;
                switch (bestAction) {
                    case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                    case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                    case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                    case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                    case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
                    case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
                    case ActionType::Interact: cmd = MotorCommand::Interact; break;
                    case ActionType::Wait: cmd = MotorCommand::Wait; break;
                    default: cmd = MotorCommand::Wait;
                }
                return cmd;
            }
        }
        
        // Low curiosity - mostly follow standard policy
        return MotorCommand::Wait;
    }
};

} // namespace nlm