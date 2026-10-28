// Phase 6 Demo Header
// Part of NLM (Neural Learning Machine) - Phase 6 Integration Demo
//
// This header declares the Phase6Demo class used in the integration test
// that verifies all brain systems are properly connected.

#pragma once

namespace nlm {

/**
 * Phase6Demo class
 * 
 * This class runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected and working together.
 * It demonstrates the final integration of all previous phases.
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run the complete integration demo
     * 
     * This method performs a comprehensive integration test that verifies:
     * - All memory systems (working, episodic, semantic, associative)
     * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
     * - Prediction systems (temporal prediction, action prediction)
     * - Cognitive systems (attention, planning, concept formation)
     * - Development systems (synaptogenesis, pruning, maturation)
     * - Checkpoint save/load functionality
     * - Replay and memory consolidation
     * 
     * @param neuronCount Number of neurons to use in the simulation
     * @param maxSteps Maximum simulation steps
     * @param enableCheckpointing Whether to test checkpoint functionality
     * @param enableReplay Whether to test memory replay
     * @param enableDevelopment Whether to test developmental processes
     */
    void run(
        size_t neuronCount = 500,
        size_t maxSteps = 2000,
        bool enableCheckpointing = true,
        bool enableReplay = true,
        bool enableDevelopment = true
    );
    
    /**
     * Run a quick integration verification
     * 
     * This method performs a lightweight check to verify that all
     * major brain systems are properly initialized and connected.
     * 
     * @return true if all systems are integrated, false otherwise
     */
    bool verifyIntegration();
    
    /**
     * Test individual system integration
     * 
     * Each method tests a specific subsystem:
     * - Memory integration (working + episodic memory)
     * - Neuromodulation integration (dopamine, curiosity, novelty)
     * - Checkpoint save/load functionality
     * - Memory replay and consolidation
     * 
     * @return true if test passes, false otherwise
     */
    bool testMemoryIntegration();
    bool testNeuromodulationIntegration();
    bool testCheckpointing();
    bool testReplay();
    
    /**
     * Get demo results and statistics
     * 
     * @return Total reward accumulated during demo
     */
    float getTotalReward() const { return totalReward; }
    
    /**
     * Get average firing rate during demo
     * 
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const { return avgFiringRate; }
    
    /**
     * Get number of episodes stored
     * 
     * @return Number of episodic memory episodes
     */
    float getMemoryEpisodesStored() const { return memoryEpisodesStored; }
    
    /**
     * Get neuromodulation level
     * 
     * @return Neuromodulation level (dopamine level)
     */
    float getDopamineLevel() const { return dopamineLevel; }
    
private:
    // Results and statistics
    float totalReward;
    float avgFiringRate;
    float memoryEpisodesStored;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    
    // Configuration
    size_t neuronCount;
    size_t maxSteps;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
};

} // namespace nlm