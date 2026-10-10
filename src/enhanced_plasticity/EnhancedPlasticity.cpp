#include "EnhancedPlasticity.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <random>
#include <numeric>

namespace nlm {

float ContextDependentHebbian::computeWeightChange(Synapse* synapse, const HebbianContext& context,
                                                 float global_reward) {
    if (!synapse || context.pre_neuron == INVALID_NEURON_ID || 
        context.post_neuron == INVALID_NEURON_ID) {
        return 0.0f;
    }
    
    // Compute base Hebbian term
    float hebbian_term = learning_rate_ * global_reward;
    
    // Apply context modulation
    float context_modulation = 1.0f;
    for (float feature : context.contextual_features) {
        context_modulation *= (1.0f + feature * novelty_weight_);
    }
    context_modulation *= std::exp(-context.temporal_context * context_decay_);
    
    // Apply novelty scaling
    context_modulation *= (1.0f + context.novelty_score * 0.5f);
    
    // Combine components
    float total_change = hebbian_term * context_modulation;
    
    // Store in eligibility trace system
    SynapseId syn_id = synapse->getId();
    if (eligibility_traces_.find(syn_id) == eligibility_traces_.end()) {
        eligibility_traces_[syn_id] = EligibilityTrace();
    }
    
    eligibility_traces_[syn_id].trace += total_change;
    eligibility_traces_[syn_id].decay_rate = context_decay_;
    eligibility_traces_[syn_id].modulation_strength = context_modulation;
    eligibility_traces_[syn_id].timestamp = std::chrono::duration<double>().count();
    
    return total_change;
}

void ContextDependentHebbian::updateEligibilityTraces(const std::vector<NeuronId>& active_neurons,
                                                     float dt, const std::vector<float>& context) {
    // Decay existing traces
    for (auto& pair : eligibility_traces_) {
        auto& trace = pair.second;
        trace.trace *= trace.decay_rate;
        trace.trace = std::max(0.0f, trace.trace);
        
        // Update timestamp
        double current_time = std::chrono::duration<double>().count();
        double time_since = current_time - trace.timestamp;
        trace.timestamp = current_time;
        
        // Apply decay based on time
        if (time_since > 0.1) {
            trace.trace *= std::pow(trace.decay_rate, static_cast<float>(time_since));
        }
    }
    
    // Record new context
    if (!context.empty()) {
        HebbianContext new_context;
        new_context.contextual_features = context;
        new_context.temporal_context = dt;
        new_context.novelty_score = calculateNoveltyScore(context);
        recent_contexts_.push_back(new_context);
        
        // Maintain history size
        if (recent_contexts_.size() > 1000) {
            recent_contexts_.erase(recent_contexts_.begin());
        }
    }
}

void ContextDependentHebbian::applyHomeostaticRegulation(Synapse* synapse, float target_activity,
                                                        float current_activity, float dt) {
    if (!synapse) return;
    
    float error = target_activity - current_activity;
    float regulatory_change = homeostatic_factor_ * error * dt;
    
    float current_weight = synapse->getWeight();
    float new_weight = current_weight + regulatory_change;
    
    // Constrain to valid range
    new_weight = std::clamp(new_weight, -1.0f, 1.0f);
    synapse->setWeight(new_weight);
}

void MetaLearningPlasticity::performMetaLearningUpdate(Synapse* synapse, float task_performance,
                                                      const MetaLearningState& current_state,
                                                      float dt) {
    if (!synapse) return;
    
    SynapseId syn_id = synapse->getId();
    MetaLearningState& meta_state = meta_states_[syn_id];
    
    // Update meta-learning state based on task performance
    meta_state.prediction_error = std::abs(current_state.prediction_error - task_performance);
    meta_state.meta_gradient = computeMetaGradient(task_performance, 
                                                   meta_state.prediction_error,
                                                   current_state.learning_rate, dt);
    
    // Adapt learning rate
    adaptLearningRate(synapse, meta_state.prediction_error, dt);
    
    // Update momentum and confidence
    meta_state.momentum = std::min(1.0f, meta_state.momentum + meta_state.meta_gradient * dt);
    meta_state.confidence = std::min(max_confidence_, 
                                   meta_state.confidence + task_performance * 0.01f * dt);
    
    meta_state.last_update = std::chrono::duration<double>().count();
}

void MetaLearningPlasticity::adaptLearningRate(Synapse* synapse, float prediction_error, float dt) {
    if (!synapse) return;
    
    SynapseId syn_id = synapse->getId();
    MetaLearningState& meta_state = meta_states_[syn_id];
    
    // Adaptive learning rate based on prediction error
    float adaptive_rate = base_learning_rate_ * (1.0f + prediction_error * meta_learning_rate_);
    adaptive_rate = std::clamp(adaptive_rate, 0.001f, 1.0f);
    
    // Apply to synapse weight
    float current_weight = synapse->getWeight();
    float new_weight = current_weight + adaptive_rate * meta_state.meta_gradient * dt;
    
    synapse->setWeight(new_weight);
}

float MetaLearningPlasticity::computeMetaGradient(float current_performance, float previous_performance,
                                                 float learning_rate, float dt) {
    float performance_change = current_performance - previous_performance;
    float gradient = performance_change / (learning_rate + 0.001f);
    
    // Smooth gradient over time
    if (!performance_history_.empty()) {
        float avg_change = std::accumulate(performance_history_.begin(), 
                                         performance_history_.end(), 0.0f) / 
                          performance_history_.size();
        gradient = 0.7f * gradient + 0.3f * avg_change;
    }
    
    performance_history_.push_back(performance_change);
    if (performance_history_.size() > adaptation_window_) {
        performance_history_.erase(performance_history_.begin());
    }
    
    return gradient;
}

MetaLearningPlasticity::MetaLearningState MetaLearningPlasticity::getMetaLearningState(SynapseId synapse_id) const {
    auto it = meta_states_.find(synapse_id);
    if (it != meta_states_.end()) {
        return it->second;
    }
    return MetaLearningState();
}

std::vector<SynapseId> SynapticPruning::determinePruningTargets(const std::vector<Synapse*>& synapses,
                                                            const PruningCriteria& criteria) {
    std::vector<SynapseId> targets;
    
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        SynapseId syn_id = synapse->getId();
        SynapticStats& stats = synaptic_stats_[syn_id];
        
        // Update stats
        stats.current_weight = synapse->getWeight();
        stats.last_update = std::chrono::duration<double>().count();
        
        // Check pruning criteria
        if (std::abs(stats.current_weight) < criteria.weight_threshold) {
            targets.push_back(syn_id);
        }
        else if (stats.recent_activity < criteria.activity_threshold) {
            targets.push_back(syn_id);
        }
        else if (stats.redundancy_score > criteria.redundancy_threshold) {
            targets.push_back(syn_id);
        }
    }
    
