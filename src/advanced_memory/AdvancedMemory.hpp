#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <functional>

namespace nlm {

// Advanced memory systems for hierarchical and temporal processing

// Hierarchical memory organization with multiple timescales
class HierarchicalMemory {
public:
    struct MemoryNode {
        // Core memory properties
        SimulationStep timestamp;
        NeuronId source_neuron;
        std::vector<float> pattern;
        float strength;
        float stability;
        
        // Hierarchical organization
        size_t hierarchy_level;  // 0 = working memory, higher = more stable
        std::vector<MemoryNode*> children;
        MemoryNode* parent;
        
        // Temporal properties
        float decay_rate;
        float consolidation_threshold;
        SimulationStep last_access;
        SimulationStep creation_time;
        
        // Interference management
        float interference_score;
        std::vector<float> interference_signature;
        
        MemoryNode() : parent(nullptr), hierarchy_level(0), stability(1.0f),
                       decay_rate(0.99f), consolidation_threshold(0.8f),
                       last_access(0), creation_time(0), interference_score(0.0f) {}
        
        MemoryNode(SimulationStep ts, NeuronId source, const std::vector<float>& pat, float str)
            : timestamp(ts), source_neuron(source), pattern(pat), strength(str),
              parent(nullptr), hierarchy_level(0), stability(1.0f),
              decay_rate(0.99f), consolidation_threshold(0.8f),
              last_access(ts), creation_time(ts), interference_score(0.0f) {}
        
        ~MemoryNode() {
            for (MemoryNode* child : children) {
                delete child;
            }
        }
        
        // Compare nodes for interference analysis
        bool operator<(const MemoryNode& other) const {
            return interference_score > other.interference_score;
        }
    };
    
    struct ConsolidationResult {
        bool was_consolidated;
        float new_stability;
        std::vector<MemoryNode*> merged_nodes;
        float interference_reduction;
        
        ConsolidationResult() : was_consolidated(false), new_stability(0.0f),
                               interference_reduction(0.0f) {}
    };
    
    HierarchicalMemory() : max_memory_size_(10000), hierarchy_depth_(5),
                          consolidation_interval_(100), interference_threshold_(0.7f),
                          temporal_decay_(0.995f) {}
    
    // Store pattern in hierarchical memory
    void store(const std::vector<float>& pattern, float strength, NeuronId source,
               SimulationStep timestamp);
    
    // Retrieve memories with temporal filtering
    std::vector<float> retrieve(SimulationStep current_timestamp,
                               float temporal_window = 1000.0f,
                               const std::vector<float>& context = {});
    
    // Consolidate memories into higher levels
    void consolidate(SimulationStep current_step);
    
    // Interfere memories with different signatures
    void interfere(MemoryNode* node1, MemoryNode* node2);
    
    // Get hierarchical statistics
    struct MemoryStats {
        size_t total_nodes;
        size_t active_nodes;
        size_t consolidated_nodes;
        float average_stability;
        float interference_level;
        std::vector<size_t> nodes_per_level;
        
        MemoryStats() : total_nodes(0), active_nodes(0), consolidated_nodes(0),
                       average_stability(0.0f), interference_level(0.0f) {}
    };
    
    MemoryStats getMemoryStats() const;
    
    // Set parameters
    void setMaxMemorySize(size_t size) { max_memory_size_ = size; }
    void setHierarchyDepth(size_t depth) { hierarchy_depth_ = depth; }
    void setConsolidationInterval(SimulationStep interval) { consolidation_interval_ = interval; }
    void setInterferenceThreshold(float threshold) { interference_threshold_ = threshold; }
    
private:
    MemoryNode* root_node_;
    size_t max_memory_size_;
    size_t hierarchy_depth_;
    SimulationStep consolidation_interval_;
    float interference_threshold_;
    float temporal_decay_;
    
    std::priority_queue<MemoryNode*> interference_heap_;
    std::unordered_map<NeuronId, std::vector<MemoryNode*>> neuron_memories_;
    
