#include "AdvancedMemory.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>
#include <numeric>
#include <limits>

namespace nlm {

void HierarchicalMemory::store(const std::vector<float>& pattern, float strength, NeuronId source,
                              SimulationStep timestamp) {
    if (pattern.empty()) return;
    
    // Create new memory node
    MemoryNode* new_node = new MemoryNode(timestamp, source, pattern, strength);
    
    // Insert into hierarchical structure
    insertMemoryNode(new_node);
    
    // Update interference tracking
    interference_heap_.push(new_node);
    
    // Update neuron memories
    neuron_memories_[source].push_back(new_node);
    
    // Maintain memory size limit
    cleanupInactiveNodes();
}

std::vector<float> HierarchicalMemory::retrieve(SimulationStep current_timestamp,
                                                float temporal_window,
                                                const std::vector<float>& context) {
    std::vector<float> result;
    
    if (root_node_ == nullptr) return result;
    
    // Collect candidate nodes within temporal window
    std::vector<MemoryNode*> candidates;
    collectTemporalCandidates(root_node_, current_timestamp, temporal_window, candidates);
    
    // Filter by context if provided
    if (!context.empty()) {
        std::vector<MemoryNode*> context_filtered;
        for (MemoryNode* node : candidates) {
            float similarity = calculatePatternSimilarity(node->pattern, context);
            if (similarity > interference_threshold_) {
                context_filtered.push_back(node);
            }
        }
        candidates = context_filtered;
    }
    
    // Sort by stability and strength
    std::sort(candidates.begin(), candidates.end(),
              [](MemoryNode* a, MemoryNode* b) {
                  return (a->stability * a->strength) > (b->stability * b->strength);
              });
    
    // Combine patterns from top candidates
    size_t combine_count = std::min(static_cast<size_t>(10), candidates.size());
    for (size_t i = 0; i < combine_count; ++i) {
        MemoryNode* node = candidates[i];
        
        // Apply temporal decay
        float time_since = current_timestamp - node->timestamp;
        float decay_factor = std::exp(-time_since / node->decay_rate);
        
        float effective_strength = node->strength * node->stability * decay_factor;
        
        // Add to result
        for (size_t j = 0; j < node->pattern.size(); ++j) {
            if (j >= result.size()) {
                result.push_back(node->pattern[j] * effective_strength);
            } else {
                result[j] += node->pattern[j] * effective_strength;
            }
        }
    }
    
    return result;
}

void HierarchicalMemory::consolidate(SimulationStep current_step) {
    if (root_node_ == nullptr) return;
    
    // Check if consolidation should occur
    if ((current_step - last_consolidation_step_) >= consolidation_interval_) {
        // Perform consolidation at each level
        consolidateLevel(root_node_, current_step);
        last_consolidation_step_ = current_step;
    }
}

void HierarchicalMemory::interfere(MemoryNode* node1, MemoryNode* node2) {
    if (!node1 || !node2) return;
    
    float interference = calculateInterference(node1, node2);
    
    // Update interference scores
    node1->interference_score = std::max(node1->interference_score, interference);
    node2->interference_score = std::max(node2->interference_score, interference);
    
    // Update interference signatures
    std::vector<float> sig1 = node1->interference_signature;
    std::vector<float> sig2 = node2->interference_signature;
    
    if (!sig1.empty() && !sig2.empty()) {
        float sig_similarity = calculatePatternSimilarity(sig1, sig2);
        interference_heap_.push(node1);
        interference_heap_.push(node2);
    }
}

HierarchicalMemory::MemoryStats HierarchicalMemory::getMemoryStats() const {
    MemoryStats stats;
    
    // Collect all nodes via DFS traversal
    std::vector<MemoryNode*> all_nodes;
    collectAllNodes(root_node_, all_nodes);
    
    stats.total_nodes = all_nodes.size();
    
    if (stats.total_nodes > 0) {
        // Calculate active nodes (within time window)
        SimulationStep current_time = std::chrono::duration<double>().count();
        for (MemoryNode* node : all_nodes) {
            if (node && (current_time - node->last_access) < consolidation_interval_) {
                stats.active_nodes++;
            }
            
            stats.average_stability += node->stability;
            node->interference_signature = node->pattern;  // Simplified signature
        }
        
        stats.average_stability /= stats.total_nodes;
        
        // Calculate interference level
        float max_interference = 0.0f;
        for (MemoryNode* node : all_nodes) {
            max_interference = std::max(max_interference, node->interference_score);
        }
        stats.interference_level = max_interference;
        
        // Count nodes per level
        std::unordered_map<size_t, size_t> level_counts;
        for (MemoryNode* node : all_nodes) {
            level_counts[node->hierarchy_level]++;
        }
        
        stats.nodes_per_level.resize(hierarchy_depth_, 0);
        for (const auto& pair : level_counts) {
            if (pair.first < stats.nodes_per_level.size()) {
                stats.nodes_per_level[pair.first] = pair.second;
            }
        }
    }
    
    return stats;
}

void HierarchicalMemory::insertMemoryNode(MemoryNode* node) {
    if (!node) return;
    
    // Find appropriate parent based on hierarchy level
    if (root_node_ == nullptr) {
        root_node_ = node;
        return;
    }
    
    // Find insertion point
    MemoryNode* current = root_node_;
    while (current != nullptr) {
        if (node->hierarchy_level <= current->hierarchy_level) {
            // Insert as child
            node->parent = current;
            current->children.push_back(node);
            return;
        }
        
        // Try to find suitable child
        bool inserted = false;
        for (MemoryNode* child : current->children) {
            if (node->hierarchy_level <= child->hierarchy_level + 1) {
                insertMemoryNode(node, child);
                inserted = true;
                break;
            }
        }
        
        if (!inserted) {
            // Create new branch
            MemoryNode* new_branch = new MemoryNode(*node);
            new_branch->parent = current;
            current->children.push_back(new_branch);
            return;
        }
        
        current = current->children.empty() ? nullptr : current->children[0];
    }
}

void HierarchicalMemory::insertMemoryNode(MemoryNode* node, MemoryNode* parent) {
    if (!node || !parent) return;
    
    node->parent = parent;
    parent->children.push_back(node);
}

MemoryNode* HierarchicalMemory::findSimilarNode(const std::vector<float>& pattern, size_t level) const {
    if (root_node_ == nullptr) return nullptr;
    
    // DFS search for similar node
    std::vector<MemoryNode*> candidates;
    collectSimilarNodes(root_node_, pattern, level, candidates);
    
    if (candidates.empty()) return nullptr;
    
    // Return most similar node
    return candidates[0];
}

HierarchicalMemory::ConsolidationResult HierarchicalMemory::consolidateLevel(MemoryNode* level_node,
                                                                         SimulationStep current_step) {
    ConsolidationResult result;
    
    if (!level_node) return result;
    
    // Check if node should be consolidated
    bool should_consolidate = level_node->strength >= 0.5f && 
                             level_node->stability >= 0.7f;
    
    if (!should_consolidate) {
        return result;
    }
    
    // Consolidate with similar nodes
    std::vector<MemoryNode*> similar_nodes;
    for (MemoryNode* child : level_node->children) {
        float similarity = calculatePatternSimilarity(level_node->pattern, child->pattern);
        if (similarity > 0.8f) {
            similar_nodes.push_back(child);
        }
    }
    
    if (similar_nodes.empty()) {
        return result;
    }
    
    // Merge similar nodes
    float new_strength = level_node->strength;
    std::vector<float> merged_pattern;
    
    for (MemoryNode* node : similar_nodes) {
        new_strength += node->strength;
        
        if (merged_pattern.empty()) {
            merged_pattern = node->pattern;
        } else {
            for (size_t i = 0; i < std::min(merged_pattern.size(), node->pattern.size()); ++i) {
                merged_pattern[i] = (merged_pattern[i] + node->pattern[i]) * 0.5f;
            }
        }
        
        // Promote children to parent
        for (MemoryNode* grandchild : node->children) {
            level_node->children.push_back(grandchild);
            grandchild->parent = level_node;
        }
        
        // Remove node
        auto it = std::find(level_node->children.begin(), level_node->children.end(), node);
        if (it != level_node->children.end()) {
            level_node->children.erase(it);
            delete node;
        }
    }
    
    // Update node with merged properties
    if (!merged_pattern.empty()) {
        level_node->pattern = merged_pattern;
    }
    level_node->strength = new_strength;
    level_node->stability = std::min(1.0f, level_node->stability + 0.1f);
    
    result.was_consolidated = true;
    result.new_stability = level_node->stability;
    result.merged_nodes = similar_nodes;
    
    return result;
}

float HierarchicalMemory::calculateInterference(const MemoryNode* node1, const MemoryNode* node2) const {
    if (!node1 || !node2) return 0.0f;
    
    // Calculate pattern similarity
    float pattern_similarity = calculatePatternSimilarity(node1->pattern, node2->pattern);
    
    // Calculate interference based on similarity and strength
    float interference = pattern_similarity * 
                         std::min(node1->strength, node2->strength) *
                         std::min(node1->stability, node2->stability);
    
    return interference;
}

void HierarchicalMemory::cleanupInactiveNodes() {
    if (root_node_ == nullptr) return;
    
    SimulationStep current_time = std::chrono::duration<double>().count();
    
    // Remove nodes that exceed memory size and are inactive
    std::vector<MemoryNode*> all_nodes;
    collectAllNodes(root_node_, all_nodes);
    
    // Sort by stability and strength (keep most important)
    std::sort(all_nodes.begin(), all_nodes.end(),
              [](MemoryNode* a, MemoryNode* b) {
                  return (a->stability * a->strength) > (b->stability * b->strength);
              });
    
    // Remove excess nodes (keeping most stable/strong)
    size_t keep_count = std::min(max_memory_size_, all_nodes.size());
    std::vector<MemoryNode*> nodes_to_delete;
    
    for (size_t i = keep_count; i < all_nodes.size(); ++i) {
        // Remove from parent's children list
        MemoryNode* node = all_nodes[i];
        if (node->parent) {
            auto it = std::find(node->parent->children.begin(), 
                              node->parent->children.end(), node);
            if (it != node->parent->children.end()) {
                node->parent->children.erase(it);
            }
        }
        
        // Remove from neuron memories
        for (auto& pair : neuron_memories_) {
            auto it = std::find(pair.second.begin(), pair.second.end(), node);
            if (it != pair.second.end()) {
                pair.second.erase(it);
            }
        }
        
        nodes_to_delete.push_back(node);
    }
    
    // Delete nodes
    for (MemoryNode* node : nodes_to_delete) {
        delete node;
    }
}

float HierarchicalMemory::calculatePatternSimilarity(const std::vector<float>& pat1,
                                                    const std::vector<float>& pat2) const {
    if (pat1.empty() || pat2.empty()) return 0.0f;
    
    size_t min_size = std::min(pat1.size(), pat2.size());
    if (min_size == 0) return 0.0f;
    
    float dot_product = 0.0f;
    for (size_t i = 0; i < min_size; ++i) {
        dot_product += pat1[i] * pat2[i];
    }
    
    float norm1 = std::sqrt(std::inner_product(pat1.begin(), pat1.end(), pat1.begin(), 0.0f));
    float norm2 = std::sqrt(std::inner_product(pat2.begin(), pat2.end(), pat2.begin(), 0.0f));
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dot_product / (norm1 * norm2);
}

void HierarchicalMemory::collectTemporalCandidates(MemoryNode* node, SimulationStep current_timestamp,
                                                   float temporal_window,
                                                   std::vector<MemoryNode*>& candidates) const {
    if (!node) return;
    
    // Check if node is within temporal window
    float time_since = current_timestamp - node->timestamp;
    if (time_since <= temporal_window) {
        candidates.push_back(node);
    }
    
    // Recursively collect from children
    for (MemoryNode* child : node->children) {
        collectTemporalCandidates(child, current_timestamp, temporal_window, candidates);
    }
}

void HierarchicalMemory::collectAllNodes(MemoryNode* node, std::vector<MemoryNode*>& all_nodes) const {
    if (!node) return;
    
    all_nodes.push_back(node);
    
    for (MemoryNode* child : node->children) {
        collectAllNodes(child, all_nodes);
    }
}

void HierarchicalMemory::collectSimilarNodes(MemoryNode* node, const std::vector<float>& pattern,
                                            size_t level,
                                            std::vector<MemoryNode*>& candidates) const {
    if (!node) return;
    
    if (node->hierarchy_level == level) {
        float similarity = calculatePatternSimilarity(node->pattern, pattern);
        if (similarity > interference_threshold_) {
            candidates.push_back(node);
        }
    }
    
    for (MemoryNode* child : node->children) {
        collectSimilarNodes(child, pattern, level, candidates);
    }
}

void TemporalMemory::storeSequence(const std::vector<std::vector<float>>& sequence,
                                   const std::vector<Timestamp>& timestamps) {
    if (sequence.empty() || timestamps.size() != sequence.size()) {
        return;
    }
    
    TemporalSequence new_sequence;
    new_sequence.sequence_patterns = sequence;
    new_sequence.timestamps = timestamps;
    
    // Compute statistics
    new_sequence.pattern_frequencies.resize(sequence.size(), 0);
    for (size_t i = 0; i < sequence.size(); ++i) {
        new_sequence.pattern_frequencies[i] = 1;  // Initially 1 occurrence
    }
    
    // Predict completion based on pattern analysis
    new_sequence.predicted_completion = timestamps.empty() ? 0.0f : 
                                       timestamps.back() + (sequence.size() * 1.0f);
    new_sequence.completion_probability = 0.5f;  // Default probability
    
    sequences_.push_back(new_sequence);
    
    // Maintain sequence memory size
    if (sequences_.size() > sequence_memory_size_) {
        sequences_.erase(sequences_.begin());
    }
}

std::vector<float> TemporalMemory::predictNextPattern(const PredictionContext& context) {
    if (sequences_.empty() || context.current_state.empty()) {
        return {};
    }
    
    // Find most similar sequence to current context
    std::vector<TemporalSequence> candidates;
    for (const TemporalSequence& seq : sequences_) {
        if (seq.sequence_patterns.empty()) continue;
        
        // Compute context similarity
        float similarity = computeSequenceContext(seq.sequence_patterns);
        
        if (similarity > pattern_matching_threshold_) {
            candidates.push_back(seq);
        }
    }
    
    if (candidates.empty()) {
        return {};
    }
    
    // Select the most likely next pattern
    std::vector<float> predicted_pattern;
    float max_probability = 0.0f;
    
    for (const TemporalSequence& seq : candidates) {
        if (seq.sequence_patterns.size() > 1) {
            // Predict next pattern based on sequence patterns
            std::vector<float> last_pattern = seq.sequence_patterns.back();
            
            // Apply some transformation (simplified)
            std::vector<float> next_pattern = last_pattern;
            for (float& val : next_pattern) {
                val *= (1.0f + context.prediction_confidence);
            }
            
            if (seq.completion_probability > max_probability) {
                max_probability = seq.completion_probability;
                predicted_pattern = next_pattern;
            }
        }
    }
    
    return predicted_pattern;
}

void TemporalMemory::updatePredictions(const PredictionContext& context,
                                      const std::vector<float>& actual_next) {
    if (actual_next.empty()) return;
    
    // Update all sequences with actual next pattern
    for (auto& sequence : sequences_) {
        if (sequence.sequence_patterns.empty()) continue;
        
        // Update sequence statistics
        updateSequenceStats(sequence, actual_next);
        
        // Adjust predictions based on actual outcome
        float prediction_error = computePredictionError(actual_next, predictNextPattern(context));
        
        // If error is large, reduce confidence
        if (prediction_error > 0.5f) {
            sequence.completion_probability *= 0.8f;
        }
    }
}

float TemporalMemory::computePredictionError(const std::vector<float>& predicted,
                                            const std::vector<float>& actual) const {
    if (predicted.empty() || actual.empty()) return 0.0f;
    
    size_t min_size = std::min(predicted.size(), actual.size());
    float error = 0.0f;
    
    for (size_t i = 0; i < min_size; ++i) {
        float diff = predicted[i] - actual[i];
        error += diff * diff;
    }
    
    return std::sqrt(error / min_size);
}

TemporalMemory::TemporalStats TemporalMemory::getTemporalStats() const {
    TemporalStats stats;
    
    if (sequences_.empty()) return stats;
    
    stats.total_sequences = sequences_.size();
    stats.active_sequences = 0;
    
    float total_accuracy = 0.0f;
    float total_length = 0.0f;
    float total_confidence = 0.0f;
    
    for (const auto& sequence : sequences_) {
        // Count active sequences (recent)
        if (!sequence.timestamps.empty()) {
            Timestamp last_time = sequence.timestamps.back();
            Timestamp current_time = std::chrono::duration<double>().count();
            if (current_time - last_time < 1000.0f) {  // Within 1000 time units
                stats.active_sequences++;
            }
        }
        
        total_accuracy += sequence.completion_probability;
        total_length += sequence.sequence_patterns.size();
        total_confidence += sequence.predicted_completion;
    }
    
    stats.average_prediction_accuracy = total_accuracy / sequences_.size();
    stats.average_sequence_length = total_length / sequences_.size();
    stats.prediction_confidence = total_confidence / sequences_.size();
    
    return stats;
}

float TemporalMemory::computeSequenceContext(const std::vector<std::vector<float>>& sequence) const {
    if (sequence.empty()) return 0.0f;
    
    // Compute average pattern similarity
    float total_similarity = 0.0f;
    int comparisons = 0;
    
    for (size_t i = 0; i < sequence.size(); ++i) {
        for (size_t j = i + 1; j < sequence.size(); ++j) {
            float similarity = computePatternSimilarity(sequence[i], sequence[j]);
            total_similarity += similarity;
            comparisons++;
        }
    }
    
    if (comparisons == 0) return 0.0f;
    return total_similarity / comparisons;
}

float TemporalMemory::computePatternSimilarity(const std::vector<float>& pat1,
                                              const std::vector<float>& pat2) const {
    if (pat1.empty() || pat2.empty()) return 0.0f;
    
    size_t min_size = std::min(pat1.size(), pat2.size());
    if (min_size == 0) return 0.0f;
    
    float dot_product = 0.0f;
    for (size_t i = 0; i < min_size; ++i) {
        dot_product += pat1[i] * pat2[i];
    }
    
    float norm1 = std::sqrt(std::inner_product(pat1.begin(), pat1.end(), pat1.begin(), 0.0f));
    float norm2 = std::sqrt(std::inner_product(pat2.begin(), pat2.end(), pat2.begin(), 0.0f));
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dot_product / (norm1 * norm2);
}

void TemporalMemory::updateSequenceStats(TemporalSequence& sequence,
                                        const std::vector<float>& actual_next) {
    // Update sequence statistics (simplified)
    if (actual_next.empty()) return;
    
    // Add to sequence patterns
    sequence.sequence_patterns.push_back(actual_next);
    
    // Update completion probability
    sequence.completion_probability = std::min(1.0f, sequence.completion_probability * 1.1f);
    
    // Update predicted completion
    if (!sequence.timestamps.empty()) {
        Timestamp last_time = sequence.timestamps.back();
        Timestamp current_time = std::chrono::duration<double>().count();
        float elapsed = current_time - last_time;
        sequence.predicted_completion += elapsed + 1.0f;
    }
}

} // namespace nlm
