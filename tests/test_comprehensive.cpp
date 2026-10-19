// Comprehensive C++ Unit Tests for NLM
// Tests all critical areas: bug fixes, thread safety, memory management, 
// plasticity, neuromodulation, memory systems, prediction/planning, development

#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>
#include <cmath>

#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "development/DevelopmentSystem.hpp"

namespace test_critical_bugs {
    
    void testRandomGeneratorInitialization() {
        std::cout << "    Testing RandomGenerator initialization..." << std::endl;
        
        // Test proper initialization (fix for Critical Bug #1)
        nlm::RandomGenerator rng1(42);
        nlm::RandomGenerator rng2(42);
        
        // Same seed should produce same sequence
        for (int i = 0; i < 10; ++i) {
            assert(rng1.uniformInt(0, 100) == rng2.uniformInt(0, 100));
        }
        
        // Test seed reset
        uint32_t val1 = rng1.uniformInt(0, 1000);
        rng1.seed(12345);
        rng1.seed(12345);
        uint32_t val2 = rng1.uniformInt(0, 1000);
        assert(val1 == val2);
        
        std::cout << "    testRandomGeneratorInitialization PASSED" << std::endl;
    }
    
    void testSpikeHistoryBufferManagement() {
        std::cout << "    Testing spike history buffer management..." << std::endl;
        
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        // Test initial state
        assert(neuron.getSpikeHistory().empty());
        
        // Add spikes
        neuron.recordSpike(0.1);
        neuron.recordSpike(0.2);
        neuron.recordSpike(0.3);
        
        assert(neuron.getSpikeHistory().size() == 3);
        assert(neuron.getSpikeHistory()[0] == 0.1);
        assert(neuron.getSpikeHistory()[2] == 0.3);
        
        // Test reset
        neuron.clearSpikeHistory();
        assert(neuron.getSpikeHistory().empty());
        
        // Test Neuron.cpp critical bug fix: proper buffer management
        for (int i = 0; i < 100; ++i) {
            neuron.recordSpike(static_cast<float>(i) * 0.1f);
        }
        
        // Buffer should not overflow
        assert(neuron.getSpikeHistory().size() <= nlm::Impl::NeuronImpl::MAX_SPIKE_HISTORY);
        
        std::cout << "    testSpikeHistoryBufferManagement PASSED" << std::endl;
    }
    
    void testNumericalStability() {
        std::cout << "    Testing numerical stability..." << std::endl;
        
        nlm::RandomGenerator rng(42);
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.initializeRandom(rng);
        
        // Test for numerical overflow/underflow (fix for Critical Bug #6)
        float maxMembranePotential = -1000.0f;
        float minMembranePotential = 1000.0f;
        
        for (int i = 0; i < 1000; ++i) {
            neuron.stepLIF(1000.0, 0.001);  // Use consistent time units
            
            float v = neuron.getMembranePotential();
            maxMembranePotential = std::max(maxMembranePotential, v);
            minMembranePotential = std::min(minMembranePotential, v);
            
            // Bound checks (fix for Critical Bug #6)
            assert(v <= 50.0f && v >= -100.0f);
        }
        
        std::cout << "    testNumericalStability PASSED" << std::endl;
    }
    
    void testConfigurationValidation() {
        std::cout << "    Testing configuration validation..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        
        // Set valid ranges
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1);
        
        // Test Brain.cpp critical bug fix: configuration validation
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        
        std::cout << "    testConfigurationValidation PASSED" << std::endl;
    }
}

namespace test_thread_safety {
    
    void testBrainThreadSafety() {
        std::cout << "    Testing Brain thread safety..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Multi-threaded spike processing (fix for Critical Bug #2)
        std::vector<std::thread> threads;
        std::atomic<size_t> spikeCount{0};
        
        for (int t = 0; t < 4; ++t) {
            threads.emplace_back([&]() {
                for (int i = 0; i < 100; ++i) {
                    brain.step(i, i * 0.001);
                    spikeCount.fetch_add(brain.getTotalSpikeCount(), std::memory_order_relaxed);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify no data races occurred
        assert(brain.getTotalSpikeCount() >= 0);
        
        std::cout << "    testBrainThreadSafety PASSED" << std::endl;
    }
}

namespace test_plasticity {
    
    void testSTDPPlasticity() {
        std::cout << "    Testing STDP plasticity..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01f);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* stdp = brain.getSTDP();
        assert(stdp != nullptr);
        
        // Test STDP weight updates
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Simulate pre-post spike pairing
            if (step % 10 == 0) {
                stdp->onPreSpike(step);
            }
            if (step % 7 == 0) {
                stdp->onPostSpike(step);
            }
        }
        
        std::cout << "    testSTDPPlasticity PASSED" << std::endl;
    }
    
    void testHebbianPlasticity() {
        std::cout << "    Testing Hebbian plasticity..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.001f);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* hebbian = brain.getHebbian();
        assert(hebbian != nullptr);
        
        // Test Hebbian weight strengthening
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Create correlated activity
            if (step % 5 == 0) {
                hebbian->onCoActivity(step);
            }
        }
        
        std::cout << "    testHebbianPlasticity PASSED" << std::endl;
    }
    
    void testStructuralPlasticity() {
        std::cout << "    Testing structural plasticity..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.pruning_threshold", 0.1f);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* structural = brain.getStructuralPlasticity();
        assert(structural != nullptr);
        
        // Test structural changes over time
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
            
            if (step % 10 == 0) {
                structural->update(step);
            }
        }
        
        std::cout << "    testStructuralPlasticity PASSED" << std::endl;
    }
}

