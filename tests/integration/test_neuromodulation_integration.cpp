// Neuromodulation System Integration Tests
// Tests all neuromodulation systems (dopamine + curiosity + novelty) integration

#include "brain/neuromodulation/BrainNeuromodulation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Novelty.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

namespace test_neuromodulation_integration {

class NeuromodulationSystemIntegrationTest {
public:
    void runAllTests() {
        std::cout << "=== Neuromodulation System Integration Tests ===" << std::endl;
        
        testDopamineIntegration();
        testCuriosityIntegration();
        testNoveltyIntegration();
        testPredictionErrorIntegration();
        testRewardIntegration();
        testNeuromodulatorCohesion();
        testNeuromodulationPerformanceBenchmarks();
        
        std::cout << "\n=== Neuromodulation System Integration Tests Complete ===" << std::endl;
    }
    
private:
    void testDopamineIntegration() {
        std::cout << "\n[TEST] Dopamine Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& neuromodulation = brain.getNeuromodulation();
        
        // Test dopamine level recording
        neuromodulation.recordDopamine(0, 0.9f);
        neuromodulation.recordDopamine(1, 0.7f);
        neuromodulation.recordDopamine(2, 0.5f);
        
        // Test dopamine effect on plasticity
        neuromodulation.applyDopamineEffect(0, 0.8f);
        neuromodulation.applyDopamineEffect(1, 0.6f);
        
        // Test dopamine decay
        neuromodulation.update(0.1f);
        
        // Test dopamine threshold effects
        neuromodulation.recordPredictionError(0, 0.3f);  // Should increase dopamine
        neuromodulation.recordReward(0, 1.0f);         // Should increase dopamine
        
        // Verify dopamine system properties
        assert(neuromodulation.getDopamineLevel(0) > 0.0f);
        assert(neuromodulation.getDopamineLevel(1) > 0.0f);
        assert(neuromodulation.getDopamineLevel(2) > 0.0f);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Dopamine integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testCuriosityIntegration() {
        std::cout << "\n[TEST] Curiosity Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& neuromodulation = brain.getNeuromodulation();
        
        // Test curiosity level recording
        neuromodulation.recordCuriosity(0, 0.8f);
        neuromodulation.recordCuriosity(1, 0.6f);
        neuromodulation.recordCuriosity(2, 0.4f);
        
        // Test curiosity-driven exploration
        neuromodulation.triggerExploration(0, 0.9f);
        neuromodulation.triggerExploration(1, 0.7f);
        
        // Test curiosity decay and renewal
        neuromodulation.update(0.1f);
        neuromodulation.recordCuriosity(0, 0.9f);  // Renew curiosity
        
        // Test curiosity modulation of learning
        neuromodulation.recordNovelty(0, 0.5f);    // High novelty increases curiosity
        neuromodulation.recordPredictionError(0, 0.4f);  // Prediction error increases curiosity
        
        // Verify curiosity system properties
        assert(neuromodulation.getCuriosityLevel(0) > 0.0f);
        assert(neuromodulation.getCuriosityLevel(1) > 0.0f);
        assert(neuromodulation.getExplorationDrive(0) > 0.0f);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Curiosity integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testNoveltyIntegration() {
        std::cout << "\n[TEST] Novelty Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& neuromodulation = brain.getNeuromodulation();
        
        // Test novelty detection
        neuromodulation.recordNovelty(0, 0.9f);
        neuromodulation.recordNovelty(1, 0.7f);
        neuromodulation.recordNovelty(2, 0.5f);
        
        // Test novelty-triggered behaviors
        neuromodulation.triggerNoveltyResponse(0, nlm::NoveltyResponse::Explore);
        neuromodulation.triggerNoveltyResponse(1, nlm::NoveltyResponse::Learn);
        neuromodulation.triggerNoveltyResponse(2, nlm::NoveltyResponse::Remember);
        
        // Test novelty decay
        neuromodulation.update(0.1f);
        neuromodulation.recordNovelty(0, 0.8f);  // New novelty
        
        // Test novelty integration with other neuromodulators
        neuromodulation.recordPredictionError(0, 0.6f);  // Prediction error with novelty
        neuromodulation.recordCuriosity(0, 0.7f);        // Novelty increases curiosity
        
        // Verify novelty system properties
        assert(neuromodulation.getNoveltyLevel(0) > 0.0f);
        assert(neuromodulation.getNoveltyLevel(1) > 0.0f);
        assert(neuromodulation.getNoveltyResponseCount() > 0);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Novelty integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testPredictionErrorIntegration() {
        std::cout << "\n[TEST] Prediction Error Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& neuromodulation = brain.getNeuromodulation();
        
        // Test prediction error recording
        neuromodulation.recordPredictionError(0, 0.9f);
        neuromodulation.recordPredictionError(1, 0.7f);
        neuromodulation.recordPredictionError(2, 0.5f);
        
        // Test prediction error effects
        neuromodulation.applyPredictionErrorEffect(0, 0.8f);
        neuromodulation.applyPredictionErrorEffect(1, 0.6f);
        
        // Test prediction error decay
        neuromodulation.update(0.1f);
        neuromodulation.recordPredictionError(0, 0.6f);  // New prediction error
        
        // Test prediction error integration with learning systems
        neuromodulation.recordDopamine(0, 0.5f);      // Prediction error modulates dopamine
        neuromodulation.recordCuriosity(0, 0.8f);      // Prediction error increases curiosity
        neuromodulation.recordNovelty(0, 0.3f);        // Prediction error detected as novelty
        
        // Verify prediction error system properties
        assert(neuromodulation.getPredictionErrorLevel(0) > 0.0f);
        assert(neuromodulation.getPredictionErrorLevel(1) > 0.0f);
        assert(neuromodulation.getLearningModulationIndex() > 0.0f);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Prediction error integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testRewardIntegration() {
        std::cout << "\n[TEST] Reward Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& neuromodulation = brain.getNeuromodulation();
        
        // Test reward recording
        neuromodulation.recordReward(0, 1.0f);
        neuromodulation.recordReward(1, 0.8f);
        neuromodulation.recordReward(2, 0.6f);
        
        // Test reward effects
        neuromodulation.applyRewardEffect(0, 0.9f);
        neuromodulation.applyRewardEffect(1, 0.7f);
        
        // Test reward decay
        neuromodulation.update(0.1f);
        neuromodulation.recordReward(0, 0.8f);  // New reward
        
        // Test reward integration with motivation systems
        neuromodulation.recordDopamine(0, 0.8f);      // Reward increases dopamine
        neuromodulation.recordCuriosity(0, 0.3f);      // Reward decreases curiosity (satisfaction)
        neuromodulation.recordPredictionError(0, 0.2f);  // Reward reduces prediction error
        
        // Verify reward system properties
        assert(neuromodulation.getRewardLevel(0) > 0.0f);
        assert(neuromodulation.getRewardLevel(1) > 0.0f);
        assert(neuromodulation.getMotivationLevel(0) > 0.0f);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Reward integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testNeuromodulatorCohesion() {
        std::cout << "\n[TEST] Neuromodulator Cohesion" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& neuromodulation = brain.getNeuromodulation();
        nlm::SimulationStep step = 0;
        
        // Test cross-neuromodulator interactions
        neuromodulation.recordDopamine(0, 0.9f);
        neuromodulation.recordCuriosity(0, 0.7f);
        neuromodulation.recordNovelty(0, 0.5f);
        neuromodulation.recordPredictionError(0, 0.3f);
        neuromodulation.recordReward(0, 0.8f);
        
        // Apply integrated neuromodulation effects
        neuromodulation.applyIntegratedModulation(step);
        
        // Test neuromodulator feedback loops
        neuromodulation.triggerPlasticityModulation(step);
        neuromodulation.triggerLearningModulation(step);
        neuromodulation.triggerAttentionModulation(step);
        
        // Verify neuromodulator integration
        assert(neuromodulation.getIntegratedModulationLevel() > 0.0f);
        assert(neuromodulation.getPlasticityModulationLevel() > 0.0f);
        assert(neuromodulation.getLearningModulationLevel() > 0.0f);
        assert(neuromodulation.getAttentionModulationLevel() > 0.0f);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Neuromodulator cohesion test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testNeuromodulationPerformanceBenchmarks() {
        std::cout << "\n[TEST] Neuromodulation Performance Benchmarks" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        struct BenchmarkConfig {
            int neuron_count;
            int neuromodulator_count;
        };
        
        std::vector<BenchmarkConfig> configs = {
            {50, 5},
            {100, 10},
            {200, 20}
        };
        
        for (const auto& config : configs) {
            std::cout << "    Testing with " << config.neuron_count << " neurons..." << std::endl;
            
            auto test_config = std::make_shared<nlm::Config>();
            test_config->set("neuron_count", static_cast<int64_t>(config.neuron_count), nlm::ConfigSource::Default);
            
            nlm::Brain brain(test_config);
            assert(brain.initialize());
            
            nlm::SimulationStep step = 0;
            
            // Record neuromodulator levels
            for (int i = 0; i < config.neuromodulator_count; ++i) {
                brain.getNeuromodulation().recordDopamine(i, 0.5f);
                brain.getNeuromodulation().recordCuriosity(i, 0.4f);
                brain.getNeuromodulation().recordNovelty(i, 0.3f);
                brain.getNeuromodulation().recordPredictionError(i, 0.2f);
                brain.getNeuromodulation().recordReward(i, 0.6f);
            }
            
            // Apply neuromodulation effects
            brain.getNeuromodulation().applyIntegratedModulation(step);
            
            // Run simulation steps
            for (int i = 1; i < 10; ++i) {
                brain.step(step + i);
                brain.getNeuromodulation().update(0.01f);
            }
        }
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Neuromodulation performance benchmarks completed in " << duration_ms << "ms" << std::endl;
    }
};

void runAllNeuromodulationIntegrationTests() {
    NeuromodulationSystemIntegrationTest tester;
    tester.runAllTests();
}

} // namespace test_neuromodulation_integration
