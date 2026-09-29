// Enhanced plasticity rule abstractions
#pragma once

#include "PlasticityRule.hpp"
#include "src/core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Plasticity rule base class with enhanced features
class EnhancedPlasticityRule : public PlasticityRule {
public:
    EnhancedPlasticityRule() : PlasticityRule(), lastUpdateTime_(0.0) {}
    virtual ~EnhancedPlasticityRule() = default;
    
    // Enhanced update with timing and neuromodulation support
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, TimestepDuration dt) override {
        if (!synapse || !isEnabled()) {
            return;
        }
        
        // Validate input spike trains
        if (preSpikes.empty() && postSpikes.empty()) {
            return; // No spikes to process
        }
        
        // Calculate elapsed time since last update
        double currentTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        double timeSinceLastUpdate = currentTime - lastUpdateTime_;
        
        // Apply enhanced plasticity computation
        applyPlasticity(synapse, preSpikes, postSpikes, dt, timeSinceLastUpdate);
        
        // Update timing
        lastUpdateTime_ = currentTime;
    }
    
    // Enhanced weight application with bounds checking
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) {
            return;
        }
        
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + delta;
        
        // Apply bounds based on synapse type and configuration
        float minWeight = -1.0f;  // Can be negative for inhibitory synapses
        float maxWeight = 1.0f;
        
        // Apply bounds with hysteresis to prevent oscillation
        if (newWeight > maxWeight * 0.999f) {  // Allow small margin
            newWeight = maxWeight;
        } else if (newWeight < minWeight * 0.999f) {
            newWeight = minWeight;
        }
        
        // Apply smoothing to prevent abrupt changes
        float smoothingFactor = 0.1f; // Configurable
        newWeight = currentWeight * (1.0f - smoothingFactor) + newWeight * smoothingFactor;
        
        synapse->setWeight(newWeight);
    }
    
protected:
    virtual void applyPlasticity(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes, TimestepDuration dt,
                                double timeSinceLastUpdate) = 0;
    
    double getLastUpdateTime() const { return lastUpdateTime_; }
    
private:
    double lastUpdateTime_;
};

// Advanced STDP implementation with biological accuracy
class AdvancedSTDP : public EnhancedPlasticityRule {
public:
    AdvancedSTDP() {
        configure(0.01f, 0.012f, 20.0f); // Default values
    }
    
    void configure(float ltpWeight, float ltdWeight, float tau) override {
        ltpWeight_ = std::max(0.0f, ltpWeight);
        ltdWeight_ = std::max(0.0f, ltdWeight);
        tau_ = std::max(1.0f, tau);
        
        // Calculate learning rate normalization factor
        learningRateNorm_ = std::max(0.1f, 1.0f / (ltpWeight_ + ltdWeight_));
    }
    
