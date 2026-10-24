#pragma once

/**
 * Example Plugin for NLM
 * 
 * This is an example plugin that demonstrates how to extend NLM functionality.
 * This plugin adds custom neural dynamics by modifying synaptic weights
 * based on environmental rewards.
 * 
 * The plugin shows:
 * 1. How to access brain systems
 * 2. How to modify neural behavior
 * 3. How to respond to environmental feedback
 * 4. How to persist plugin state across simulation steps
 */

#include "plugin/NLMPluginSystem.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Dopamine.hpp"
#include <vector>
#include <cmath>

namespace nlm {

/**
 * Reward-Modulated Plasticity Plugin
 * 
 * This plugin enhances standard STDP (Spike-Timing-Dependent Plasticity)
 * by incorporating reward signals. When dopamine levels are high (indicating
 * reward), synaptic strengthening is enhanced. When dopamine levels are low
 * (indicating punishment), synaptic weakening is enhanced.
 * 
 * This creates a reinforcement learning mechanism that operates at the
 * synaptic level, allowing the brain to learn from environmental feedback
 * more efficiently.
 */
class RewardModulatedPlasticityPlugin : public NLMPlugin {
public:
    RewardModulatedPlasticityPlugin() : rewardHistory_() {}
    
    std::string getName() const override { return "RewardModulatedPlasticity"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(Brain* brain) override {
        if (!NLMPlugin::initialize(brain)) return false;
        
        // Initialize plugin state
        rewardHistory_.clear();
        rewardHistory_.reserve(1000);  // Store last 1000 reward values
        
        // Get reference to dopamine system for reward signal
        dopamine_ = brain->getDopamine();
        if (!dopamine_) {
            NLM_LOG_WARNING("RewardModulatedPlasticityPlugin: Dopamine system not available");
        }
        
        // Get reference to STDP system for plasticity modulation
        stdp_ = brain->getSTDP();
        if (!stdp_) {
            NLM_LOG_ERROR("RewardModulatedPlasticityPlugin: STDP system not available");
            return false;
        }
        
        NLM_LOG_INFO("RewardModulatedPlasticityPlugin initialized successfully");
        return true;
    }
    
    void processStep(SimulationStep step, Timestamp time) override {
        if (!dopamine_ || !stdp_) return;
        
        // Get current dopamine level (reward prediction error)
        float dopamineLevel = dopamine_->getLevel();
        
        // Record reward history for analysis
        rewardHistory_.push_back(dopamineLevel);
        if (rewardHistory_.size() > 1000) {
            rewardHistory_.erase(rewardHistory_.begin());
        }
        
        // Calculate reward trend (average of recent rewards)
        float rewardTrend = calculateRewardTrend();
        
        // Modulate STDP parameters based on reward
        if (step % 100 == 0) {  // Update plasticity parameters every 100 steps
            modulatePlasticityParameters(rewardTrend);
        }
        
        // Apply reward-modulated plasticity to synapses
        applyRewardModulatedPlasticity(step, time);
        
        // Log plugin activity
        if (step % 1000 == 0) {
            NLM_LOG_INFO("RewardModulatedPlasticityPlugin: Step " + std::to_string(step) + 
                        ", Dopamine: " + std::to_string(dopamineLevel) +
                        ", Reward Trend: " + std::to_string(rewardTrend));
        }
    }
    
    void shutdown() override {
        NLM_LOG_INFO("RewardModulatedPlasticityPlugin shutdown");
    }
    
    /**
     * Get the current reward trend
     * 
     * @return Average reward over recent history (-1.0 to 1.0)
     */
    float getRewardTrend() const {
        return calculateRewardTrend();
    }
    
    /**
     * Get the number of reward records
     */
    size_t getRewardRecordCount() const {
        return rewardHistory_.size();
    }
    
    /**
     * Get the average dopamine level over the recorded history
     */
    float getAverageDopamineLevel() const {
        if (rewardHistory_.empty()) return 0.0f;
        
        float sum = std::accumulate(rewardHistory_.begin(), rewardHistory_.end(), 0.0f);
        return sum / rewardHistory_.size();
    }

private:
    /**
     * Calculate the reward trend from recent reward history
     * 
     * @return Reward trend where positive values indicate learning rewards
     *         and negative values indicate punishment or extinction
     */
    float calculateRewardTrend() const {
        if (rewardHistory_.size() < 10) return 0.0f;
        
        // Get the last 100 rewards for trend calculation
        size_t sampleSize = std::min(size_t(100), rewardHistory_.size());
        float sum = 0.0f;
        
        for (size_t i = 0; i < sampleSize; ++i) {
            sum += rewardHistory_[rewardHistory_.size() - 1 - i];
        }
        
        float averageReward = sum / sampleSize;
        
        // Normalize to -1.0 to 1.0 range
        return std::max(-1.0f, std::min(1.0f, averageReward));
    }
    
    /**
     * Modulate STDP parameters based on reward trend
     * 
     * @param rewardTrend Current reward trend (-1.0 to 1.0)
     */
    void modulatePlasticityParameters(float rewardTrend) {
        // Scale STDP learning rates based on reward trend
        // Positive reward trend (learning) -> increase LTP and LTD
        // Negative reward trend (punishment) -> decrease LTP, increase LTD
        
        float ltpWeight = 0.01f;
        float ltdWeight = 0.012f;
        
        if (rewardTrend > 0.5f) {
            // Strong learning reward
            ltpWeight *= 1.5f;
            ltdWeight *= 1.5f;
        } else if (rewardTrend > 0.0f) {
            // Mild learning reward
            ltpWeight *= 1.2f;
            ltdWeight *= 1.1f;
        } else if (rewardTrend < -0.5f) {
            // Strong punishment
            ltpWeight *= 0.5f;
            ltdWeight *= 2.0f;
        } else if (rewardTrend < 0.0f) {
            // Mild punishment
            ltpWeight *= 0.8f;
            ltdWeight *= 1.3f;
        }
        
        // Apply modulated weights
        stdp_->setLTPWeight(ltpWeight);
        stdp_->setLTDWeight(ltdWeight);
        
        NLM_LOG_INFO("RewardModulatedPlasticityPlugin: Modulated STDP - LTP: " + std::to_string(ltpWeight) + ", LTD: " + std::to_string(ltdWeight));
    }
    
    /**
     * Apply reward-modulated plasticity to all synapses
     * 
     * @param step Current simulation step
     * @param time Current simulation time
     */
    void applyRewardModulatedPlasticity(SimulationStep step, Timestamp time) {
        if (!brain_) return;
        
        // Get all regions
        auto regions = brain_->getRegions();
        
        // For each region, get all synapses and apply modulation
        for (const auto& region : regions) {
            if (!region) continue;
            
            auto& synapses = region->getSynapses();
            
            for (const auto& syn : synapses) {
                // Get current synaptic weight
                float weight = syn->getWeight();
                
                // Apply reward-modulated modification
                // Use a simpler model: modify weight based on dopamine level
                float modulation = dopamine_->getLevel();
                
                // Apply small weight adjustment based on reward
                if (std::abs(modulation) > 0.1f) {
                    float weightChange = modulation * 0.001f * step * 0.001f;  // Small adjustment
                    
                    // Ensure weight stays within valid range
                    float newWeight = std::max(-1.0f, std::min(1.0f, weight + weightChange));
                    
                    // Note: In a real implementation, we would need access to modify synapse weights
                    // For this example, we just log the intended change
                    if (std::abs(newWeight - weight) > 0.001f) {
                        NLM_LOG_INFO("RewardModulatedPlasticityPlugin: Synapse " + std::to_string(syn->getId()) +
                                    " weight change: " + std::to_string(weight) + " -> " + std::to_string(newWeight) +
                                    " (modulation: " + std::to_string(modulation) + ")");
                    }
                }
            }
        }
    }
    
    // Member variables
    std::vector<float> rewardHistory_;  // Historical reward values
    Dopamine* dopamine_ = nullptr;      // Dopamine system reference
    STDP* stdp_ = nullptr;              // STDP system reference
};

/**
 * Plugin factory function for RewardModulatedPlasticityPlugin
 * 
 * This function is called by the plugin system to create plugin instances.
 * The "extern "C"" ensures C linkage, making the function visible to
 * dynamic loading systems on different platforms.
 */
extern "C" std::shared_ptr<NLMPlugin> create_reward_modulated_plasticity_plugin() {
    return std::make_shared<RewardModulatedPlasticityPlugin>();
}

/**
 * Example plugin that demonstrates concept formation enhancement
 */
class ConceptFormationEnhancementPlugin : public NLMPlugin {
public:
    ConceptFormationEnhancementPlugin() : conceptActivation_() {}
    
