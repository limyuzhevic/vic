#pragma once

#include "CustomNeuromodulatorPlugin.hpp"
#include "Core/PluginInterface.hpp"
#include "Core/Types/Types.hpp"
#include "Brain/Brain.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {
namespace plugins {

class CustomNeuromodulatorPlugin : public Plugin {
public:
    CustomNeuromodulatorPlugin() : baselineDopamine_(1.0f), noveltyGain_(1.0f),
                                   curiosityBoost_(1.0f), predictionErrorSensitivity_(1.0f),
                                   enableActionValueLearning_(true) {}
    
    const std::string& getName() const override {
        static const std::string name = "CustomNeuromodulatorPlugin";
        return name;
    }
    
    const std::string& getVersion() const override {
        static const std::string version = "1.3.0";
        return version;
    }
    
    const std::string& getAuthor() const override {
        static const std::string author = "NLM Development Team";
        return author;
    }
    
    const std::string& getDescription() const override {
        static const std::string description = "Advanced neuromodulator system with adaptive gain control, novelty detection, and curiosity-driven exploration";
        return description;
    }
    
    bool initialize(std::shared_ptr<PluginManager> manager, 
                   std::shared_ptr<Config> config) override {
        pluginManager_ = manager;
        config_ = config;
        
        // Initialize random number generator
        std::random_device rd;
        rng_.seed(rd());
        
        // Apply configuration if available
        if (config_) {
            baselineDopamine_ = config_->getOr<float>("plugin_neuromodulator_baseline_dopamine", baselineDopamine_);
            noveltyGain_ = config_->getOr<float>("plugin_neuromodulator_novelty_gain", noveltyGain_);
            curiosityBoost_ = config_->getOr<float>("plugin_neuromodulator_curiosity_boost", curiosityBoost_);
            predictionErrorSensitivity_ = config_->getOr<float>("plugin_neuromodulator_prediction_error_sensitivity", predictionErrorSensitivity_);
            enableActionValueLearning_ = config_->getOr<bool>("plugin_neuromodulator_enable_action_value_learning", enableActionValueLearning_);
        }
        
        // Register neuromodulation hooks
        if (pluginManager_) {
            pluginManager_->executePluginFunction(getName(), "register_neuromodulation", {"dopamine", "novelty", "curiosity"});
        }
        
        return true;
    }
    
    void shutdown() override {
        if (pluginManager_) {
            pluginManager_->executePluginFunction(getName(), "unregister_neuromodulation", {});
        }
        
        noveltyHistory_.clear();
        curiosityHistory_.clear();
        actionValues_.clear();
    }
    
    void update(TimestepDuration dt) override {
        // Update neuromodulator levels
        currentTime_ += dt;
        
        // Update novelty detection
        updateNoveltyDetection(dt);
        
        // Update curiosity levels
        updateCuriosity(dt);
        
        // Update prediction error integration
        updatePredictionError(dt);
        
        // Update action values
        if (enableActionValueLearning_) {
            updateActionValues(dt);
        }
        
        // Apply neuromodulatory effects
        applyNeuromodulation(dt);
    }
    
    bool onPluginMessage(const std::string& message, 
                        const std::vector<std::string>& data) override {
        if (message == "reward_signal") {
            return handleRewardSignal(data);
        } else if (message == "prediction_error") {
            return handlePredictionError(data);
        } else if (message == "action_executed") {
            return handleActionExecuted(data);
        } else if (message == "get_neuromodulation_levels") {
            return provideNeuromodulationLevels(data);
        } else if (message == "get_action_values") {
            return provideActionValues(data);
        }
        return false;
    }
    
    std::string getPluginSpecificInfo() const override {
        json info;
        info["baseline_dopamine"] = baselineDopamine_;
        info["novelty_gain"] = noveltyGain_;
        info["curiosity_boost"] = curiosityBoost_;
        info["prediction_error_sensitivity"] = predictionErrorSensitivity_;
        info["enable_action_value_learning"] = enableActionValueLearning_;
        info["current_time"] = currentTime_;
        info["novelty_level"] = getNoveltyLevel();
        info["curiosity_level"] = getCuriosityLevel();
        info["dopamine_level"] = getDopamineLevel();
        info["prediction_error"] = getPredictionError();
        info["novelty_history_size"] = noveltyHistory_.size();
        info["action_values_count"] = actionValues_.size();
        
        std::stringstream ss;
        ss << info.dump(2);
        return ss.str();
    }
    
    bool requires(const std::string& dependency) const override {
        static const std::vector<std::string> dependencies = {"brain", "sensory", "action", "reward"};
        return std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end();
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"brain", "sensory", "action", "reward"};
    }
    
    // Neuromodulator control
    void setBaselineDopamine(float level) { baselineDopamine_ = std::max(0.0f, std::min(level, 2.0f)); }
    float getBaselineDopamine() const { return baselineDopamine_; }
    