    void applyPlasticity(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes, TimestepDuration dt,
                        double timeSinceLastUpdate) override {
        if (!synapse) return;
        
        // Get synaptic information
        float currentWeight = synapse->getWeight();
        Timestamp synapseCreationTime = getSynapseCreationTime(synapse); // Need to add this
        
        // Calculate spike time differences
        float weightChange = 0.0f;
        
        // Process pre-post spike pairs for LTP
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float delta_t = static_cast<float>(postTime - preTime);
                
                if (delta_t > 0 && delta_t < tau_) {
                    // LTP: post fires after pre (causal)
                    float contribution = ltpWeight_ * std::exp(-delta_t / tau_);
                    weightChange += contribution * learningRateNorm_;
                } else if (delta_t < 0 && std::abs(delta_t) < tau_) {
                    // LTD: pre fires after post (anti-causal)
                    float contribution = ltdWeight_ * std::exp(std::abs(delta_t) / tau_);
                    weightChange -= contribution * learningRateNorm_;
                }
            }
        }
        
        // Apply weight change with synapse-specific modulation
        float modulationFactor = getModulationFactor(synapse); // Need to add this
        weightChange *= modulationFactor;
        
        applyWeightChange(synapse, weightChange);
    }
    
    const char* getName() const override { return "Advanced-STDP"; }
    
    // Advanced STDP parameters
    void setLTPWeight(float weight) override { configure(weight, ltdWeight_, tau_); }
    float getLTPWeight() const override { return ltpWeight_; }
    
    void setLTDWeight(float weight) override { configure(ltpWeight_, weight, tau_); }
    float getLTDWeight() const override { return ltdWeight_; }
    
    void setTimeConstant(float tau) override { configure(ltpWeight_, ltdWeight_, tau); }
    float getTimeConstant() const override { return tau_; }
    
    // Additional advanced STDP features
    void setTripletWeights(float a_plus, float a_minus, float b) {
        a_plus_ = std::max(0.0f, a_plus);
        a_minus_ = std::max(0.0f, a_minus);
        b_ = std::max(0.0f, b);
    }
    
    void enableTripletInteractions(bool enable) {
        enableTripletInteractions_ = enable;
    }
    
    // Spike trace management for triplet STDP
    void updateSpikeTraces(const std::vector<Timestamp>& spikeTimes, 
                          std::vector<float>& trace, float decayRate) {
        float currentTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Decay existing traces
        for (float& traceVal : trace) {
            traceVal *= std::exp(-decayRate * (currentTime - lastUpdateTime_));
        }
        
        // Add new spikes to traces
        for (Timestamp spikeTime : spikeTimes) {
            double spikeTimeDouble = static_cast<double>(spikeTime);
            for (size_t i = 0; i < trace.size(); ++i) {
                float delta = static_cast<float>(currentTime - spikeTimeDouble);
                if (delta < 100.0) { // Within 100ms window
                    trace[i] += 1.0f * std::exp(-delta / 10.0f); // Fast decay
                }
            }
        }
        
        lastUpdateTime_ = currentTime;
    }
    
private:
    float ltpWeight_;
    float ltdWeight_;
    float tau_;
    float learningRateNorm_;
    
    // Triplet STDP parameters
    float a_plus_;
    float a_minus_;
    float b_;
    bool enableTripletInteractions_;
    
    // Helper methods (need implementation)
    Timestamp getSynapseCreationTime(Synapse* synapse) {
        // Need to add creation time tracking to Synapse class
        return 0;
    }
    
    float getModulationFactor(Synapse* synapse) {
        // Get neuromodulation factor from synapse or global state
        return 1.0f;
    }
};

// Reward-modulated STDP (R-STDP) with enhanced features
class RewardModulatedSTDP : public EnhancedPlasticityRule {
public:
    RewardModulatedSTDP() {
        configure(0.01f, 0.012f, 20.0f, 1.0f); // Default values including reward modulation
    }
    
    void configure(float ltpWeight, float ltdWeight, float tau, float rewardMod) {
        EnhancedPlasticityRule::configure(ltpWeight, ltdWeight, tau);
        rewardMod_ = std::max(0.0f, rewardMod);
        baselineLearningRate_ = 0.01f;
    }
    
    void applyPlasticity(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes, TimestepDuration dt,
                        double timeSinceLastUpdate) override {
        if (!synapse) return;
        
        // Get current reward prediction error
        float rpe = getRewardPredictionError(); // Need to add this
        
        // Get current dopamine level
        float dopamine = getDopamineLevel(); // Need to add this
        
        // Reward modulation factor
        float rewardMod = calculateRewardModulation(rpe, dopamine);
        
        // Apply standard STDP with reward modulation
        AdvancedSTDP::applyPlasticity(synapse, preSpikes, postSpikes, dt, timeSinceLastUpdate);
        
        // Additional reward-based modulation
        float rewardFactor = baselineLearningRate_ * rewardMod;
        applyWeightChange(synapse, rewardFactor * 0.001f); // Small additional change
    }
    
    const char* getName() const override { return "RewardModulated-STDP"; }
    
    // Reward modulation configuration
    void setRewardModulationFactor(float factor) {
        rewardMod_ = std::max(0.0f, factor);
    }
    
    float getRewardModulationFactor() const { return rewardMod_; }
    
