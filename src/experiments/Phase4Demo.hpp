// Phase 4 Demo header - Comprehensive demonstration of Phase 4 cognitive mechanisms
#pragma once

#include <string>

namespace nlm {

/**
 * Phase 4 Demo
 * 
 * Demonstrates the emergence of cognitive capabilities in NLM:
 * - Temporal prediction and pattern recognition
 * - Working memory with delayed response tasks
 * - Episodic memory formation and recall
 * - Concept formation from recurring patterns
 * - Selective attention mechanisms
 * - Multi-step planning abilities
 * - Self-model development
 * - Social learning through observation
 * - Continual learning across tasks
 * 
 * This demo runs 10 separate experiments showing how these
cognitive capabilities emerge from neural dynamics without
explicit programming of rules or concepts.
 */

class Phase4Demo {
public:
    Phase4Demo();
    ~Phase4Demo();
    
    /**
     * Run the complete Phase 4 demonstration
     * 
     * @param trialsPerExperiment Number of trials per experiment (default: 50)
     * @param stepsPerEpisode Steps per episode for integrated experiment (default: 200)
     * @param verbose Enable verbose output (default: true)
     */
    void run(size_t trialsPerExperiment = 50, size_t stepsPerEpisode = 200, bool verbose = true);
    
    /**
     * Run individual experiments
     * 
     * @param experimentIndex Index of experiment to run (0-9 for all experiments)
     */
    void runExperiment(size_t experimentIndex);
    
    /**
     * Get experiment results
     * 
     * @return String containing experiment results and statistics
     */
    std::string getResults() const;
    
    /**
     * Check if demonstration completed successfully
     * 
     * @return true if all experiments completed, false otherwise
     */
    bool isCompleted() const;
    
    /**
     * Reset demonstration state
     */
    void reset();
    
    /**
     * Get demonstration statistics
     * 
     * @return String containing performance statistics
     */
    std::string getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