    // Internal helper methods
    void insertMemoryNode(MemoryNode* node);
    MemoryNode* findSimilarNode(const std::vector<float>& pattern, size_t level) const;
    ConsolidationResult consolidateLevel(MemoryNode* level_node, SimulationStep current_step);
    float calculateInterference(const MemoryNode* node1, const MemoryNode* node2) const;
    void cleanupInactiveNodes();
    float calculatePatternSimilarity(const std::vector<float>& pat1, const std::vector<float>& pat2) const;
};

// Temporal memory and sequence prediction
class TemporalMemory {
public:
    struct TemporalSequence {
        std::vector<std::vector<float>> sequence_patterns;  // Each time step's pattern
        std::vector<Timestamp> timestamps;                 // When each pattern occurred
        float predicted_completion;                         // When sequence is predicted to complete
        float completion_probability;                       // Confidence in prediction
        std::vector<size_t> pattern_frequencies;            // How often each pattern appears
        
        TemporalSequence() : predicted_completion(0.0f), completion_probability(0.0f) {}
    };
    
    struct PredictionContext {
        std::vector<float> current_state;
        std::vector<float> recent_context;
        float prediction_confidence;
        Timestamp prediction_time;
        
        PredictionContext() : prediction_confidence(0.0f), prediction_time(0.0) {}
    };
    
    TemporalMemory() : sequence_memory_size_(1000), prediction_window_(50),
                      pattern_matching_threshold_(0.7f), learning_rate_(0.01f),
                      sequence_decay_(0.98f) {}
    
    // Store temporal sequence
    void storeSequence(const std::vector<std::vector<float>>& sequence,
                      const std::vector<Timestamp>& timestamps);
    
    // Predict next pattern in sequence
    std::vector<float> predictNextPattern(const PredictionContext& context);
    
    // Update sequence predictions based on new data
    void updatePredictions(const PredictionContext& context, const std::vector<float>& actual_next);
    
    // Compute prediction error
    float computePredictionError(const std::vector<float>& predicted,
                               const std::vector<float>& actual) const;
    
    // Get temporal statistics
    struct TemporalStats {
        size_t total_sequences;
        size_t active_sequences;
        float average_prediction_accuracy;
        float average_sequence_length;
        float prediction_confidence;
        
        TemporalStats() : total_sequences(0), active_sequences(0),
                         average_prediction_accuracy(0.0f), average_sequence_length(0.0f),
                         prediction_confidence(0.0f) {}
    };
    
    TemporalStats getTemporalStats() const;
    
    // Set parameters
    void setSequenceMemorySize(size_t size) { sequence_memory_size_ = size; }
    void setPredictionWindow(size_t window) { prediction_window_ = window; }
    void setPatternMatchingThreshold(float threshold) { pattern_matching_threshold_ = threshold; }
    void setLearningRate(float rate) { learning_rate_ = rate; }
    void setSequenceDecay(float decay) { sequence_decay_ = decay; }
    
private:
    std::vector<TemporalSequence> sequences_;
    size_t sequence_memory_size_;
    size_t prediction_window_;
    float pattern_matching_threshold_;
    float learning_rate_;
    float sequence_decay_;
    
    // Internal helper methods
    size_t findSimilarSequence(const std::vector<std::vector<float>>& sequence) const;
    std::vector<float> computeSequenceContext(const std::vector<std::vector<float>>& sequence) const;
    float computePatternSimilarity(const std::vector<float>& pat1, const std::vector<float>& pat2) const;
    void updateSequenceStats(TemporalSequence& seq, const std::vector<float>& actual_next);
    std::vector<float> predictFromContext(const PredictionContext& context) const;
};

// Memory consolidation and interference management
class MemoryConsolidation {
public:
    struct ConsolidationRule {
        float threshold_strength;      // Minimum strength for consolidation
        float min_stability;           // Minimum stability required
        float time_window;             // Time window for consolidation
        float interference_penalty;    // Penalty for high interference
        float novelty_bonus;           // Bonus for novel patterns
        
        ConsolidationRule() : threshold_strength(0.5f), min_stability(0.7f),
                              time_window(100.0f), interference_penalty(0.2f),
                              novelty_bonus(0.1f) {}
    };
    
    struct ConsolidationEvent {
        SimulationStep step;
        NeuronId neuron_id;
        std::vector<MemoryNode*> involved_nodes;
        float consolidation_strength;
        bool was_successful;
        std::string consolidation_type;  // "standard", "systematic", "reactive"
        
        ConsolidationEvent() : step(0), consolidation_strength(0.0f), was_successful(false) {}
    };
    
    MemoryConsolidation() : consolidation_rate_(0.05f), reactive_consolidation_(true),
                           systematic_phase_(true), interference_management_(true),
                           memory_boost_(false) {}
    
