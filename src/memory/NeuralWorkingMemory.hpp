#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention

// Advanced query builder for working memory
class WorkingMemoryQueryBuilder {
public:
    WorkingMemoryQueryBuilder(const NeuralWorkingMemory* wm) : wm_(wm) {}
    
    // Query by activation threshold
    std::vector<NeuronId> aboveThreshold(float threshold) const {
        std::vector<NeuronId> results;
        for (size_t i = 0; i < wm_->getMemoryNeuronCount(); ++i) {
            if (wm_->getMemoryActivations()[i] > threshold) {
                results.push_back(wm_->getMemoryNeurons()[i]);
            }
        }
        return results;
    }
    
    // Query by time since last update
    std::vector<NeuronId> updatedAfter(SimulationStep minTime) const {
        std::vector<NeuronId> results;
        const auto& timestamps = wm_->getMemoryTimestamps();
        for (size_t i = 0; i < timestamps.size(); ++i) {
            if (timestamps[i] >= minTime) {
                results.push_back(wm_->getMemoryNeurons()[i]);
            }
        }
        return results;
    }
    
    // Get top-k activations
    std::vector<std::pair<NeuronId, float>> getTopKActivations(size_t k) const {
        std::vector<std::pair<NeuronId, float>> results;
        const auto& neurons = wm_->getMemoryNeurons();
        const auto& activations = wm_->getMemoryActivations();
        
        for (size_t i = 0; i < std::min(k, neurons.size()); ++i) {
            size_t idx = std::max_element(
                activations.begin(), activations.end()) - activations.begin();
            results.emplace_back(neurons[idx], activations[idx]);
            activations[idx] = -1.0f; // Prevent duplicates
        }
        return results;
    }
    
    // Get activity statistics
    struct ActivityStats {
        float min = 0.0f;
        float max = 0.0f;
        float mean = 0.0f;
        float stdDev = 0.0f;
        size_t activeTraces = 0;
        size_t totalNeurons = 0;
    };
    
    ActivityStats getActivityStats() const {
        ActivityStats stats;
        const auto& activations = wm_->getMemoryActivations();
        if (activations.empty()) return stats;
        
        float sum = 0.0f, sumSq = 0.0f;
        for (float act : activations) {
            if (act > 0.0f) {
                stats.activeTraces++;
            }
            if (act < stats.min || stats.totalNeurons == 0) stats.min = act;
            if (act > stats.max) stats.max = act;
            sum += act;
            sumSq += act * act;
        }
        
        stats.totalNeurons = activations.size();
        stats.mean = sum / stats.totalNeurons;
        if (stats.totalNeurons > 1) {
            stats.stdDev = std::sqrt((sumSq - sum * sum / stats.totalNeurons) / (stats.totalNeurons - 1));
        }
        
        return stats;
    }
    