    void setBaselineLearningRate(float rate) {
        baselineLearningRate_ = std::max(0.0f, rate);
    }
    
    // Reward prediction error handling
    void updateRewardPredictionError(float rpe) {
        currentRPE_ = rpe;
    }
    
    float getCurrentRPE() const { return currentRPE_; }
    
private:
    float rewardMod_;
    float baselineLearningRate_;
    float currentRPE_;
    
    float calculateRewardModulation(float rpe, float dopamine) {
        // Complex reward modulation function
        float mod = 1.0f + rewardMod_ * (rpe * 0.5f + dopamine * 0.3f);
        return std::max(0.05f, std::min(2.0f, mod)); // Clamp to reasonable range
    }
    
    float getRewardPredictionError() {
        // Need to implement actual RPE calculation
        return currentRPE_;
    }
    
    float getDopamineLevel() {
        // Need to implement actual dopamine level retrieval
        return 0.5f;
    }
};

// Concept-based Hebbian learning with pattern association
class ConceptBasedHebbian : public EnhancedPlasticityRule {
public:
    ConceptBasedHebbian() {
        configure(0.01f, 0.5f, 100.0f); // learning rate, max weight, association window
    }
    
    void configure(float learningRate, float maxWeight, float associationWindow) {
        learningRate_ = std::max(0.0f, learningRate);
        maxWeight_ = std::max(0.1f, maxWeight);
        associationWindow_ = std::max(1.0f, associationWindow);
    }
    
    void applyPlasticity(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes, TimestepDuration dt,
                        double timeSinceLastUpdate) override {
        if (!synapse) return;
        
        // Get concept association information
        ConceptId preConcept = getPreSpikeConcept(preSpikes); // Need to add this
        ConceptId postConcept = getPostSpikeConcept(postSpikes); // Need to add this
        
        // Check if concepts are associated
        float associationStrength = getConceptAssociation(preConcept, postConcept);
        
        // Apply Hebbian learning based on concept association
        float weightChange = 0.0f;
        
        if (associationStrength > 0.0f) {
            // Conceptually correlated firing strengthens synapse
            weightChange = learningRate_ * associationStrength * 
                          calculateCoincidenceDetection(preSpikes, postSpikes);
        } else {
            // Unassociated firing has minimal effect
            weightChange = learningRate_ * calculateWeakCoincidence(preSpikes, postSpikes) * 0.1f;
        }
        
        // Apply weight change with concept-based bounds
        applyWeightChange(synapse, weightChange);
        
        // Update concept associations
        updateConceptAssociation(preConcept, postConcept, weightChange);
    }
    
    const char* getName() const override { return "ConceptBased-Hebbian"; }
    
    // Concept management
    void setConceptAssociation(ConceptId conceptA, ConceptId conceptB, float strength) {
        conceptAssociations_[std::make_pair(conceptA, conceptB)] = strength;
        conceptAssociations_[std::make_pair(conceptB, conceptA)] = strength;
    }
    
    float getConceptAssociation(ConceptId conceptA, ConceptId conceptB) const {
        auto key = std::make_pair(conceptA, conceptB);
        auto it = conceptAssociations_.find(key);
        return (it != conceptAssociations_.end()) ? it->second : 0.0f;
    }
    
    // Pattern learning capabilities
    void learnPattern(const std::vector<NeuronId>& pattern, float learningRate) {
        // Learn pattern associations
        patternLearning_[pattern] += learningRate;
    }
    
    float getPatternStrength(const std::vector<NeuronId>& pattern) const {
        auto it = patternLearning_.find(pattern);
        return (it != patternLearning_.end()) ? it->second : 0.0f;
    }
    
private:
    float learningRate_;
    float maxWeight_;
    float associationWindow_;
    
    // Concept and pattern storage
    std::map<std::pair<ConceptId, ConceptId>, float> conceptAssociations_;
    std::map<std::vector<NeuronId>, float> patternLearning_;
    
