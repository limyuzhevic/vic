#pragma once

#include "Phase6IntegratedExperiment.hpp"

namespace nlm {

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    // Run the complete integration demo
    int run();
    
    // Verify integration without running full experiment
    bool verifyIntegration();
    
    // Test individual systems
    void testMemoryIntegration();
    void testNeuromodulationIntegration();
    void testCheckpointing();
    void testReplay();
    
    // Run full experiment with configuration
    Phase6IntegratedExperiment::ExperimentResult run(const Phase6Config& config);
    
private:
    Phase6IntegratedExperiment experiment_;
};

} // namespace nlm