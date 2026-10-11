// Phase 6 Integration Tests Header
// Comprehensive test suite for Phase 6 integration improvements

#ifndef PHASE6_INTEGRATION_TESTS_HPP
#define PHASE6_INTEGRATION_TESTS_HPP

#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/SIMDVectorization.hpp"

namespace nlm {
    class WorkingMemory;
    class EpisodicMemory;
    class AttentionalSelection;
    class PredictionSystem;
    class ConceptFormation;
    class Neuromodulator;
    class SleepManager;
    class DevelopmentSystem;
}

class IntegrationTestFramework {
public:
    struct IntegrationStatus {
        bool working_memory_active = false;
        bool prediction_active = false;
        bool concept_formation_active = false;
        bool neuromodulation_active = false;
        bool replay_active = false;
        bool sleep_consolidation_active = false;
        bool development_active = false;
        bool performance_infrastructure_active = false;
    };

    enum class EventType { Cognitive, Memory, Prediction, Concept, Neuromodulation, Replay, Sleep, Development };

    struct Event {
        int id;
        EventType type;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string data;
    };

    IntegrationTestFramework();
    ~IntegrationTestFramework();

    void initialize();
    void cleanup();
    void runAllTests();

    // Integration Test Suites
    void runWorkingMemoryIntegrationTests();
    void runPredictionSystemIntegrationTests();
    void runConceptFormationIntegrationTests();
    void runNeuromodulationIntegrationTests();
    void runReplayIntegrationTests();
    void runSleepConsolidationTests();
    void runDevelopmentIntegrationTests();

    // Performance Test Suites
    void runMemoryPoolTests();
    void runEventQueueTests();
    void runSIMDIntegrationTests();
    void runParallelProcessingTests();

    // Phase 6 Integration Tests
    void runCompleteBrainLoopTests();
    void runSystemConnectivityTests();
    void runIntegrationStateTests();
    void runCrossSystemTests();

    // Stress and Load Tests
    void runMemoryLoadTests();
    void runPerformanceStressTests();
    void runIntegrationStressTests();

    IntegrationStatus getIntegrationStatus() const { return current_status; }
    void setIntegrationStatus(const IntegrationStatus& status) { current_status = status; }

private:
    IntegrationStatus current_status;
};

// Test suite runner function
void test_neuron_integration();

#endif // PHASE6_INTEGRATION_TESTS_HPP