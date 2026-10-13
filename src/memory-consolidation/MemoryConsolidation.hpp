#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace nlm {

/**
 * MemoryConsolidation: System for consolidating working memory into episodic memory
 * 
 * Key mechanisms:
 * - Detects when working memory patterns have stabilized
 * - Sends selected patterns to episodic memory for long-term storage
 * - Applies relevance-based filtering to select important memories
 * - Integrates with neuromodulation for consolidation timing
 * - Operates periodically during awake phases
 * 
 * Integration:
 * - Works with working memory to detect consolidation opportunities
 * - Communicates with episodic memory to store consolidated memories
 * - Can be triggered by neuromodulatory signals
 * - Operates independently of sleep replay
 */

class MemoryConsolidation {
public:
    MemoryConsolidation();
    ~MemoryConsolidation();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update consolidation system (called periodically)
    // Should be called every N simulation steps
    void update(SimulationStep currentStep);
    
    // Check if consolidation should occur based on working memory stability
    // Returns true if consolidation should happen this step
    bool shouldConsolidate(SimulationStep currentStep) const;
    
    // Perform consolidation of working memory to episodic memory
    // Selected patterns are sent to episodic memory for storage
    void consolidateWorkingMemoryToEpisodic(SimulationStep currentStep);
    
    // Set consolidation interval (how often to consolidate)
    void setConsolidationInterval(size_t interval) { consolidationInterval_ = interval; }
    
    // Set stability threshold for triggering consolidation
    void setStabilityThreshold(float threshold) { stabilityThreshold_ = threshold; }
    
    // Enable/disable consolidation
    void enable(bool enable) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }
    
    // Get number of consolidations performed
    size_t getConsolidationCount() const { return consolidationCount_; }
    
    // Get working memory patterns that are candidates for consolidation
    std::vector<float> getConsolidationCandidates(SimulationStep currentStep) const;
    
    // Force consolidation regardless of stability
    void forceConsolidation(SimulationStep currentStep);
    
    // Check if system is ready (has brain reference)
    bool isReady() const { return brain_ != nullptr; }
    
    // Get current consolidation rate
    float getConsolidationRate() const;
    
    // Reset consolidation state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Consolidation parameters
    size_t consolidationInterval_;
    float stabilityThreshold_;
    bool enabled_;
    
    // Statistics
    size_t consolidationCount_;
    size_t stepsSinceLastConsolidation_;
    
    // Stability tracking for working memory patterns
    std::unordered_map<size_t, float> patternStability_;
    std::unordered_map<size_t, SimulationStep> patternLastUpdated_;
    
    // Relevance scoring for patterns
    float computePatternRelevance(const std::vector<float>& pattern) const;
    bool isPatternStable(size_t neuronId) const;
    float getPatternStability(size_t neuronId) const;
};

} // namespace nlm