    // Consolidate memory with multiple rules
    ConsolidationEvent consolidate(MemoryNode* node, SimulationStep current_step);
    
    // Perform systematic consolidation (during rest/sleep)
    void systematicConsolidation(SimulationStep current_step);
    
    // Perform reactive consolidation (based on interference)
    void reactiveConsolidation(SimulationStep current_step);
    
    // Apply memory boost (enhanced consolidation)
    void applyMemoryBoost(MemoryNode* node, float boost_strength);
    
    // Get consolidation statistics
    struct ConsolidationStats {
        size_t total_events;
        size_t successful_consolidations;
        float average_consolidation_strength;
        float interference_reduction;
        std::vector<std::string> consolidation_types;
        
        ConsolidationStats() : total_events(0), successful_consolidations(0),
                              average_consolidation_strength(0.0f), interference_reduction(0.0f) {}
    };
    
    ConsolidationStats getConsolidationStats() const;
    
    // Set parameters
    void setConsolidationRate(float rate) { consolidation_rate_ = rate; }
    void setReactiveConsolidation(bool enable) { reactive_consolidation_ = enable; }
    void setSystematicPhase(bool enable) { systematic_phase_ = enable; }
    void setInterferenceManagement(bool enable) { interference_management_ = enable; }
    void setMemoryBoost(bool enable) { memory_boost_ = enable; }
    
private:
    float consolidation_rate_;
    bool reactive_consolidation_;
    bool systematic_phase_;
    bool interference_management_;
    bool memory_boost_;
    
    std::vector<ConsolidationEvent> consolidation_history_;
    std::vector<ConsolidationRule> consolidation_rules_;
    
    // Internal helper methods
    bool shouldConsolidate(MemoryNode* node, const ConsolidationRule& rule) const;
    float computeInterferencePenalty(MemoryNode* node) const;
    float computeNoveltyBonus(const std::vector<float>& pattern) const;
    void applyConsolidationEffects(MemoryNode* node, const ConsolidationEvent& event);
    std::vector<MemoryNode*> findNodesForConsolidation(SimulationStep current_step) const;
    void cleanupOldConsolidationEvents();
};

// Memory replay and optimization algorithms
class MemoryReplay {
public:
    struct ReplayEvent {
        SimulationStep timestamp;        // When the replay occurs
        SimulationStep replay_step;       // Which simulation step to replay
        std::vector<NeuronId> replay_neuron_ids;
        float replay_strength;             // How strongly to replay
        ReplayPriority priority;           // Priority of the replay
        bool is_optimal;                   // Whether this is an optimal replay
        
        ReplayEvent() : timestamp(0), replay_step(0), replay_strength(1.0f),
                       is_optimal(false) {}
    };
    
    struct ReplayTrace {
        SimulationStep trace_id;
        std::vector<ReplayEvent> events;
        float trace_strength;
        float optimization_score;
        Timestamp last_optimization;
        
        ReplayTrace() : trace_id(0), trace_strength(1.0f), optimization_score(0.0f), last_optimization(0.0) {}
    };
    
    MemoryReplay() : max_replay_traces_(100), replay_frequency_(0.1f),
                     optimization_iterations_(5), meta_learning_(true),
                     replay_strength_decay_(0.95f) {}
    
    // Replay memory trace for optimization
    void replayTrace(const ReplayTrace& trace, float strength = 1.0f);
    
    // Create new replay trace
    ReplayTrace createReplayTrace(const std::vector<MemoryNode*>& nodes,
                                 SimulationStep timestamp);
    
    // Optimize replay traces
    void optimizeReplayTraces(const std::vector<ReplayTrace>& current_traces,
                             const std::vector<float>& performance_metrics);
    
    // Get replay statistics
    struct ReplayStats {
        size_t total_replays;
        size_t optimal_replays;
        float average_replay_strength;
        float optimization_improvement;
        float meta_learning_effectiveness;
        
        ReplayStats() : total_replays(0), optimal_replays(0),
                       average_replay_strength(0.0f), optimization_improvement(0.0f),
                       meta_learning_effectiveness(0.0f) {}
    };
    
    ReplayStats getReplayStats() const;
    
