// test_main.cpp - Test framework entry point for Phase 6 integration tests
// Author: Kilo
// Purpose: Run all Phase 6 integration tests

#include "brain_test_common.hpp"

namespace nlm {
namespace test {

/**
 * Print Phase 6 test suite information
 */
void printTestInfo() {
    std::cout << "=== NLM Phase 6 Integration Test Suite ===" << std::endl;
    std::cout << "Testing the complete integrated artificial brain" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Test Categories:" << std::endl;
    std::cout << "  1. Brain Integration Tests" << std::endl;
    std::cout << "     - All integrated systems work together in Brain::step()" << std::endl;
    std::cout << "     - Memory systems properly exchange data" << std::endl;
    std::cout << "     - Prediction system trains and updates correctly" << std::endl;
    std::cout << "     - Neuromodulators properly affect neural computation" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  2. Memory System Tests" << std::endl;
    std::cout << "     - WorkingMemory receives and stores sensory input" << std::endl;
    std::cout << "     - EpisodicMemory integrates with sensory input and brain state" << std::endl;
    std::cout << "     - Memory consolidation works during rest periods" << std::endl;
    std::cout << "     - Replay mechanism functions properly" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  3. Neuromodulation Tests" << std::endl;
    std::cout << "     - Adenosine-based sleep/rest cycle works correctly" << std::endl;
    std::cout << "     - Dopamine effects on plasticity and attention" << std::endl;
    std::cout << "     - Acetylcholine effects on memory and attention" << std::endl;
    std::cout << "     - Other neuromodulators (NE, 5-HT) function" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  4. Cognitive System Tests" << std::endl;
    std::cout << "     - NeuralPlanner properly integrates with prediction system" << std::endl;
    std::cout << "     - ConceptFormation processes experiences correctly" << std::endl;
    std::cout << "     - Attention system integrates with working memory" << std::endl;
    std::cout << "     - Action selection uses all integrated systems" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  5. Development and Rest Tests" << std::endl;
    std::cout << "     - Rest cycle triggered automatically based on adenosine" << std::endl;
    std::cout << "     - Memory consolidation during rest" << std::endl;
    std::cout << "     - Development stage affects system behavior" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  6. Persistence Tests" << std::endl;
    std::cout << "     - Save/load properly restores all integrated states" << std::endl;
    std::cout << "     - Checkpoint system works with new neuromodulators" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  7. Stress Tests" << std::endl;
    std::cout << "     - High load and complex scenarios" << std::endl;
    std::cout << "     - Error conditions and recovery" << std::endl;
    std::cout << "     - Performance under stress" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Implementation Priority:" << std::endl;
    std::cout << "  Priority 1 (Critical): Brain, Memory, Neuromodulation integration tests" << std::endl;
    std::cout << "  Priority 2 (Important): Prediction, Cognition, Development integration tests" << std::endl;
    std::cout << "  Priority 3 (Comprehensive): Persistence, Stress, Cross-system integration tests" << std::endl;
}

/**
 * Run all Phase 6 integration tests
 */
int runAllPhase6Tests() {
    std::cout << "=== Running All Phase 6 Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    TestStats stats;
    
    // Create test configuration manager
    TestConfigManager configManager;
    
    // Print available test scenarios
    configManager.printAvailableScenarios();
    std::cout << std::endl;
    
    // Test 1: Brain integration tests
    std::cout << "=== BRAIN INTEGRATION TESTS ===" << std::endl;
    PerformanceTimer timer;
    
    timer.reset();
    bool brainIntegrationPassed = test::testBasicBrainIntegration();
    stats.addTest("Basic Brain Integration", brainIntegrationPassed, timer.elapsed());
    
    timer.reset();
    bool memoryDataExchangePassed = test::testMemorySystemDataExchange();
    stats.addTest("Memory System Data Exchange", memoryDataExchangePassed, timer.elapsed());
    
    // Test 2: Neuromodulation integration tests
    std::cout << "\n=== NEUROMODULATION INTEGRATION TESTS ===" << std::endl;
    
    timer.reset();
    bool adenosineRestPassed = test::testAdenosineRestCycle();
    stats.addTest("Adenosine Rest Cycle", adenosineRestPassed, timer.elapsed());
    
    timer.reset();
    bool dopamineEffectsPassed = test::testDopamineEffects();
    stats.addTest("Dopamine Effects", dopamineEffectsPassed, timer.elapsed());
    
    timer.reset();
    bool acetylcholineEffectsPassed = test::testAcetylcholineEffects();
    stats.addTest("Acetylcholine Effects", acetylcholineEffectsPassed, timer.elapsed());
    
    // Test 3: Memory system tests
    std::cout << "\n=== MEMORY SYSTEM TESTS ===" << std::endl;
    
    timer.reset();
    bool workingMemorySensoryPassed = test::testWorkingMemorySensoryIntegration();
    stats.addTest("Working Memory Sensory Integration", workingMemorySensoryPassed, timer.elapsed());
    
    timer.reset();
    bool episodicMemoryPassed = test::testEpisodicMemoryIntegration();
    stats.addTest("Episodic Memory Integration", episodicMemoryPassed, timer.elapsed());
    
    timer.reset();
    bool memoryConsolidationPassed = test::testMemoryConsolidationRest();
    stats.addTest("Memory Consolidation Rest", memoryConsolidationPassed, timer.elapsed());
    
    // Test 4: Prediction integration tests
    std::cout << "\n=== PREDICTION INTEGRATION TESTS ===" << std::endl;
    
    timer.reset();
    bool predictionSystemPassed = test::testPredictionSystemIntegration();
    stats.addTest("Prediction System Integration", predictionSystemPassed, timer.elapsed());
    
    timer.reset();
    bool predictionErrorPassed = test::testPredictionErrorIntegration();
    stats.addTest("Prediction Error Integration", predictionErrorPassed, timer.elapsed());
    
    // Test 5: Cognition integration tests
    std::cout << "\n=== COGNITION INTEGRATION TESTS ===" << std::endl;
    
    timer.reset();
    bool neuralPlannerPassed = test::testNeuralPlannerIntegration();
    stats.addTest("Neural Planner Integration", neuralPlannerPassed, timer.elapsed());
    
    timer.reset();
    bool conceptFormationPassed = test::testConceptFormationIntegration();
    stats.addTest("Concept Formation Integration", conceptFormationPassed, timer.elapsed());
    
    timer.reset();
    bool attentionSystemPassed = test::testAttentionSystemIntegration();
    stats.addTest("Attention System Integration", attentionSystemPassed, timer.elapsed());
    
    // Test 6: Development and rest tests
    std::cout << "\n=== DEVELOPMENT AND REST TESTS ===" << std::endl;
    
    timer.reset();
    bool restCyclePassed = test::testRestCycleAutomatic();
    stats.addTest("Rest Cycle Automatic", restCyclePassed, timer.elapsed());
    
    timer.reset();
    bool developmentStagePassed = test::testDevelopmentalStageEffects();
    stats.addTest("Developmental Stage Effects", developmentStagePassed, timer.elapsed());
    
    // Test 7: Persistence tests
    std::cout << "\n=== PERSISTENCE TESTS ===" << std::endl;
    
    timer.reset();
    bool saveLoadPassed = test::testSaveLoadRestoresAllStates();
    stats.addTest("Save/Load Restores All States", saveLoadPassed, timer.elapsed());
    
    timer.reset();
    bool checkpointNeuromodulatorsPassed = test::testCheckpointNeuromodulators();
    stats.addTest("Checkpoint System with Neuromodulators", checkpointNeuromodulatorsPassed, timer.elapsed());
    
    // Test 8: Stress tests
    std::cout << "\n=== STRESS TESTS ===" << std::endl;
    
    timer.reset();
    bool crossSystemStressPassed = test::testCrossSystemStress();
    stats.addTest("Cross-System Stress", crossSystemStressPassed, timer.elapsed());
    
    timer.reset();
    bool errorConditionsPassed = test::testErrorConditionsRecovery();
    stats.addTest("Error Conditions and Recovery", errorConditionsPassed, timer.elapsed());
    
    // Report final results
    stats.report();
    std::cout << std::endl;
    
    // Overall test results
    std::cout << "=== OVERALL TEST RESULTS ===" << std::endl;
    std::cout << "Total tests: " << stats.getTotal() << std::endl;
    std::cout << "Tests passed: " << stats.getPassed() << std::endl;
    std::cout << "Tests failed: " << stats.getFailed() << std::endl;
    std::cout << "Success rate: " << (stats.getTotal() > 0 ? 
        (stats.getPassed() * 100.0 / stats.getTotal()) : 0.0) << "%" << std::endl;
    std::cout << "Total time: " << stats.getTotalTime() << " seconds" << std::endl;
    
    if (stats.getFailed() == 0) {
        std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "\n=== SOME TESTS FAILED ===" << std::endl;
        std::cout << stats.getFailed() << " test(s) failed" << std::endl;
        return 1;
    }
}

} // namespace test
} // namespace nlm