// Phase 6 Demo header - Final integration demonstration of artificial brain
#pragma once

#include <string>

namespace nlm {

/**
 * Phase 6 Demo - Integration Test
 * 
 * Comprehensive demonstration of Phase 6 integration achievements:
 * - All memory systems connected to neural processing
 * - Neuromodulation affects neural dynamics and plasticity
 * - Prediction system integrated with learning
 * - Development affects plasticity rates
 * - Checkpoint save/load functionality
 * - Replay and consolidation systems operational
 * - Complete brain loop functions coherently
 * 
 * This demo runs a thorough integration test verifying that
 * all previously developed systems work together as a unified
 * artificial brain capable of learning and adaptation.
 */

class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run the Phase 6 integration demo
     * 
     * @param neuronCount Number of neurons for test (default: 500)
     * @param maxSteps Maximum simulation steps (default: 2000)
     * @param verbose Enable verbose output (default: true)
     */
    void run(size_t neuronCount = 500, size_t maxSteps = 2000, bool verbose = true);
    
    /**
     * Run integration verification only
     * 
     * @return true if all systems integrated, false otherwise
     */
    bool verifyIntegration();
    
    /**
     * Run individual system tests
     * 
     * @param system System to test ("memory", "neuromodulation", "checkpoint", "replay", "development")
     * @return true if test passes, false otherwise
     */
    bool testSystem(const std::string& system);
    
    /**
     * Get demo results and statistics
     * 
     * @return String containing comprehensive results and integration status
     */
    std::string getResults() const;
    
    /**
     * Get integration status report
     * 
     * @return String containing detailed integration status
     */
    std::string getIntegrationStatus() const;
    
    /**
     * Check if demo completed successfully
     * 
     * @return true if all tests passed, false otherwise
     */
    bool isCompleted() const;
    
    /**
     * Reset demo state
     */
    void reset();
    
    /**
     * Get demo metrics
     * 
     * @return String containing performance metrics
     */
    std::string getMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