    // Sort by importance (keep more important synapses)
    std::sort(targets.begin(), targets.end(),
              [&](SynapseId id1, SynapseId id2) {
                  return synaptic_stats_[id1].importance > synaptic_stats_[id2].importance;
              });
    
    // Prune only a percentage to maintain network stability
    size_t prune_count = static_cast<size_t>(targets.size() * pruning_rate_);
    if (targets.size() > min_synapses_) {
        prune_count = std::min(prune_count, targets.size() - min_synapses_);
    }
    
    targets.resize(prune_count);
    return targets;
}

void SynapticPruning::applySynapticDropout(std::vector<Synapse*>& synapses, float dropout_rate,
                                         std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        if (dist(rng) < dropout_rate) {
            // Set weight to near zero (effectively dropping the synapse)
            float current_weight = synapse->getWeight();
            synapse->setWeight(current_weight * (1.0f - dropout_rate * 2.0f));
            
            // Record for potential recovery
            SynapseId syn_id = synapse->getId();
            pruned_synapses_.push_back(syn_id);
        }
    }
}

void SynapticPruning::recoverSynapses(std::vector<Synapse*>& synapses, const std::vector<SynapseId>& recovered_ids,
                                     float recovery_rate) {
    for (SynapseId syn_id : recovered_ids) {
        for (Synapse* synapse : synapses) {
            if (synapse && synapse->getId() == syn_id) {
                float current_weight = synapse->getWeight();
                float recovered_weight = current_weight / (1.0f - recovery_rate);
                synapse->setWeight(recovered_weight);
                
                // Remove from pruned list
                auto it = std::find(pruned_synapses_.begin(), pruned_synapses_.end(), syn_id);
                if (it != pruned_synapses_.end()) {
                    pruned_synapses_.erase(it);
                }
                break;
            }
        }
    }
}