    // Helper methods (need implementation)
    ConceptId getPreSpikeConcept(const std::vector<Timestamp>& spikes) {
        // Need to implement concept identification from spike trains
        return 0;
    }
    
    ConceptId getPostSpikeConcept(const std::vector<Timestamp>& spikes) {
        // Need to implement concept identification from spike trains
        return 0;
    }
    
    float calculateCoincidenceDetection(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes) {
        // Calculate temporal coincidence between pre and post spikes
        if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
        
        float minTimeDiff = std::numeric_limits<float>::max();
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                float diff = std::abs(static_cast<float>(post - pre));
                if (diff < minTimeDiff) {
                    minTimeDiff = diff;
                }
            }
        }
        
        return std::exp(-minTimeDiff / associationWindow_);
    }
    
    float calculateWeakCoincidence(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes) {
        // Calculate weak temporal correlation
        if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
        
        float totalCorrelation = 0.0f;
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                float diff = std::abs(static_cast<float>(post - pre));
                if (diff > associationWindow_) {
                    totalCorrelation += std::exp(-diff / (associationWindow_ * 10.0f));
                }
            }
        }
        
        return totalCorrelation / (preSpikes.size() * postSpikes.size());
    }
    
    void updateConceptAssociation(ConceptId conceptA, ConceptId conceptB, float weightChange) {
        auto key = std::make_pair(conceptA, conceptB);
        auto reverseKey = std::make_pair(conceptB, conceptA);
        
        if (conceptAssociations_.find(key) != conceptAssociations_.end()) {
            conceptAssociations_[key] += weightChange * 0.01f; // Slow concept learning
            conceptAssociations_[reverseKey] += weightChange * 0.01f;
        }
    }
};

// Structural plasticity with activity-dependent synapse creation
class ActivityDependentStructuralPlasticity : public EnhancedPlasticityRule {
public:
    ActivityDependentStructuralPlasticity() {
        configure(0.0001f, 0.00001f, 0.01f); // synaptogenesis_rate, pruning_rate, threshold
    }
    
    void configure(float synaptogenesisRate, float pruningRate, float activityThreshold) {
        synaptogenesisRate_ = std::max(0.0f, synaptogenesisRate);
        pruningRate_ = std::max(0.0f, pruningRate);
        activityThreshold_ = std::max(0.0f, activityThreshold);
    }
    
    void applyPlasticity(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes, TimestepDuration dt,
                        double timeSinceLastUpdate) override {
        // This method is overridden to handle structural changes
        // rather than synaptic weight changes
        
        if (!synapse) return;
        
        // Calculate local neuronal activity
        float localActivity = calculateLocalActivity(preSpikes, postSpikes);
        
        // Apply structural plasticity based on activity
        if (localActivity > activityThreshold_) {
            // Synaptogenesis: create new synapses
            float synaptogenesisProbability = synaptogenesisRate_ * localActivity;
            if (shouldCreateSynapse(synaptogenesisProbability)) {
                createNewSynapse(); // Need to implement synapse creation
            }
            
            // Strengthen existing synapses
            float strengthening = synaptogenesisProbability * 0.1f;
            applyWeightChange(synapse, strengthening);
        } else {
            // Pruning: remove weak synapses
            float pruningProbability = pruningRate_ * (1.0f - localActivity);
            if (shouldPruneSynapse(pruningProbability) && synapse->getWeight() < 0.1f) {
                pruneSynapse(synapse); // Need to implement synapse removal
            }
            
            // Depression of existing synapses
            float depression = pruningProbability * 0.05f;
            applyWeightChange(synapse, -depression);
        }
    }
    
    const char* getName() const override { return "ActivityDependent-Structural"; }
    
    // Structural plasticity control
    void setSynaptogenesisRate(float rate) { configure(rate, pruningRate_, activityThreshold_); }
    float getSynaptogenesisRate() const { return synaptogenesisRate_; }
    
    void setPruningRate(float rate) { configure(synaptogenesisRate_, rate, activityThreshold_); }
    float getPruningRate() const { return pruningRate_; }
    