    void setNoveltyGain(float gain) { noveltyGain_ = std::max(0.0f, std::min(gain, 2.0f)); }
    float getNoveltyGain() const { return noveltyGain_; }
    
    void setCuriosityBoost(float boost) { curiosityBoost_ = std::max(0.0f, std::min(boost, 3.0f)); }
    float getCuriosityBoost() const { return curiosityBoost_; }
    
    // Neuromodulator accessors
    float getDopamineLevel() const {
        // Add noise for biological realism
        std::normal_distribution<float> noise(0.0f, 0.05f);
        return baselineDopamine_ * (1.0f + noveltyGain_ * getNoveltyLevel()) * (1.0f + noise(rng_));
    }
    
    float getNoveltyLevel() const {
        if (noveltyHistory_.empty()) return 0.0f;
        
        // Calculate novelty as average of recent novelty scores
        float sum = 0.0f;
        size_t count = std::min(size_t(10), noveltyHistory_.size());
        for (size_t i = 0; i < count; ++i) {
            sum += noveltyHistory_[noveltyHistory_.size() - 1 - i];
        }
        
        return sum / count;
    }
    
    float getCuriosityLevel() const {
        if (curiosityHistory_.empty()) return 0.0f;
        
        // Curvature-based curiosity
        float avgCuriosity = 0.0f;
        size_t count = std::min(size_t(5), curiosityHistory_.size());
        for (size_t i = 0; i < count; ++i) {
            avgCuriosity += curiosityHistory_[curiosityHistory_.size() - 1 - i];
        }
        avgCuriosity /= count;
        
        return curiosityBoost_ * avgCuriosity * (1.0f + predictionError_ * predictionErrorSensitivity_);
    }
    
    float getPredictionError() const { return predictionError_; }
    
    // Action value methods
    void updateActionValue(const std::string& action, float reward, float predictionError) {
        auto it = actionValues_.find(action);
        if (it == actionValues_.end()) {
            actionValues_[action] = ActionValue{0.0f, 0.0f, 0.0f};
        }
        
        ActionValue& av = actionValues_[action];n        av.qValue += getDopamineLevel() * (reward - av.qValue) * 0.01f;
        av.predictionError = predictionError;
        av.lastUpdated = currentTime_;
    }
    
    float getActionValue(const std::string& action) const {
        auto it = actionValues_.find(action);
        if (it != actionValues_.end()) {
            return it->second.qValue;
        }
        return 0.0f;
    }
    
    std::string getBestAction() const {
        if (actionValues_.empty()) return "";
        
        std::string bestAction;
        float bestValue = std::numeric_limits<float>::lowest();
        
        for (const auto& pair : actionValues_) {
            if (pair.second.qValue > bestValue) {
                bestValue = pair.second.qValue;
                bestAction = pair.first;
            }
        }
        
        return bestAction;
    }
    
    // Novelty detection
    void registerNovelty(float noveltyScore) {
        noveltyHistory_.push_back(noveltyScore);
        if (noveltyHistory_.size() > 100) {
            noveltyHistory_.erase(noveltyHistory_.begin());
        }
    }
    
    // Curiosity computation
    float computeCuriosity(const std::string& state) {
        // Simple curiosity computation based on state entropy
        float entropy = calculateStateEntropy(state);
        curiosityHistory_.push_back(entropy);
        if (curiosityHistory_.size() > 50) {
            curiosityHistory_.erase(curiosityHistory_.begin());
        }
        
        return entropy;
    }
    
    // Helper functions
    float calculateStateEntropy(const std::string& state) const {
        // Very simplified entropy calculation
        std::unordered_map<char, int> charCounts;
        for (char c : state) {
            charCounts[c]++;
        }
        
        float entropy = 0.0f;
        for (const auto& pair : charCounts) {
            float p = static_cast<float>(pair.second) / static_cast<float>(state.size());
            if (p > 0.0f) {
                entropy -= p * std::log(p);
            }
        }
        
        return entropy;
    }
    
    void applyActionValueNoise() {
        // Add exploration noise to action values
        std::normal_distribution<float> noise(0.0f, 0.1f);
        
        for (auto& pair : actionValues_) {
            pair.second.qValue += noise(rng_) * 0.1f;
        }
    }
    
    float computeExplorationBonus() const {
        // UCT-style exploration bonus based on prediction error
        return curiosityBoost_ * std::sqrt(predictionError_);
    }

private:
    struct ActionValue {
        float qValue = 0.0f;
        float predictionError = 0.0f;
        float lastUpdated = 0.0f;
    };
    
    bool handleRewardSignal(const std::vector<std::string>& data) {
        if (data.size() < 1) return false;
        
        float reward = std::stof(data[0]);
        
        // Update dopamine with reward prediction error
        float expected = 0.0f; // Simplified expected reward
        float error = reward - expected;
        
        // Apply reward signal
        baselineDopamine_ += error * 0.1f;
        baselineDopamine_ = std::max(0.0f, std::min(baselineDopamine_, 2.0f));
        
        // Update action values
        if (enableActionValueLearning_) {
            // In real implementation, would know which action was taken
            // For now, just demonstrate the mechanism
            if (pluginManager_) {
                pluginManager_->executePluginFunction(
                    "ActionPlugin",
                    "reward_received",
                    {std::to_string(reward)}
                );
            }
        }
        
        return true;
    }
    