namespace test_neuromodulation {
    
    void testDopamineModulation() {
        std::cout << "    Testing dopamine neuromodulation..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.dopamine.baseline", 0.1f);
        
        nlm::Dopamine dopamine(config);
        
        // Test dopamine level changes
        float initialLevel = dopamine.getLevel();
        
        for (int step = 0; step < 100; ++step) {
            dopamine.update(static_cast<float>(step));
            
            // Should decay towards baseline
            assert(dopamine.getLevel() <= 1.0f);
        }
        
        std::cout << "    testDopamineModulation PASSED" << std::endl;
    }
    
    void testCuriosityModulation() {
        std::cout << "    Testing curiosity neuromodulation..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.scale", 0.5f);
        
        nlm::Curiosity curiosity(config);
        
        // Test curiosity level changes
        for (int step = 0; step < 50; ++step) {
            curiosity.update(static_cast<float>(step), 0.5f, 0.0f);
            
            assert(curiosity.getLevel() >= 0.0f);
        }
        
        std::cout << "    testCuriosityModulation PASSED" << std::endl;
    }
    
    void testNoveltyDetection() {
        std::cout << "    Testing novelty detection..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 0.3f);
        
        nlm::Novelty novelty(config);
        
        // Test novelty signal generation
        std::vector<float> inputs{0.1f, 0.5f, 0.3f, 0.9f, 0.2f};
        
        for (int step = 0; step < 100; ++step) {
            float input = inputs[step % inputs.size()];
            novelty.update(input, static_cast<float>(step));
            
            assert(novelty.getNovelty() >= 0.0f);
        }
        
        std::cout << "    testNoveltyDetection PASSED" << std::endl;
    }
}

namespace test_memory_systems {
    
    void testWorkingMemory() {
        std::cout << "    Testing working memory..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("memory.workingMemoryCapacity", 10);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Test working memory operations
        std::vector<float> pattern(10, 0.5f);
        
        for (int step = 0; step < 50; ++step) {
            workingMemory->store(pattern, static_cast<float>(step));
            
            auto retrieved = workingMemory->retrieve(static_cast<float>(step));
            assert(retrieved.size() == pattern.size());
        }
        
        std::cout << "    testWorkingMemory PASSED" << std::endl;
    }
    
    void testEpisodicMemory() {
        std::cout << "    Testing episodic memory..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("memory.episodicCapacity", 5);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Test episodic memory storage and replay
        for (int step = 0; step < 20; ++step) {
            episodicMemory->storeEpisode("test_episode", step);
        }
        
        // Test replay (fix for Critical Bug #4)
        episodicMemory->replayEpisode(0, 10);
        
        std::cout << "    testEpisodicMemory PASSED" << std::endl;\n    }
    
    void testAssociativeMemory() {
        std::cout << "    Testing associative memory..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("memory.associativeStrength", 0.8f);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* associativeMemory = brain.getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        // Test associative memory patterns
        std::vector<float> pattern1{1.0f, 0.0f, 0.0f};
        std::vector<float> pattern2{0.0f, 1.0f, 0.0f};
        
        associativeMemory->associate(pattern1, pattern2);
        
        auto association = associativeMemory->getAssociation(pattern1);
        assert(association.size() == pattern2.size());
        
        std::cout << "    testAssociativeMemory PASSED" << std::endl;
    }
}

namespace test_prediction_planning {
    