    // Check if memory contains specific pattern
    bool containsPattern(const std::vector<float>& pattern, float tolerance = 0.1f) const {
        if (pattern.size() != wm_->getMemoryNeuronCount()) return false;
        const auto& activations = wm_->getMemoryActivations();
        
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (std::abs(activations[i] - pattern[i]) > tolerance) {
                return false;
            }
        }
        return true;
    }
    
    // Get memory usage statistics
    struct MemoryStats {
        size_t memoryNeuronCount = 0;
        size_t activeTraceCount = 0;
        float memoryUtilization = 0.0f;
        float decayRate = 0.0f;
        size_t capacity = 0;
        double memoryEfficiency = 0.0f;
    };
    
    MemoryStats getMemoryStats() const {
        MemoryStats stats;
        stats.memoryNeuronCount = wm_->getMemoryNeuronCount();
        stats.activeTraceCount = wm_->getActiveTraces();
        stats.decayRate = wm_->getDecayRate();
        stats.capacity = wm_->getCapacity();
        
        if (stats.capacity > 0) {
            stats.memoryUtilization = static_cast<float>(stats.memoryNeuronCount) / stats.capacity;
            // Calculate efficiency based on active traces vs total neurons
            stats.memoryEfficiency = static_cast<double>(stats.activeTraceCount) / stats.memoryNeuronCount;
        }
        
        return stats;
    }
    
    // Export memory state for analysis
    std::string exportMemoryState() const {
        std::ostringstream oss;
        oss << "Working Memory State:" << std::endl;
        oss << "  Neurons: " << wm_->getMemoryNeuronCount() << std::endl;
        oss << "  Active Traces: " << wm_->getActiveTraces() << std::endl;
        oss << "  Capacity: " << wm_->getCapacity() << std::endl;
        oss << "  Decay Rate: " << wm_->getDecayRate() << std::endl;
        oss << "  Total Activations: " << wm_->getMemoryActivations().size() << std::endl;
        
        const auto& neurons = wm_->getMemoryNeurons();
        const auto& activations = wm_->getMemoryActivations();
        
        oss << "  Top 5 Activations:" << std::endl;
        for (size_t i = 0; i < std::min<size_t>(5, neurons.size()); ++i) {
            oss << "    Neuron " << neurons[i] << ": " << activations[i] << std::endl;
        }
        
        return oss.str();
    }
    
    // Compare with another working memory
    double compareWith(const NeuralWorkingMemory* other) const {
        const auto& thisActivations = wm_->getMemoryActivations();
        const auto& otherActivations = other->getMemoryActivations();
        
        if (thisActivations.size() != otherActivations.size()) {
            return -1.0; // Invalid comparison
        }
        
        float sumDiff = 0.0f, sumSqDiff = 0.0f;
        for (size_t i = 0; i < thisActivations.size(); ++i) {
            float diff = thisActivations[i] - otherActivations[i];
            sumDiff += std::abs(diff);
            sumSqDiff += diff * diff;
        }
        
        float mse = sumSqDiff / thisActivations.size();
        float mae = sumDiff / thisActivations.size();
        
        // Return similarity score (1.0 = identical, 0.0 = completely different)
        return std::exp(-mse);
    }
    
    // Get memory health indicators
    struct MemoryHealth {
        bool isHealthy = true;
        std::string healthIssues;
        float stabilityScore = 0.0f;
        float adaptabilityScore = 0.0f;
    };
    
    MemoryHealth assessHealth() const {
        MemoryHealth health;
        health.healthIssues = "";
        
        const auto& activations = wm_->getMemoryActivations();
        float totalActivity = 0.0f;
        size_t activeNeurons = 0;
        
        for (float act : activations) {
            totalActivity += act;
            if (act > 0.01f) activeNeurons++;
        }
        
        // Check for instability (high variance in activations)
        float variance = 0.0f;
        if (!activations.empty()) {
            float mean = totalActivity / activations.size();
            for (float act : activations) {
                variance += (act - mean) * (act - mean);
            }
            variance /= activations.size();
        }
        
        if (variance > 0.1f) {
            health.isHealthy = false;
            health.healthIssues += "High activity variance; ";
            health.stabilityScore = 1.0f - std::min(1.0f, variance);
        } else {
            health.stabilityScore = 1.0f;
        }
        
        // Check for low activity (potential decay)
        if (totalActivity < 0.1f && activeNeurons > 0) {
            health.isHealthy = false;
            health.healthIssues += "Low total activity; ";
            health.adaptabilityScore = totalActivity;
        } else if (totalActivity > 0.0f) {
            health.adaptabilityScore = std::min(1.0f, totalActivity / 10.0f);
        }
        
        return health;
    }

private:
    const NeuralWorkingMemory* wm_;
};