    void setActivityThreshold(float threshold) { configure(synaptogenesisRate_, pruningRate_, threshold); }
    float getActivityThreshold() const { return activityThreshold_; }
    
    // Network-level structural updates
    void updateNetworkStructure(const std::vector<Neuron*>& neurons) {
        // Analyze network activity for large-scale structural changes
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons) {
            totalActivity += calculateNeuronalActivity(neuron);
        }
        
        // Global structural changes based on network activity
        float globalActivityFactor = std::min(1.0f, totalActivity / neurons.size());
        
        // Modulate structural plasticity rates based on global activity
        float modulatedSynaptogenesis = synaptogenesisRate_ * (1.0f + globalActivityFactor * 0.5f);
        float modulatedPruning = pruningRate_ * (1.0f - globalActivityFactor * 0.3f);
        
        // Apply global structural changes
        if (globalActivityFactor > 0.8f) {
            // High activity -> promote synaptogenesis
            performBulkSynaptogenesis(); // Need to implement
        } else if (globalActivityFactor < 0.2f) {
            // Low activity -> promote pruning
            performBulkPruning(); // Need to implement
        }
    }
    
private:
    float synaptogenesisRate_;
    float pruningRate_;
    float activityThreshold_;
    
    // Structural state
    size_t totalSynapsesCreated_;
    size_t totalSynapsesPruned_;
    
    float calculateLocalActivity(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes) {
        // Calculate local neuronal activity from spike trains
        float preActivity = static_cast<float>(preSpikes.size());
        float postActivity = static_cast<float>(postSpikes.size());
        
        // Combine pre- and post-synaptic activity
        return (preActivity + postActivity) * 0.5f;
    }
    
    bool shouldCreateSynapse(float probability) {
        // Randomly decide whether to create a new synapse
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        return dis(gen) < probability;
    }
    
    bool shouldPruneSynapse(float probability) {
        // Randomly decide whether to prune an existing synapse
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        return dis(gen) < probability;
    }
    
    void createNewSynapse() {
        // Create a new synapse (implementation depends on synapse creation API)
        totalSynapsesCreated_++;
    }
    
    void pruneSynapse(Synapse* synapse) {
        // Remove a synapse (implementation depends on synapse removal API)
        if (synapse) {
            totalSynapsesPruned_++;
        }
    }
    
    float calculateNeuronalActivity(Neuron* neuron) {
        // Calculate activity level of a neuron
        if (!neuron) return 0.0f;
        
        const auto& state = neuron->getState();
        float membraneActivity = std::abs(state.membranePotential - state.restingPotential);
        float firingActivity = neuron->isFiring() ? 1.0f : 0.0f;
        
        return (membraneActivity * 0.7f) + (firingActivity * 0.3f);
    }
    
    void performBulkSynaptogenesis() {
        // Create multiple new synapses for network growth
        size_t newSynapses = static_cast<size_t>(synaptogenesisRate_ * 100.0f);
        for (size_t i = 0; i < newSynapses; ++i) {
            createNewSynapse();
        }
    }
    
    void performBulkPruning() {
        // Remove multiple synapses for network refinement
        size_t synapsesToPrune = static_cast<size_t>(pruningRate_ * 100.0f);
        for (size_t i = 0; i < synapsesToPrune; ++i) {
            pruneSynapse(nullptr); // Would need actual synapse pointers
        }
    }
};

// Factory function for creating enhanced plasticity rules
inline std::unique_ptr<PlasticityRule> createEnhancedPlasticityRule(const std::string& type) {
    if (type == "advanced-stdp") {
        return std::make_unique<AdvancedSTDP>();
    } else if (type == "reward-modulated-stdp") {
        return std::make_unique<RewardModulatedSTDP>();
    } else if (type == "concept-based-hebbian") {
        return std::make_unique<ConceptBasedHebbian>();
    } else if (type == "activity-dependent-structural") {
        return std::make_unique<ActivityDependentStructuralPlasticity>();
    } else {
        // Default to standard Hebbian
        return std::make_unique<Hebbian>();
    }
}

} // namespace nlm