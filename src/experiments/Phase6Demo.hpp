#pragma once

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo provides a unified interface for running Phase 6 integration tests
 * with different levels of thoroughness.
 */

namespace nlm {

/**
 * Phase 6 Demo class provides integration testing capabilities
 * for the Phase 6 integrated brain systems.
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run quick integration verification tests
     * 
     * This runs essential integration tests to verify that the core
     * Phase 6 systems are connected and functional.
     * 
     * @return true if all quick tests pass, false otherwise
     */
    bool runQuickTests();
    
    /**
     * Run comprehensive integration and demonstration tests
     * 
     * This runs thorough integration tests covering all brain systems
     * including memory, neuromodulation, prediction, and development.
     * 
     * @return true if all comprehensive tests pass, false otherwise
     */
    bool runComprehensiveTests();
};

} // namespace nlm