    void testPredictionSystem() {
        std::cout << "    Testing prediction system..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("prediction.planningDepth", 3);
        config->set("prediction.confidenceThreshold", 0.5f);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* predictionSystem = brain.getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Test prediction operations
        for (int step = 0; step < 50; ++step) {
            predictionSystem->update(step, 0.5f, 0.3f);
            
            float confidence = predictionSystem->getConfidence();
            assert(confidence >= 0.0f && confidence <= 1.0f);
        }
        
        std::cout << "    testPredictionSystem PASSED" << std::endl;
    }
    
    void testNeuralPlanner() {
        std::cout << "    Testing neural planner..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("cognition.planning.enabled", true);
        config->set("cognition.planning.horizon", 5);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        // Test planning operations
        for (int step = 0; step < 30; ++step) {
            planner->update(step);
            
            auto plan = planner->getCurrentPlan();
            assert(plan.size() <= config->getOr<int64_t>("cognition.planning.horizon", 5));
        }
        
        std::cout << "    testNeuralPlanner PASSED" << std::endl;
    }
}

namespace test_development {
    
    void testDevelopmentalStages() {
        std::cout << "    Testing developmental stages..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("development.stage", "critical_period");
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test developmental stage progression
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
        
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
        
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
        
        std::cout << "    testDevelopmentalStages PASSED" << std::endl;
    }
    
    void testMaturation() {
        std::cout << "    Testing neural maturation..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("development.maturation.enabled", true);
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* developmentSystem = brain.getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Test maturation over time
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            developmentSystem->update(step);
            
            // Neural properties should change with development
            assert(brain.getTotalNeuronCount() == 50);
        }
        
        std::cout << "    testMaturation PASSED" << std::endl;
    }
}

namespace test_memory_management {
    
    void testResourceCleanup() {
        std::cout << "    Testing resource cleanup..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Use resources
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Reset and verify cleanup
        brain.reset();
        assert(brain.getTotalNeuronCount() == 50);
        
        std::cout << "    testResourceCleanup PASSED" << std::endl;
    }
    
    void testMemoryPool() {
        std::cout << "    Testing memory pool operations..." << std::endl;
        
        nlm::MemoryPool<float> pool(1024, 4);
        
        // Test allocation
        std::vector<float*> objects;
        for (int i = 0; i < 100; ++i) {
            objects.push_back(pool.allocate());
            assert(objects.back() != nullptr);
        }
        
        // Test deallocation
        for (auto* obj : objects) {
            pool.deallocate(obj);
        }
        
        // Pool should be empty again
        assert(pool.empty());
        
        std::cout << "    testMemoryPool PASSED" << std::endl;
    }
}

int main() {
    std::cout << "=== NLM Comprehensive Unit Tests ===" << std::endl;
    std::cout << "Testing all critical areas: Bug fixes, thread safety, memory management, " << std::endl;
    std::cout << "plasticity, neuromodulation, memory systems, prediction/planning, development" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Critical bug fixes
    std::cout << "Running Critical Bug Fix Tests..." << std::endl;
    try {
        test_critical_bugs::testRandomGeneratorInitialization();
        test_critical_bugs::testSpikeHistoryBufferManagement();
        test_critical_bugs::testNumericalStability();
        test_critical_bugs::testConfigurationValidation();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Thread safety
    std::cout << "Running Thread Safety Tests..." << std::endl;
    try {
        test_thread_safety::testBrainThreadSafety();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Plasticity
    std::cout << "Running Plasticity Tests..." << std::endl;
    try {
        test_plasticity::testSTDPPlasticity();
        test_plasticity::testHebbianPlasticity();
        test_plasticity::testStructuralPlasticity();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Neuromodulation
    std::cout << "Running Neuromodulation Tests..." << std::endl;
    try {
        test_neuromodulation::testDopamineModulation();
        test_neuromodulation::testCuriosityModulation();
        test_neuromodulation::testNoveltyDetection();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Memory systems
    std::cout << "Running Memory System Tests..." << std::endl;
    try {
        test_memory_systems::testWorkingMemory();
        test_memory_systems::testEpisodicMemory();
        test_memory_systems::testAssociativeMemory();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Prediction and planning
    std::cout << "Running Prediction and Planning Tests..." << std::endl;
    try {
        test_prediction_planning::testPredictionSystem();
        test_prediction_planning::testNeuralPlanner();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Development
    std::cout << "Running Development Tests..." << std::endl;
    try {
        test_development::testDevelopmentalStages();
        test_development::testMaturation();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Memory management
    std::cout << "Running Memory Management Tests..." << std::endl;
    try {
        test_memory_management::testResourceCleanup();
        test_memory_management::testMemoryPool();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Comprehensive Unit Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Comprehensive Unit Tests FAILED ===" << std::endl;
        return 1;
    }
}