// Advanced episodic memory with improved capabilities
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store an episode with context
    void storeEpisode(const std::string& description,
                     const std::vector<float>& sensoryInput,
                     const std::vector<float>& motorOutput,
                     float reward,
                     SimulationStep timestamp);
    
    // Retrieve episodes by time range
    std::vector<EpisodeInfo> getEpisodesByTime(SimulationStep startTime,
                                              SimulationStep endTime) const;
    
    // Retrieve episodes by reward threshold
    std::vector<EpisodeInfo> getEpisodesByReward(float minReward,
                                                 float maxReward = std::numeric_limits<float>::max()) const;
    
    // Get most recent episodes
    std::vector<EpisodeInfo> getRecentEpisodes(size_t count) const;
    
    // Search episodes by keyword in description
    std::vector<EpisodeInfo> searchByDescription(const std::string& keyword) const;
    
    // Get episode statistics
    struct EpisodeStats {
        size_t totalEpisodes = 0;
        float totalReward = 0.0f;
        float avgReward = 0.0f;
        SimulationStep earliestTime = 0;
        SimulationStep latestTime = 0;
        size_t episodesWithContext = 0;
    };
    
    EpisodeStats getStats() const;
    
    // Episode replay functionality
    void replayEpisode(size_t episodeIndex, SimulationStep startTime = 0);
    
    // Clear old episodes
    void clearOldEpisodes(SimulationStep cutoffTime);
    
    // Export episodes for analysis
    std::string exportEpisodes() const;
    
    // Import episodes from backup
    bool importEpisodes(const std::string& filepath);
    
    // Get episodes for consolidation
    std::vector<EpisodeInfo> getEpisodesForConsolidation(float importanceThreshold = 0.5f) const;
    
    // Get memory efficiency metrics
    struct MemoryEfficiency {
        float storageEfficiency = 0.0f; // Actual vs allocated memory
        float retrievalSpeed = 0.0f;   // Time to retrieve episodes
        float consolidationRatio = 0.0f; // Episodes consolidated vs total
        float compressionRatio = 0.0f;  // Data compression achieved
    };
    
    MemoryEfficiency calculateEfficiency() const;
    
    // Advanced query builder for episodic memory
    class EpisodicMemoryQueryBuilder {
    public:
        EpisodicMemoryQueryBuilder(const NeuralEpisodicMemory* em) : em_(em) {}
        
        // Query by sensory input similarity
        std::vector<EpisodeInfo> similarToSensory(const std::vector<float>& input,
                                                 float similarityThreshold = 0.8f) const;
        
        // Query by motor output pattern
        std::vector<EpisodeInfo> matchingMotorOutput(const std::vector<float>& output,
                                                     float threshold = 0.9f) const;
        
        // Get reward distribution
        struct RewardDistribution {
            std::map<float, size_t> bins;
            float minReward = 0.0f;
            float maxReward = 0.0f;
            float mean = 0.0f;
            float stdDev = 0.0f;
        };
        
        RewardDistribution getRewardDistribution() const;
        
        // Get temporal patterns
        struct TemporalPattern {
            std::map<SimulationStep, size_t> stepsPerEpisode;
            SimulationStep avgStepsPerEpisode = 0;
            SimulationStep minSteps = 0;
            SimulationStep maxSteps = 0;
        };
        
        TemporalPattern getTemporalPattern() const;
        
        // Analyze sequence coherence
        struct SequenceCoherence {
            float coherenceScore = 0.0f; // How well actions follow patterns
            std::map<std::string, float> patternStrengths;
            bool hasRecurrence = false;
            size_t recurrenceCount = 0;
        };
        
        SequenceCoherence analyzeSequenceCoherence() const;
        
    private:
        const NeuralEpisodicMemory* em_;
    };
    
    // Get query builder
    EpisodicMemoryQueryBuilder createQueryBuilder() const {
        return EpisodicMemoryQueryBuilder(this);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced associative memory with advanced capabilities
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store pattern association
    void storeAssociation(const std::vector<float>& key,
                         const std::vector<float>& value,
                         float strength = 1.0f);
    
    // Retrieve value by key
    std::vector<float> retrieveByKey(const std::vector<float>& key) const;
    
    // Retrieve multiple associations by partial key
    std::vector<std::pair<std::vector<float>, std::vector<float>>> retrieveByPartialKey(
        const std::vector<float>& partialKey,
        float threshold = 0.8f) const;
    
    // Get all associations for a key pattern
    std::vector<std::pair<std::vector<float>, std::vector<float>>> getAllAssociations(
        const std::vector<float>& keyPattern) const;
    
    // Remove association
    bool removeAssociation(const std::vector<float>& key);
    
    // Clear all associations
    void clear();
    
    // Get association statistics
    struct AssociationStats {
        size_t totalAssociations = 0;
        size_t uniqueKeys = 0;
        float avgStrength = 0.0f;
        float maxStrength = 0.0f;
        size_t keySize = 0;
        size_t valueSize = 0;
    };
    
    AssociationStats getStats() const;
    
    // Pattern completion
    std::vector<float> completePattern(const std::vector<float>& partialPattern) const;
    
    // Get similar patterns
    std::vector<std::pair<std::vector<float>, std::vector<float>>> getSimilarPatterns(
        const std::vector<float>& pattern,
        float threshold = 0.7f) const;
    
    // Export associations for analysis
    std::string exportAssociations() const;
    
    // Import associations from backup
    bool importAssociations(const std::string& filepath);
    
    // Get associations by strength threshold
    std::vector<std::pair<std::vector<float>, std::vector<float>>> getAssociationsByStrength(
        float minStrength) const;
    
    // Get strongest associations
    std::vector<std::pair<std::vector<float>, std::vector<float>>> getStrongestAssociations(
        size_t count) const;
    
    // Calculate pattern similarity
    float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                    const std::vector<float>& pattern2) const;
    
    // Get pattern statistics
    struct PatternStats {
        size_t totalPatterns = 0;
        float avgPatternSize = 0.0f;
        float avgValueSize = 0.0f;
        std::map<size_t, size_t> patternSizeDistribution;
    };
    
    PatternStats getPatternStats() const;
    
    // Advanced query builder for associative memory
    class AssociativeMemoryQueryBuilder {
    public:
        AssociativeMemoryQueryBuilder(const NeuralAssociativeMemory* am) : am_(am) {}
        
        // Query by key similarity
        std::vector<std::pair<std::vector<float>, std::vector<float>>> findSimilarKeys(
            const std::vector<float>& key,
            float threshold = 0.8f) const;
        
        // Query by value similarity
        std::vector<std::pair<std::vector<float>, std::vector<float>>> findSimilarValues(
            const std::vector<float>& value,
            float threshold = 0.8f) const;
        
        // Get associations with high strength
        std::vector<std::pair<std::vector<float>, std::vector<float>>> getHighStrengthAssociations(
            float minStrength) const;
        
        // Get sparse associations (low strength)
        std::vector<std::pair<std::vector<float>, std::vector<float>>> getSparseAssociations(
            float maxStrength) const;
        
        // Pattern completion with uncertainty
        std::vector<std::pair<std::vector<float>, float>> completeWithUncertainty(
            const std::vector<float>& partialPattern) const;
        
    private:
        const NeuralAssociativeMemory* am_;
    };
    
    // Get query builder
    AssociativeMemoryQueryBuilder createQueryBuilder() const {
        return AssociativeMemoryQueryBuilder(this);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory consolidation system
class MemoryConsolidation {
public:
    MemoryConsolidation();
    ~MemoryConsolidation();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Consolidate working memory into episodic memory
    void consolidateWorkingMemoryToEpisodic(float importanceThreshold = 0.5f);
    
    // Consolidate episodic memory (extract patterns)
    void consolidateEpisodicToAssociative(float patternStrengthThreshold = 0.6f);
    
    // Reconstruct memories from associations
    void reconstructFromAssociations(const std::vector<float>& cuePattern,
                                     std::vector<std::vector<float>>& reconstructedMemories);
    
    // Get consolidation statistics
    struct ConsolidationStats {
        size_t workingToEpisodicConsolidations = 0;
        size_t episodicToAssociativeConsolidations = 0;
        float consolidationEfficiency = 0.0f;
        float memoryStability = 0.0f;
        std::vector<std::string> consolidationIssues;
    };
    
    ConsolidationStats getStats() const;
    
    // Enable/disable consolidation
    void enableConsolidation(bool enable) { enabled_ = enable; }
    bool isConsolidationEnabled() const { return enabled_; }
    
    // Set consolidation parameters
    void setWorkingToEpisodicThreshold(float threshold) { workingToEpisodicThreshold_ = threshold; }
    void setEpisodicToAssociativeThreshold(float threshold) { episodicToAssociativeThreshold_ = threshold; }
    
    // Get consolidation metrics
    struct ConsolidationMetrics {
        std::chrono::duration<double> totalTime = std::chrono::duration<double>::zero();
        uint64_t operationsCount = 0;
        double successRate = 0.0;
        std::map<std::string, double> operationTimes;
    };
    
    ConsolidationMetrics getMetrics() const;
    
    // Reset metrics
    void resetMetrics() { metrics_ = ConsolidationMetrics(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool enabled_ = true;
    float workingToEpisodicThreshold_ = 0.5f;
    float episodicToAssociativeThreshold_ = 0.6f;
    ConsolidationMetrics metrics_;
};

// Memory management and optimization utilities
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Optimize memory usage
    void optimizeMemoryUsage();
    
    // Reallocate memory efficiently
    void reallocateMemory(size_t newCapacity);
    
    // Compact memory (remove weak traces)
    void compactMemory(float decayThreshold = 0.1f);
    
    // Get memory usage statistics
    struct MemoryUsageStats {
        size_t workingMemoryUsage = 0;
        size_t episodicMemoryUsage = 0;
        size_t associativeMemoryUsage = 0;
        size_t totalMemoryUsage = 0;
        float memoryEfficiency = 1.0f;
        std::chrono::duration<double> lastCompaction = std::chrono::duration<double>::zero();
    };
    
    MemoryUsageStats getMemoryUsageStats() const;
    
    // Get memory allocation recommendations
    std::vector<std::string> getAllocationRecommendations() const;
    
    // Monitor memory performance
    struct MemoryPerformance {
        double accessTime = 0.0;  // Average time to access memory
        double consolidationTime = 0.0;  // Average consolidation time
        double reconstructionTime = 0.0; // Average reconstruction time
        float hitRate = 0.0f;     // Memory retrieval success rate
        float storageUtilization = 0.0f; // How much of allocated memory is used
    };
    
    MemoryPerformance getMemoryPerformance() const;
    
    // Set memory parameters
    void setWorkingMemoryCapacity(size_t capacity) { workingMemoryCapacity_ = capacity; }
    void setEpisodicMemoryLimit(size_t limit) { episodicMemoryLimit_ = limit; }
    void setAssociationCacheSize(size_t size) { associationCacheSize_ = size; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    size_t workingMemoryCapacity_ = 1000;
    size_t episodicMemoryLimit_ = 10000;
    size_t associationCacheSize_ = 5000;
};

} // namespace nlm

} // namespace nlm