    bool handlePredictionError(const std::vector<std::string>& data) {
        if (data.size() < 1) return false;
        
        predictionError_ = std::stof(data[0]);
        return true;
    }
    
    bool handleActionExecuted(const std::vector<std::string>& data) {
        if (data.size() < 2) return false;
        
        std::string action = data[0];
        float reward = std::stof(data[1]);
        
        updateActionValue(action, reward, predictionError_);
        return true;
    }
    
    bool provideNeuromodulationLevels(const std::vector<std::string>& data) {
        json levels;
        levels["dopamine"] = getDopamineLevel();
        levels["novelty"] = getNoveltyLevel();
        levels["curiosity"] = getCuriosityLevel();
        levels["prediction_error"] = getPredictionError();
        
        if (pluginManager_) {
            pluginManager_->executePluginFunction(
                getName(),
                "neuromodulation_levels",
                {levels.dump()}
            );
        }
        
        return true;
    }
    
    bool provideActionValues(const std::vector<std::string>& data) {
        json values;
        for (const auto& pair : actionValues_) {
            values[pair.first] = pair.second.qValue;
        }
        
        if (pluginManager_) {
            pluginManager_->executePluginFunction(
                getName(),
                "action_values",
                {values.dump()}
            );
        }
        
        return true;
    }
    
    void updateNoveltyDetection(TimestepDuration dt) {
        // Novelty detection runs continuously
        // In real implementation, would analyze sensory input for novelty
        static float noveltyAccumulator = 0.0f;
        noveltyAccumulator += dt.count() * 0.001f; // Simulate novelty accumulation
        
        // Add some random novelty for demonstration
        std::uniform_real_distribution<float> dist(0.0f, 0.01f);
        registerNovelty(dist(rng_));
    }
    
    void updateCuriosity(TimestepDuration dt) {
        // Curiosity computation
        // Would normally analyze prediction errors and uncertainty
        static float curiosityAccumulator = 0.0f;
        curiosityAccumulator += dt.count() * 0.0005f;
        
        // Compute and store curiosity based on state changes
        std::string dummyState = "state_" + std::to_string(currentTime_);
        float curiosity = computeCuriosity(dummyState);
        curiosityHistory_.push_back(curiosity);
        if (curiosityHistory_.size() > 50) {
            curiosityHistory_.erase(curiosityHistory_.begin());
        }
    }
    
    void updatePredictionError(TimestepDuration dt) {
        // Update prediction error with decay
        predictionError_ *= (1.0f - dt.count() * 0.1f);
        
        // Add some noise for realism
        std::normal_distribution<float> noise(0.0f, 0.05f);
        predictionError_ += noise(rng_) * 0.01f;
        predictionError_ = std::max(0.0f, predictionError_);
    }
    
    void updateActionValues(TimestepDuration dt) {
        // Update action values with decay and exploration
        for (auto& pair : actionValues_) {
            ActionValue& av = pair.second;
            
            // Decay action values over time
            if (currentTime_ - av.lastUpdated > 1.0f) {
                av.qValue *= 0.95f;
            }
            
            // Add exploration noise periodically
            static float noiseTimer = 0.0f;
            noiseTimer += dt.count();
            if (noiseTimer > 0.5f) {
                applyActionValueNoise();
                noiseTimer = 0.0f;
            }
        }
    }
    
    void applyNeuromodulation(TimestepDuration dt) {
        // Apply neuromodulation effects to plasticity
        float totalNeuromodulation = getDopamineLevel() * 0.1f + 
                                     getNoveltyLevel() * noveltyGain_ * 0.05f +
                                     getCuriosityLevel() * curiosityBoost_ * 0.05f;
        
        // Apply to plasticity rules
        if (pluginManager_) {
            pluginManager_->executePluginFunction(
                "PlasticityPlugin",
                "apply_neuromodulation",
                {std::to_string(totalNeuromodulation)}
            );
        }
        
        // Update state
        neuromodulationAccumulator_ += totalNeuromodulation * dt.count();
    }
    
    float neuromodulationAccumulator_ = 0.0f;
    float predictionError_ = 0.0f;
    float currentTime_ = 0.0f;
    std::mt19937 rng_;
    std::vector<float> noveltyHistory_;
    std::vector<float> curiosityHistory_;
    std::unordered_map<std::string, ActionValue> actionValues_;
};

} // namespace plugins
} // namespace nlm

// Plugin factory function
extern "C" {
    std::shared_ptr<nlm::cli::Plugin> createPlugin() {
        return std::make_shared<nlm::plugins::CustomNeuromodulatorPlugin>();
    }
}
