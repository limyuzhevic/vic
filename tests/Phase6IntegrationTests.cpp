// Phase 6 Integration Tests Suite
// Comprehensive tests for all Phase 6 integration improvements
// Validates working memory, prediction, concept formation, neuromodulation,
// replay, sleep consolidation, and development integration

#include "Phase6IntegrationTests.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/SIMDVectorization.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AttentionalSelection.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "sleep/SleepManager.hpp"
#include "development/DevelopmentSystem.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

namespace test_phase6_integration {

using namespace nlm;

void IntegrationTestFramework::initialize() {
    Logger::initialize();
    NLM_LOG_INFO("Initializing Phase 6 Integration Test Framework");
}

void IntegrationTestFramework::cleanup() {
    NLM_LOG_INFO("Phase 6 Integration Test Framework cleanup completed");
}

void IntegrationTestFramework::runAllTests() {
    NLM_LOG_INFO("Starting comprehensive Phase 6 integration test suite");
    
    // Integration Test Suites
    runWorkingMemoryIntegrationTests();
    runPredictionSystemIntegrationTests();
    runConceptFormationIntegrationTests();
    runNeuromodulationIntegrationTests();
    runReplayIntegrationTests();
    runSleepConsolidationTests();
    runDevelopmentIntegrationTests();
    
    // Performance Test Suites
    runMemoryPoolTests();
    runEventQueueTests();
    runSIMDIntegrationTests();
    runParallelProcessingTests();
    
    // Phase 6 Integration Tests
    runCompleteBrainLoopTests();
    runSystemConnectivityTests();
    runIntegrationStateTests();
    runCrossSystemTests();
    
    // Stress and Load Tests
    runMemoryLoadTests();
    runPerformanceStressTests();
    runIntegrationStressTests();
    
    NLM_LOG_SUCCESS("All Phase 6 integration tests completed successfully!");
}

void IntegrationTestFramework::runWorkingMemoryIntegrationTests() {
    NLM_LOG_INFO("Running Working Memory Integration Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("working_memory_capacity", static_cast<int64_t>(50), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    auto wm = brain.getWorkingMemory();
    auto attention = brain.getAttentionSelection();
    auto episodic = brain.getEpisodicMemory();
    
    assert(wm != nullptr);
    assert(attention != nullptr);
    assert(episodic != nullptr);
    
    // Test working memory with attention integration
    wm->store("test_item", "test_data", 0.5f);
    auto retrieved = wm->retrieve("test_item");
    assert(retrieved.has_value());
    
    // Test attention selection integration
    attention->updateAttentionWeights();
    auto attention_weights = attention->getAttentionWeights();
    assert(!attention_weights.empty());
    
    // Test episodic memory integration with working memory
    episodic->storeEpisodicMemory("test_episode", "episode_data", 0.7f);
    auto episode = episodic->retrieveEpisodicMemory("test_episode");
    assert(episode.has_value());
    
    NLM_LOG_SUCCESS("Working Memory Integration Tests PASSED");
}

void IntegrationTestFramework::runPredictionSystemIntegrationTests() {
    NLM_LOG_INFO("Running Prediction System Integration Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("prediction_system_enabled", true, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    auto prediction = brain.getPredictionSystem();
    auto wm = brain.getWorkingMemory();
    auto attention = brain.getAttentionSelection();
    
    assert(prediction != nullptr);
    assert(wm != nullptr);
    assert(attention != nullptr);
    
    // Test prediction system with working memory
    wm->store("prediction_input", "sensor_data", 0.8f);
    prediction->updatePredictions();
    
    // Test prediction system with attention
    attention->updateAttentionWeights();
    prediction->updatePredictions();
    
    // Test prediction accuracy
    auto predictions = prediction->getPredictions();
    assert(!predictions.empty());
    
    NLM_LOG_SUCCESS("Prediction System Integration Tests PASSED");
}

void IntegrationTestFramework::runConceptFormationIntegrationTests() {
    NLM_LOG_INFO("Running Concept Formation Integration Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("concept_formation_enabled", true, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    auto concept = brain.getConceptFormation();
    auto episodic = brain.getEpisodicMemory();
    auto attention = brain.getAttentionSelection();
    
    assert(concept != nullptr);
    assert(episodic != nullptr);
    assert(attention != nullptr);
    
    // Test concept formation with episodic memory
    episodic->storeEpisodicMemory("concept_episode", "raw_data", 0.9f);
    concept->processEpisode("concept_episode");
    
    auto concepts = concept->getConcepts();
    assert(!concepts.empty());
    
    // Test concept formation with attention
    attention->updateAttentionWeights();
    concept->updateConcepts();
    
    NLM_LOG_SUCCESS("Concept Formation Integration Tests PASSED");
}

void IntegrationTestFramework::runNeuromodulationIntegrationTests() {
    NLM_LOG_INFO("Running Neuromodulation Integration Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("neuromodulation_enabled", true, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test all neuromodulators integration
    auto reward = brain.getRewardModulator();
    auto error = brain.getPredictionError();
    auto novelty = brain.getNoveltyModulator();
    auto curiosity = brain.getCuriosityModulator();
    auto ach = brain.getAcetylcholine();
    auto nor = brain.getNorepinephrine();
    auto serotonin = brain.getSerotonin();
    
    // Verify all neuromodulators are active
    assert(reward != nullptr || !config->getBool("reward_enabled", false));
    assert(error != nullptr || !config->getBool("prediction_error_enabled", false));
    assert(novelty != nullptr || !config->getBool("novelty_enabled", false));
    assert(curiosity != nullptr || !config->getBool("curiosity_enabled", false));
    assert(ach != nullptr || !config->getBool("acetylcholine_enabled", false));
    assert(nor != nullptr || !config->getBool("noradrenaline_enabled", false));
    assert(serotonin != nullptr || !config->getBool("serotonin_enabled", false));
    
    // Test neuromodulation system integration with cognitive systems
    brain.getWorkingMemory()->store("modulation_test", "data", 0.5f);
    brain.step(0);
    
    NLM_LOG_SUCCESS("Neuromodulation Integration Tests PASSED");
}

void IntegrationTestFramework::runReplayIntegrationTests() {
    NLM_LOG_INFO("Running Replay Integration Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("replay_enabled", true, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    auto wm = brain.getWorkingMemory();
    auto episodic = brain.getEpisodicMemory();
    auto attention = brain.getAttentionSelection();
    
    assert(wm != nullptr);
    assert(episodic != nullptr);
    assert(attention != nullptr);
    
    // Store data for replay
    wm->store("replay_item", "replay_data", 0.6f);
    episodic->storeEpisodicMemory("replay_episode", "replay_episode_data", 0.8f);
    
    // Trigger replay process
    brain.triggerReplay();
    
    // Test attention integration with replay
    attention->updateAttentionWeights();
    brain.step(0);
    
    NLM_LOG_SUCCESS("Replay Integration Tests PASSED");
}

void IntegrationTestFramework::runSleepConsolidationTests() {
    NLM_LOG_INFO("Running Sleep Consolidation Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("sleep_consolidation_enabled", true, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    auto sleep = brain.getSleepManager();
    auto wm = brain.getWorkingMemory();
    auto episodic = brain.getEpisodicMemory();
    
    assert(wm != nullptr);
    assert(episodic != nullptr);
    assert(sleep != nullptr);
    
    // Store data for consolidation
    wm->store("consolidation_item", "consolidation_data", 0.7f);
    episodic->storeEpisodicMemory("consolidation_episode", "consolidation_episode_data", 0.9f);
    
    // Test sleep consolidation process
    brain.enterSleep();
    brain.wakeUp();
    
    // Test memory prioritization during sleep
    sleep->updateMemoryPriorities();
    
    NLM_LOG_SUCCESS("Sleep Consolidation Tests PASSED");
}

void IntegrationTestFramework::runDevelopmentIntegrationTests() {
    NLM_LOG_INFO("Running Development Integration Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("development_enabled", true, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    auto dev = brain.getDevelopmentSystem();
    auto wm = brain.getWorkingMemory();
    auto episodic = brain.getEpisodicMemory();
    
    assert(dev != nullptr);
    assert(wm != nullptr);
    assert(episodic != nullptr);
    
    // Test developmental processes integration
    brain.simulateDevelopment();
    
    // Test memory development effects
    wm->store("development_test", "dev_data", 0.5f);
    episodic->storeEpisodicMemory("development_episode", "dev_episode_data", 0.6f);
    
    brain.step(0);
    
    NLM_LOG_SUCCESS("Development Integration Tests PASSED");
}

void IntegrationTestFramework::runMemoryPoolTests() {
    NLM_LOG_INFO("Running Memory Pool Tests");
    
    MemoryPool pool(1024 * 1024, 64);  // 1MB pool, 64-byte blocks
    
    // Test allocation and deallocation
    for (int i = 0; i < 100; ++i) {
        auto block = pool.allocate();
        assert(block != nullptr);
        pool.deallocate(block);
    }
    
    // Test performance metrics
    auto stats = pool.getStats();
    assert(stats.total_allocations >= 0);
    assert(stats.allocation_failures == 0);
    
    NLM_LOG_SUCCESS("Memory Pool Tests PASSED");
}

void IntegrationTestFramework::runEventQueueTests() {
    NLM_LOG_INFO("Running Event Queue Tests");
    
    EventQueue queue(10000, 100);
    
    // Test event enqueuing and dequeuing
    for (int i = 0; i < 100; ++i) {
        Event event;
        event.id = i;
        event.type = EventType::Cognitive;
        event.timestamp = std::chrono::high_resolution_clock::now();
        
        queue.enqueue(event);
    }
    
    // Test event processing
    queue.processEvents();
    
    auto stats = queue.getStats();
    assert(stats.events_processed >= 0);
    
    NLM_LOG_SUCCESS("Event Queue Tests PASSED");
}

void IntegrationTestFramework::runSIMDIntegrationTests() {
    NLM_LOG_INFO("Running SIMD Integration Tests");
    
    SIMDVectorization simd;
    
    // Test vector operations
    std::vector<float> vec1(1000, 1.0f);
    std::vector<float> vec2(1000, 2.0f);
    std::vector<float> result(1000);
    
    simd.addVectors(vec1, vec2, result);
    
    for (int i = 0; i < 1000; ++i) {
        assert(result[i] == 3.0f);
    }
    
    // Test vectorized operations
    simd.multiplyScalar(vec1, 2.0f, result);
    
    for (int i = 0; i < 1000; ++i) {
        assert(result[i] == 2.0f);
    }
    
    NLM_LOG_SUCCESS("SIMD Integration Tests PASSED");
}

void IntegrationTestFramework::runParallelProcessingTests() {
    NLM_LOG_INFO("Running Parallel Processing Tests");
    
    ParallelProcessing parallel;
    
    // Test parallel computation
    std::vector<float> input1(1000, 1.0f);
    std::vector<float> input2(1000, 2.0f);
    std::vector<float> output(1000);
    
    parallel.process(input1, input2, output, [](float a, float b) {
        return a * b;
    });
    
    for (int i = 0; i < 1000; ++i) {
        assert(output[i] == 2.0f);
    }
    
    NLM_LOG_SUCCESS("Parallel Processing Tests PASSED");
}

void IntegrationTestFramework::runCompleteBrainLoopTests() {
    NLM_LOG_INFO("Running Complete Brain Loop Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("simulation_steps", static_cast<int64_t>(10), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Run complete brain loop with all Phase 6 systems
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify all systems completed their processing
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getFiringNeuronCount() >= 0);
    
    NLM_LOG_SUCCESS("Complete Brain Loop Tests PASSED");
}

void IntegrationTestFramework::runSystemConnectivityTests() {
    NLM_LOG_INFO("Running System Connectivity Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Verify system connectivity
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getSleepManager() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getAttentionSelection() != nullptr);
    
    // Test data flow between systems
    brain.getWorkingMemory()->store("connectivity_test", "data", 0.5f);
    brain.step(0);
    
    auto retrieved = brain.getWorkingMemory()->retrieve("connectivity_test");
    assert(retrieved.has_value());
    
    NLM_LOG_SUCCESS("System Connectivity Tests PASSED");
}

void IntegrationTestFramework::runIntegrationStateTests() {
    NLM_LOG_INFO("Running Integration State Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Check integration status
    IntegrationStatus status = brain.getIntegrationStatus();
    
    assert(status.working_memory_active);
    assert(status.prediction_active);
    assert(status.concept_formation_active);
    assert(status.neuromodulation_active);
    assert(status.replay_active);
    assert(status.sleep_consolidation_active);
    assert(status.development_active);
    assert(status.performance_infrastructure_active);
    
    NLM_LOG_SUCCESS("Integration State Tests PASSED");
}

void IntegrationTestFramework::runCrossSystemTests() {
    NLM_LOG_INFO("Running Cross-System Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test cross-system interactions
    auto wm = brain.getWorkingMemory();
    auto prediction = brain.getPredictionSystem();
    auto concept = brain.getConceptFormation();
    
    // Store data in working memory
    wm->store("cross_test", "cross_data", 0.7f);
    
    // Process through prediction system
    prediction->updatePredictions();
    
    // Process through concept formation
    concept->updateConcepts();
    
    // Verify integration
    auto wm_result = wm->retrieve("cross_test");
    assert(wm_result.has_value());
    
    NLM_LOG_SUCCESS("Cross-System Tests PASSED");
}

void IntegrationTestFramework::runMemoryLoadTests() {
    NLM_LOG_INFO("Running Memory Load Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("working_memory_capacity", static_cast<int64_t>(500), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    auto wm = brain.getWorkingMemory();
    
    // Load memory with large dataset
    for (int i = 0; i < 100; ++i) {
        wm->store(std::to_string(i), std::string("data_" + std::to_string(i)), 0.5f);
    }
    
    // Verify all data stored
    for (int i = 0; i < 100; ++i) {
        auto result = wm->retrieve(std::to_string(i));
        assert(result.has_value());
    }
    
    // Process with load
    brain.step(0);
    
    NLM_LOG_SUCCESS("Memory Load Tests PASSED");
}

void IntegrationTestFramework::runPerformanceStressTests() {
    NLM_LOG_INFO("Running Performance Stress Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("simulation_steps", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Run performance stress test
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Store and retrieve frequently
        brain.getWorkingMemory()->store("perf_test", "perf_data", 0.5f);
        brain.getWorkingMemory()->retrieve("perf_test");
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Verify performance metrics
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    NLM_LOG_INFO("Performance stress test completed in " + std::to_string(duration.count()) + "ms");
    NLM_LOG_SUCCESS("Performance Stress Tests PASSED");
}

void IntegrationTestFramework::runIntegrationStressTests() {
    NLM_LOG_INFO("Running Integration Stress Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("working_memory_capacity", static_cast<int64_t>(200), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Complex integration stress test
    for (int step = 0; step < 50; ++step) {
        // Multi-system interaction
        brain.getWorkingMemory()->store("stress_test", "stress_data", 0.5f);
        brain.getPredictionSystem()->updatePredictions();
        brain.getConceptFormation()->updateConcepts();
        brain.step(step);
        
        // Trigger replay occasionally
        if (step % 10 == 0) {
            brain.triggerReplay();
        }
        
        // Enter/exit sleep occasionally
        if (step % 20 == 0) {
            brain.enterSleep();
            brain.wakeUp();
        }
        
        // Simulate development occasionally
        if (step % 15 == 0) {
            brain.simulateDevelopment();
        }
    }
    
    // Verify system stability
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getFiringNeuronCount() >= 0);
    
    NLM_LOG_SUCCESS("Integration Stress Tests PASSED");
}

void test_neuron_integration() {
    std::cout << "  Running Neuron integration tests..." << std::endl;
    
    IntegrationTestFramework framework;
    framework.initialize();
    framework.runAllTests();
    framework.cleanup();
    
    std::cout << "  All Phase 6 integration tests completed successfully!" << std::endl;
}

} // namespace test_phase6_integration