    // Set parameters
    void setMaxReplayTraces(size_t max) { max_replay_traces_ = max; }
    void setReplayFrequency(float frequency) { replay_frequency_ = frequency; }
    void setOptimizationIterations(size_t iterations) { optimization_iterations_ = iterations; }
    void setMetaLearning(bool enable) { meta_learning_ = enable; }
    void setReplayStrengthDecay(float decay) { replay_strength_decay_ = decay; }
    
private:
    std::vector<ReplayTrace> replay_traces_;
    size_t max_replay_traces_;
    float replay_frequency_;
    size_t optimization_iterations_;
    bool meta_learning_;
    float replay_strength_decay_;
    
    // Internal helper methods
    void performReplayEvents(const ReplayTrace& trace);
    float computeTraceOptimality(const ReplayTrace& trace) const;
    float computeMetaLearningSignal(const ReplayTrace& trace, const std::vector<float>& metrics) const;
    std::vector<float> generateOptimalReplaySchedule(const std::vector<ReplayTrace>& traces) const;
    void cleanupExpiredTraces(SimulationStep current_step);
    ReplayPriority computeReplayPriority(const MemoryNode* node, const ReplayTrace& trace) const;
};

// Meta-learning memory for adaptive learning
class MetaLearningMemory {
public:
    struct MetaLearningEntry {
        SimulationStep timestamp;
        std::vector<float> meta_features;     // Features about learning context
        float meta_target;                      // Target for meta-learning
        float meta_prediction;                  // Meta-prediction of learning outcome
        float meta_gradient;                    // Meta-gradient for adaptation
        float learning_effectiveness;           // How well learning worked
        
        MetaLearningEntry() : timestamp(0), meta_target(0.0f), meta_prediction(0.0f),
                             meta_gradient(0.0f), learning_effectiveness(0.0f) {}
    };
    
    struct MetaLearningModel {
        std::vector<std::vector<float>> weights;      // Model weights
        std::vector<float> biases;                     // Model biases
        float learning_rate;                           // Meta-learning rate
        float momentum;                                // Meta-momentum
        float confidence;                              // Model confidence
        
        MetaLearningModel() : learning_rate(0.01f), momentum(0.9f), confidence(0.5f) {}
    };
    
    MetaLearningMemory() : meta_memory_size_(1000), meta_learning_rate_(0.01f),
                          meta_momentum_(0.9f), meta_predictive_(true),
                          meta_adaptive_(true) {}
    
    // Store meta-learning experience
    void storeMetaExperience(const MetaLearningEntry& entry);
    
    // Predict learning outcome
    float predictLearningOutcome(const std::vector<float>& meta_features);
    
    // Update meta-learning model
    void updateMetaModel(const MetaLearningEntry& entry, float actual_outcome);
    
    // Get meta-learning statistics
    struct MetaLearningStats {
        size_t total_entries;
        size_t active_entries;
        float average_prediction_accuracy;
        float meta_learning_effectiveness;
        float adaptation_speed;
        
        MetaLearningStats() : total_entries(0), active_entries(0),
                             average_prediction_accuracy(0.0f), meta_learning_effectiveness(0.0f),
                             adaptation_speed(0.0f) {}
    };
    
    MetaLearningStats getMetaLearningStats() const;
    
    // Set parameters
    void setMetaMemorySize(size_t size) { meta_memory_size_ = size; }
    void setMetaLearningRate(float rate) { meta_learning_rate_ = rate; }
    void setMetaMomentum(float momentum) { meta_momentum_ = momentum; }
    void setMetaPredictive(bool enable) { meta_predictive_ = enable; }
    void setMetaAdaptive(bool enable) { meta_adaptive_ = enable; }
    
private:
    std::vector<MetaLearningEntry> meta_experiences_;
    MetaLearningModel meta_model_;
    size_t meta_memory_size_;
    float meta_learning_rate_;
    float meta_momentum_;
    bool meta_predictive_;
    bool meta_adaptive_;
    
    // Internal helper methods
    std::vector<float> extractMetaFeatures(const MetaLearningEntry& entry) const;
    float computeMetaPrediction(const std::vector<float>& features,
                               const MetaLearningModel& model) const;
    void updateMetaModelWeights(const MetaLearningEntry& entry, float error,
                              const std::vector<float>& features);
    float computeMetaGradient(float prediction, float target, float learning_rate) const;
    void cleanupOldMetaExperiences();
};

} // namespace nlm
