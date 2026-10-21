// Phase6IntegratedExperimentIntegration.hpp - Integration Test Framework Header
// Refactored integration test framework for Phase 6
// Provides comprehensive test coordination and component management

#pragma once

#include "experiments/Phase6IntegratedExperiment.hpp"
#include <memory>

namespace nlm {

// ========== INTEGRATION TEST COMPONENTS ==========

// Forward declarations for test components
class IntegrationTestRunner;
class BrainIntegrationTests;
class MemoryIntegrationTests;
class NeuromodulationIntegrationTests;
class CheckpointIntegrationTests;
class ReplayIntegrationTests;

// Main integration test framework
class IntegrationTestRunner {
public:
    IntegrationTestRunner();
    ~IntegrationTestRunner();
    
    // Run all integration tests in sequence
    bool runAllTests();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// ========== BRAIN INTEGRATION TESTS ==========

class BrainIntegrationTests {
public:
    static bool testBasicBrainFunctionality();
    static bool testBrainMemorySystems();
    static bool testBrainNeuromodulation();
    static bool testBrainPlasticity();
    static bool testBrainDevelopment();
};

// ========== MEMORY INTEGRATION TESTS ==========

class MemoryIntegrationTests {
public:
    static bool testWorkingMemoryIntegration();
    static bool testEpisodicMemoryIntegration();
    static bool testAssociativeMemoryIntegration();
};

// ========== NEUROMODULATION INTEGRATION TESTS ==========

class NeuromodulationIntegrationTests {
public:
    static bool testDopamineSystem();
    static bool testCuriositySystem();
    static bool testPredictionErrorSystem();
    static bool testNoveltyDetection();
};

// ========== CHECKPOINT INTEGRATION TESTS ==========

class CheckpointIntegrationTests {
public:
    static bool testCheckpointSaveLoad();
    static bool testCheckpointValidation();
};

// ========== REPLAY INTEGRATION TESTS ==========

class ReplayIntegrationTests {
public:
    static bool testMemoryReplay();
    static bool testReplayInterval();
};

} // namespace nlm