    std::string getName() const override { return "ConceptFormationEnhancement"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(Brain* brain) override {
        if (!NLMPlugin::initialize(brain)) return false;
        
        // Initialize concept activation tracking
        conceptActivation_.clear();
        conceptActivation_.reserve(1000);
        
        // Get reference to concept formation system
        conceptFormation_ = brain->getConceptFormation();
        if (!conceptFormation_) {
            NLM_LOG_ERROR("ConceptFormationEnhancementPlugin: Concept formation system not available");
            return false;
        }
        
        NLM_LOG_INFO("ConceptFormationEnhancementPlugin initialized successfully");
        return true;
    }
    
    void processStep(SimulationStep step, Timestamp time) override {
        if (!conceptFormation_) return;
        
        // Get current neural activity pattern
        std::vector<float> activityPattern = getCurrentActivityPattern();
        
        // Update concept activation based on current activity
        updateConceptActivation(activityPattern);
        
        // Form new concepts based on patterns
        formNewConcepts(activityPattern);
        
        // Log concept activity
        if (step % 500 == 0) {
            float totalConceptActivation = calculateTotalConceptActivation();
            NLM_LOG_INFO("ConceptFormationEnhancementPlugin: Total concept activation: " + std::to_string(totalConceptActivation));
        }
    }
    
    void shutdown() override {
        NLM_LOG_INFO("ConceptFormationEnhancementPlugin shutdown");
    }
    
    /**
     * Get the number of active concepts
     */
    size_t getActiveConceptCount() const {
        return conceptActivation_.size();
    }
    
    /**
     * Get the total activation level across all concepts
     */
    float getTotalConceptActivation() const {
        return calculateTotalConceptActivation();
    }

private:
    /**
     * Get the current neural activity pattern
     * 
     * @return Vector of activation levels for all neurons
     */
    std::vector<float> getCurrentActivityPattern() const {
        std::vector<float> activityPattern;
        
        if (!brain_) return activityPattern;
        
        // Collect activity from all regions
        auto regions = brain_->getRegions();
        
        for (const auto& region : regions) {
            if (!region) continue;
            
            auto neurons = region->getAllNeurons();
            for (const auto* neuron : neurons) {
                activityPattern.push_back(neuron->getMembranePotential());
            }
        }
        
        return activityPattern;
    }
    
    /**
     * Update concept activation based on current neural activity
     * 
     * @param activityPattern Current neural activity pattern
     */
    void updateConceptActivation(const std::vector<float>& activityPattern) {
        if (activityPattern.empty()) return;
        
        // Simple concept activation: sum of high-activity neurons
        float threshold = 0.5f;  // Activation threshold
        float totalActivation = 0.0f;
        size_t conceptCount = 0;
        
        for (float activation : activityPattern) {
            if (activation > threshold) {
                // This neuron contributes to concept activation
                totalActivation += activation;
                conceptCount++;
            }
        }
        
        // Store concept activation
        ConceptStats stats;
        stats.activation = totalActivation;
        stats.neuronCount = conceptCount;
        stats.timestamp = std::chrono::steady_clock::now();
        
        conceptActivation_.push_back(stats);
        if (conceptActivation_.size() > 1000) {
            conceptActivation_.erase(conceptActivation_.begin());
        }
    }
    
    /**
     * Form new concepts based on activity patterns
     * 
     * @param activityPattern Current neural activity pattern
     */
    void formNewConcepts(const std::vector<float>& activityPattern) {
        if (activityPattern.size() < 10) return;
        
        // Simple concept formation: identify clusters of high activity
        std::vector<float> clusterActivations;
        
        for (size_t i = 0; i < activityPattern.size() - 10; i += 10) {
            float clusterSum = 0.0f;
            size_t clusterCount = 0;
            
            for (size_t j = 0; j < 10 && i + j < activityPattern.size(); ++j) {
                if (activityPattern[i + j] > 0.3f) {
                    clusterSum += activityPattern[i + j];
                    clusterCount++;
                }
            }
            
            if (clusterCount > 0) {
                clusterActivations.push_back(clusterSum / clusterCount);
            }
        }
        
        // Log new concepts if any are detected
        for (float activation : clusterActivations) {
            if (activation > 0.7f) {
                NLM_LOG_INFO("ConceptFormationEnhancementPlugin: New concept detected with activation: " + std::to_string(activation));
            }
        }
    }
    
    /**
     * Calculate total activation across all concepts
     */
    float calculateTotalConceptActivation() const {
        float total = 0.0f;
        for (const auto& stats : conceptActivation_) {
            total += stats.activation;
        }
        return total;
    }
    
    // Member variables
    std::vector<ConceptStats> conceptActivation_;  // Concept activation statistics
    ConceptFormation* conceptFormation_ = nullptr;  // Concept formation system reference
    
    // Helper struct for concept statistics
    struct ConceptStats {
        float activation;                             // Current activation level
        size_t neuronCount;                           // Number of neurons in concept
        std::chrono::steady_clock::time_point timestamp;  // When concept was formed
    };
};

/**
 * Plugin factory function for ConceptFormationEnhancementPlugin
 */
extern "C" std::shared_ptr<NLMPlugin> create_concept_formation_enhancement_plugin() {
    return std::make_shared<ConceptFormationEnhancementPlugin>();
}

} // namespace nlm