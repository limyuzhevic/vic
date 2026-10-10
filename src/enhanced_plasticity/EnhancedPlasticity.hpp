#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

// Forward declarations
class Synapse;
class Neuron;

// Advanced plasticity rules for meta-learning and adaptive behavior

// Context-dependent Hebbian plasticity with eligibility traces
class ContextDependentHebbian {
public:
    struct HebbianContext {
        NeuronId pre_neuron;
        NeuronId post_neuron;
        std::vector<float> contextual_features;  // Environmental/context features
        float temporal_context;                    // Time since last interaction
        float novelty_score;                       // How novel the context is
        
        HebbianContext() : pre_neuron(), post_neuron(), temporal_context(0.0f), 
                          novelty_score(0.0f) {}
        
        HebbianContext(NeuronId pre, NeuronId post, const std::vector<float>& features,
                      float time, float novelty)
            : pre_neuron(pre), post_neuron(post), contextual_features(features),
              temporal_context(time), novelty_score(novelty) {}
    };
    
    struct EligibilityTrace {
        float trace;
        float decay_rate;
        float modulation_strength;
        Timestamp timestamp;
        
        EligibilityTrace() : trace(0.0f), decay_rate(0.95f), modulation_strength(1.0f), timestamp(0.0) {}
    };
    
    ContextDependentHebbian() : learning_rate_(0.01f), context_decay_(0.9f), 
                                novelty_weight_(1.0f), homeostatic_factor_(0.1f) {}
    
    // Compute weight change based on context
    float computeWeightChange(Synapse* synapse, const HebbianContext& context,
                             float global_reward = 1.0f);
    
    // Update eligibility traces
    void updateEligibilityTraces(const std::vector<NeuronId>& active_neurons, 
                                 float dt, const std::vector<float>& context);
    
    // Apply homeostatic regulation
    void applyHomeostaticRegulation(Synapse* synapse, float target_activity,
                                   float current_activity, float dt);
    
    // Set parameters
    void setLearningRate(float rate) { learning_rate_ = rate; }
    void setContextDecay(float decay) { context_decay_ = decay; }
    void setNoveltyWeight(float weight) { novelty_weight_ = weight; }
    void setHomeostaticFactor(float factor) { homeostatic_factor_ = factor; }
    
private:
    float learning_rate_;
    float context_decay_;
    float novelty_weight_;
    float homeostatic_factor_;
    
    std::unordered_map<SynapseId, EligibilityTrace> eligibility_traces_;
    std::vector<HebbianContext> recent_contexts_;
};

// Meta-learning plasticity with online adaptation
class MetaLearningPlasticity {
public:
    struct MetaLearningState {
        float learning_rate;
        float momentum;
        float confidence;
        float prediction_error;
        float meta_gradient;
        Timestamp last_update;
        
        MetaLearningState() : learning_rate(0.01f), momentum(0.9f), confidence(0.5f),
                              prediction_error(0.0f), meta_gradient(0.0f), last_update(0.0) {}
    };
    
    MetaLearningPlasticity() : base_learning_rate_(0.01f), meta_learning_rate_(0.001f),
                              adaptation_window_(1000), max_confidence_(1.0f) {}
    
    // Perform meta-learning update based on task performance
    void performMetaLearningUpdate(Synapse* synapse, float task_performance,
                                   const MetaLearningState& current_state,
                                   float dt);
    
    // Adapt learning rate based on meta-prediction error
    void adaptLearningRate(Synapse* synapse, float prediction_error, float dt);
    
    // Compute meta-gradient for learning rate adaptation
    float computeMetaGradient(float current_performance, float previous_performance,
                             float learning_rate, float dt);
    
    // Get current meta-learning state
    MetaLearningState getMetaLearningState(SynapseId synapse_id) const;
    
    // Set parameters
    void setBaseLearningRate(float rate) { base_learning_rate_ = rate; }
    void setMetaLearningRate(float rate) { meta_learning_rate_ = rate; }
    void setAdaptationWindow(size_t window) { adaptation_window_ = window; }
    void setMaxConfidence(float confidence) { max_confidence_ = confidence; }
    
private:
    float base_learning_rate_;
    float meta_learning_rate_;
    size_t adaptation_window_;
    float max_confidence_;
    
