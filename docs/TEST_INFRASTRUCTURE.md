/*!
 * Comprehensive Test Suite Documentation
 * =====================================
 *
 * This document describes the comprehensive test infrastructure implemented for NLM Phase 6.
 * The test suite validates all integration improvements and ensures system functionality.
 *
 * ## Overview
 *
 * The NLM (熙然) Phase 6 comprehensive test suite implements:
 * - **Unit Tests**: Testing individual components
 * - **Integration Tests**: Validating system interactions
 * - **End-to-End Tests**: Testing complete workflows
 * - **Performance Tests**: Benchmarking system performance
 * - **Regression Tests**: Preventing feature regressions
 *
 * ## Test Structure
 *
 * The test infrastructure is organized into:
 *
 * ### Core Test Framework (`tests/framework/`)
 * - `TestFramework.hpp`: Comprehensive test framework base classes
 * - `TestFramework.cpp`: Framework implementation
 * - Provides: TestResult structure, TestSuite base, IntegrationTestSuite,
 *   BenchmarkSuite, TestAssertions, TestRunner
 *
 * ### System-Specific Tests
 * - `test_runner.cpp`: Main test runner and comprehensive test suite
 * - Each system has dedicated test files for specific functionality
 *
 * ### Configuration System Tests
 * - Tests JSON/YAML parsing and validation
 * - Tests configuration inheritance and merging
 * - Tests schema validation and environment variable expansion
 *
 * ### Memory System Tests
 * - Tests working, episodic, semantic, and procedural memory
 * - Tests memory integration and consolidation
 * - Tests replay functionality and memory capacity
 *
 * ### Neuromodulation Tests
 * - Tests all neuromodulators: Dopamine, Curiosity, Novelty, Prediction Error
 * - Tests ACh, NE, 5-HT (Phase 2 additions)
 * - Tests neuromodulation integration with brain dynamics
 *
 * ### Prediction System Tests
 * - Tests prediction system integration
 * - Tests prediction accuracy and confidence tracking
 * - Tests prediction error computation
 *
 * ### Cognition System Tests
 * - Tests NeuralPlanner for action planning
 * - Tests ConceptFormation for pattern discovery
 * - Tests AttentionalSelection for focus management
 *
 * ### Development System Tests
 * - Tests developmental stages and effects
 * - Tests synaptogenesis, pruning, and maturation
 * - Tests development integration with other systems
 *
 * ### Checkpoint System Tests
 * - Tests save/load functionality
 * - Tests checkpoint persistence
 * - Tests recovery mechanisms
 *
 * ### Sleep/Rest Cycle Tests
 * - Tests sleep stage management
 * - Tests memory consolidation during sleep
 * - Tests rest cycle integration
 *
 * ### Replay System Tests
 * - Tests episodic memory replay
 * - Tests replay schedule and frequency
 * - Tests replay effectiveness
 *
 * ### Performance Benchmarks
 * - Tests system performance under load
 * - Tests memory usage patterns
 * - Tests computational efficiency
 *
 * ## Test Execution
 *
 * The test suite can be run using:
 *
 * ### Unit Tests
 * ```bash
 *   test_config          # Configuration system tests
 *   test_config_enhanced # Enhanced configuration tests
 *   test_brain           # Brain integration tests
 *   test_clock           # Clock tests
 * ```
 *
 * ### Comprehensive Integration Tests
 * ```bash
 *   nlm_test_runner     # Complete integration test suite
 * ```
 *
 * ### Performance Benchmarks
 * ```bash
 *   nlm_benchmark       # Performance benchmarks
 * ```
 *
 * ## Test Results
 *
 * Each test produces detailed results including:
 * - Test name and system
 * - Pass/fail status
 * - Execution time
 * - Error details (if failed)
 * - Performance metrics (for benchmarks)
 *
 * The comprehensive test runner generates a complete report of all test results.
 *
 * ## Test Coverage
 *
 * The test suite provides comprehensive coverage of:
 *
 * ### Configuration System
 * - ✓ JSON/YAML format support
 * - ✓ Configuration validation
 * - ✓ Parameter inheritance
 * - ✓ Environment variable expansion
 * - ✓ Schema validation
 * - ✓ Configuration merging
 *
 * ### Memory Systems
 * - ✓ Working memory integration
 * - ✓ Episodic memory (replay and consolidation)
 * - ✓ Semantic memory (knowledge storage and retrieval)
 * - ✓ Procedural memory (skill learning)
 * - ✓ Associative memory (pattern associations)
 *
 * ### Neuromodulation
 * - ✓ All neuromodulators integrated
 * - ✓ ACh, NE, 5-HT systems (Phase 2)
 * - ✓ Neuromodulation effects on plasticity
 * - ✓ Integration with prediction and learning
 *
 * ### Prediction System
 * - ✓ Prediction accuracy
 * - ✓ Confidence tracking
 * - ✓ Prediction error computation
 * - ✓ State prediction and update
 *
 * ### Cognition Systems
 * - ✓ Neural planner for action evaluation
 * - ✓ Concept formation for pattern discovery
 * - ✓ Attentional selection for focus
 *
 * ### Development System
 * - ✓ Developmental stages
 * - ✓ Synaptogenesis and maturation
 * - ✓ Pruning and refinement
 * - ✓ Integration with plasticity
 *
 * ### Checkpoint and Recovery
 * - ✓ Save/load functionality
 * - ✓ State persistence
 * - ✓ Recovery mechanisms
 *
 * ### Sleep and Memory Consolidation
 * - ✓ Sleep/rest cycle management
 * - ✓ Memory consolidation during sleep
 * - ✓ Replay mechanisms
 *
 * ### Performance and Scalability
 * - ✓ Benchmarks for all major systems
 * - ✓ Memory usage profiling
 * - ✓ Computational efficiency tests
 *
 * ## Integration with Existing Tests
 *
 * The comprehensive test suite builds upon and extends existing tests:
 *
 * ### Existing Tests
 * - `test_config.cpp`: Enhanced configuration tests
 * - `test_config_enhanced.cpp`: Additional configuration tests
 * - `test_brain.cpp`: Brain integration tests
 * - `test_clock.cpp`: Clock tests
 * - `test_main.cpp`: Main system tests
 *
 * ### Integration Improvements
 * - Enhanced test coverage for new features
 * - Integration tests for all system interactions
 * - Performance benchmarks for validation
 * - Regression tests for core functionality
 *
 * ## Building and Running Tests
 *
 * To build and run the comprehensive test suite:
 *
 * ### Build
 * ```bash
 *   mkdir build
 *   cd build
 *   cmake .. -DCMAKE_BUILD_TYPE=Release
 *   make -j4
 * ```
 *
 * ### Run Tests
 * ```bash
 *   # Run specific unit tests
 *   ./test_config
 *   ./test_brain
 *   ./nlm_test_runner
 *
 *   # Run all tests with CTest
 *   ctest --output-on-failure
 * ```
 *
 * ### Performance Tests
 * ```bash
 *   # Run performance benchmarks
 *   ./nlm_benchmark
 * ```
 *
 * ## Test Quality Assurance
 *
 * The test suite ensures:
 *
 * ### Functional Correctness
 * - All systems integrate properly
 * - No component fails to initialize
 * - All core functionality works as expected
 *
 * ### Performance Requirements
 * - System meets performance benchmarks
 * - Memory usage is reasonable
 * - Computational efficiency is acceptable
 *
 * ### Regression Prevention
 * - Existing functionality is preserved
 * - No new bugs introduced
 * - System stability is maintained
 *
 * ### Integration Validation
 * - All systems work together
 * - Cross-system interactions are correct
 * - End-to-end workflows function properly
 *
 * ## Conclusion
 *
 * The comprehensive test suite provides:
 *
 * 1. **Complete coverage** of all Phase 6 integration improvements
 * 2. **Systematic validation** of all brain components
 * 3. **Performance benchmarking** for optimization
 * 4. **Regression testing** for stability
 * 5. **Integration testing** for system interactions
 *
 * The test suite ensures that the NLM Phase 6 integration is robust, reliable, and ready for production use.
 */

/**
 * NLM Phase 6 Comprehensive Test Suite
 *
 * This test suite validates all integration improvements made in Phase 6.
 * It ensures that the NLM system is fully functional and ready for use.
 *
 * The suite can be run in multiple modes:
 * - Unit tests for individual components
 * - Integration tests for system interactions
 * - Performance benchmarks for optimization
 * - Comprehensive end-to-end tests
 *
 * All tests pass when the NLM Phase 6 integration is complete and correct.
 */

#include "tests/test_runner.cpp"

/**
 * Entry point for the comprehensive test suite.
 *
 * @return 0 if all tests pass, 1 if any test fails
 */
int main(int argc, char* argv[]) {
    return nlm::main(argc, argv);
}