void SynapticPruning::updateImportanceScores(Synapse* synapse, float activity, float dt) {
    if (!synapse) return;
    
    SynapseId syn_id = synapse->getId();
    SynapticStats& stats = synaptic_stats_[syn_id];
    
    // Update importance based on activity
    stats.importance = std::max(0.0f, stats.importance + activity * dt);
    stats.importance *= importance_decay_;
    
    // Update recent activity (exponential moving average)
    stats.recent_activity = 0.9f * stats.recent_activity + 0.1f * activity;
    
    // Update redundancy based on weight similarity to other synapses
    // This would require checking other synapses in the network
    // For simplicity, we'll use a placeholder calculation
    stats.redundancy_score = calculateRedundancyScore(synapse, stats.current_weight);
}

SynapticPruning::SynapticStats SynapticPruning::getSynapticStats(SynapseId synapse_id) const {
    auto it = synaptic_stats_.find(synapse_id);
    if (it != synaptic_stats_.end()) {
        return it->second;
    }
    return SynapticStats();
}

float NeuromodulatorGatedPlasticity::applyGatedPlasticity(Synapse* synapse, float raw_plasticity_change,
                                                       const NeuromodulatorState& neuromodulators, float dt) {
    if (!synapse) return 0.0f;
    
    // Compute modulation factor based on neuromodulators
    float modulation_factor = computeModulationFactor(neuromodulators, HebbianContext());
    
    // Apply plasticity sensitivity
    float gated_change = raw_plasticity_change * modulation_factor * plasticity_sensitivity_ * dt;
    
    // Apply to synapse
    float current_weight = synapse->getWeight();
    float new_weight = current_weight + gated_change;
    
    // Constrain to valid range
    new_weight = std::clamp(new_weight, -1.0f, 1.0f);
    synapse->setWeight(new_weight);
    
    return gated_change;
}

void NeuromodulatorGatedPlasticity::updateNeuromodulatorState(const std::vector<Neuron*>& neurons, float dt) {
    float total_dopamine = 0.0f;
    float total_acetylcholine = 0.0f;
    float total_serotonin = 0.0f;
    float total_noradrenaline = 0.0f;
    
    // Aggregate neuromodulator signals from neurons
    for (Neuron* neuron : neurons) {
        if (!neuron) continue;
        
        const auto& state = neuron->getState();
        
        // Simple model: firing rate influences neuromodulator release
        float firing_rate = state.firingRate;
        
        total_dopamine += firing_rate * 0.1f;
        total_acetylcholine += firing_rate * 0.15f;
        total_serotonin += firing_rate * 0.05f;
        total_noradrenaline += firing_rate * 0.12f;
    }
    
    // Normalize by number of neurons
    int neuron_count = neurons.size();
    if (neuron_count > 0) {
        current_neuromodulators_.dopamine = total_dopamine / neuron_count;
        current_neuromodulators_.acetylcholine = total_acetylcholine / neuron_count;
        current_neuromodulators_.serotonin = total_serotonin / neuron_count;
        current_neuromodulators_.noradrenaline = total_noradrenaline / neuron_count;
    }
    
    // Store in history for temporal dynamics
    neuromodulator_history_.push_back(current_neuromodulators_);
    if (neuromodulator_history_.size() > neuromodulation_window_) {
        neuromodulator_history_.erase(neuromodulator_history_.begin());
    }
}

float NeuromodulatorGatedPlasticity::computeModulationFactor(const NeuromodulatorState& neuromodulators,
                                                          const HebbianContext& context) const {
    // Compute modulation based on neuromodulator levels
    float dopamine_factor = 1.0f + neuromodulators.dopamine * 0.5f;
    float acetylcholine_factor = 1.0f + neuromodulators.acetylcholine * 0.3f;
    float serotonin_factor = 1.0f - neuromodulators.serotonin * 0.2f;  // Inhibition
    float noradrenaline_factor = 1.0f + neuromodulators.noradrenaline * 0.4f;
    
    // Combine factors
    float total_factor = dopamine_factor * acetylcholine_factor * 
                        serotonin_factor * noradrenaline_factor;
    
    return std::max(0.0f, total_factor);
}

} // namespace nlm