    std::unordered_map<SynapseId, MetaLearningState> meta_states_;
    std::vector<float> performance_history_;
};

// Synaptic dropout and pruning strategies
class SynapticPruning {
public:
    struct PruningCriteria {
        float weight_threshold;
        float activity_threshold;
        float redundancy_threshold;
        float importance_score;
        
        PruningCriteria() : weight_threshold(0.01f), activity_threshold(0.001f),
                           redundancy_threshold(0.9f), importance_score(1.0f) {}
    };
    
    struct SynapticStats {
        SynapticWeight current_weight;
        float recent_activity;
        float redundancy_score;
        float importance;
        Timestamp last_update;
        
        SynapticStats() : current_weight(0.0f), recent_activity(0.0f),
                         redundancy_score(0.0f), importance(0.0f), last_update(0.0) {}
    };
    
    SynapticPruning() : pruning_rate_(0.1f), recovery_rate_(0.01f), 
                       importance_decay_(0.99f), min_synapses_(10) {}
    
    // Decide which synapses to prune based on criteria
    std::vector<SynapseId> determinePruningTargets(const std::vector<Synapse*>& synapses,
                                                  const PruningCriteria& criteria);
    
    // Apply synaptic dropout during training
    void applySynapticDropout(std::vector<Synapse*>& synapses, float dropout_rate,
                           std::mt19937& rng);
    
    // Recover synapses that were previously pruned
    void recoverSynapses(std::vector<Synapse*>& synapses, const std::vector<SynapseId>& recovered_ids,
                        float recovery_rate);
    
    // Update importance scores based on usage
    void updateImportanceScores(Synapse* synapse, float activity, float dt);
    
    // Get synaptic statistics
    SynapticStats getSynapticStats(SynapseId synapse_id) const;
    
    // Set parameters
    void setPruningRate(float rate) { pruning_rate_ = rate; }
    void setRecoveryRate(float rate) { recovery_rate_ = rate; }
    void setImportanceDecay(float decay) { importance_decay_ = decay; }
    void setMinSynapses(size_t min) { min_synapses_ = min; }
    
private:
    float pruning_rate_;
    float recovery_rate_;
    float importance_decay_;
    size_t min_synapses_;
    
    std::unordered_map<SynapseId, SynapticStats> synaptic_stats_;
    std::vector<SynapseId> pruned_synapses_;
};

// Neuromodulator-gated plasticity
class NeuromodulatorGatedPlasticity {
public:
    struct NeuromodulatorState {
        float dopamine;        // Reward prediction error signal
        float acetylcholine;  // Attention/arousal
        float serotonin;      // Behavioral inhibition/excitation
        float noradrenaline;   // Focus and alertness
        float glutamate;      // General excitation
        float gaba;           // General inhibition
        
        NeuromodulatorState() : dopamine(1.0f), acetylcholine(1.0f), serotonin(1.0f),
                               noradrenaline(1.0f), glutamate(1.0f), gaba(1.0f) {}
    };
    
    NeuromodulatorGatedPlasticity() : plasticity_sensitivity_(1.0f), 
                                     neuromodulation_window_(100) {}
    
    // Apply plasticity gated by neuromodulatory state
    float applyGatedPlasticity(Synapse* synapse, float raw_plasticity_change,
                              const NeuromodulatorState& neuromodulators, float dt);
    
    // Update neuromodulator state based on neural activity
    void updateNeuromodulatorState(const std::vector<Neuron*>& neurons, float dt);
    
    // Compute plasticity modulation factor
    float computeModulationFactor(const NeuromodulatorState& neuromodulators,
                                 const HebbianContext& context) const;
    
    // Get current neuromodulator state
    NeuromodulatorState getNeuromodulatorState() const { return current_neuromodulators_; }
    
    // Set parameters
    void setPlasticitySensitivity(float sensitivity) { plasticity_sensitivity_ = sensitivity; }
    void setNeuromodulationWindow(size_t window) { neuromodulation_window_ = window; }
    
private:
    float plasticity_sensitivity_;
    size_t neuromodulation_window_;
    NeuromodulatorState current_neuromodulators_;
    std::vector<NeuromodulatorState> neuromodulator_history_;
};

} // namespace nlm
