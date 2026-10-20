// Memory System Integration Tests
// Tests all memory systems (working + episodic + associative) integration

#include "brain/memory/BrainMemory.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

namespace test_memory_integration {

class MemorySystemIntegrationTest {
public:
    void runAllTests() {
        std::cout << "=== Memory System Integration Tests ===" << std::endl;
        
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        testAssociativeMemoryIntegration();
        testMemorySystemCohesion();
        testMemoryPerformanceBenchmarks();
        
        std::cout << "\n=== Memory System Integration Tests Complete ===" << std::endl;
    }
    
private:
    void testWorkingMemoryIntegration() {
        std::cout << "\n[TEST] Working Memory Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& workingMemory = brain.getWorkingMemory();
        
        // Test pattern storage and retrieval
        workingMemory.store("pattern_1", 0.9f, 100, nlm::MemoryModule::Working);
        workingMemory.store("pattern_2", 0.7f, 200, nlm::MemoryModule::Working);
        
        // Test competitive activation
        workingMemory.activatePattern(0, "pattern_1");
        workingMemory.activatePattern(1, "pattern_2");
        
        // Test decay
        workingMemory.update(0.1f);
        
        // Test capacity management
        for (int i = 0; i < 50; ++i) {
            workingMemory.store(std::to_string(i), 0.5f, i * 10, nlm::MemoryModule::Working);
        }
        
        // Verify working memory properties
        assert(workingMemory.getCapacity() > 0);
        assert(workingMemory.getActivePatternCount() > 0);
        assert(workingMemory.getTotalPatterns() > 0);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Working memory integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testEpisodicMemoryIntegration() {
        std::cout << "\n[TEST] Episodic Memory Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& episodicMemory = brain.getEpisodicMemory();
        
        // Test episode encoding
        episodicMemory.encode(0, "episode_1", "sensory_input_1", "motor_output_1");
        episodicMemory.encode(1, "episode_2", "sensory_input_2", "motor_output_2");
        episodicMemory.encode(2, "episode_3", "sensory_input_3", "motor_output_3");
        
        // Test memory replay
        episodicMemory.triggerReplay(0, 0.5f);
        episodicMemory.triggerReplay(1, 0.3f);
        
        // Test consolidation
        episodicMemory.triggerConsolidation();
        
        // Test memory strengthening
        episodicMemory.strengthenMemory(0, 0.8f);
        episodicMemory.strengthenMemory(1, 0.6f);
        
        // Verify episodic memory properties
        assert(episodicMemory.getMaxEpisodes() > 0);
        assert(episodicMemory.getCurrentEpisodeCount() > 0);
        assert(episodicMemory.getReplayActive() > 0);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Episodic memory integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testAssociativeMemoryIntegration() {
        std::cout << "\n[TEST] Associative Memory Integration" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& associativeMemory = brain.getAssociativeMemory();
        
        // Test pattern association recording
        associativeMemory.recordAssociation(0, 100, 0.9f, "pattern_A");
        associativeMemory.recordAssociation(1, 101, 0.8f, "pattern_B");
        associativeMemory.recordAssociation(2, 102, 0.7f, "pattern_C");
        
        // Test Hebbian learning integration
        associativeMemory.applyHebbianLearning(0, 100, 0.3f);
        associativeMemory.applyHebbianLearning(1, 101, 0.4f);
        
        // Test association strength updates
        associativeMemory.updateAssociationStrength(0, 0.95f);
        associativeMemory.updateAssociationStrength(1, 0.85f);
        
        // Test pattern completion
        associativeMemory.completePattern(0, "input_pattern", "output_pattern");
        associativeMemory.completePattern(1, "input_pattern2", "output_pattern2");
        
        // Verify associative memory properties
        assert(associativeMemory.getAssociationCount() > 0);
        assert(associativeMemory.getStrengthAverage() > 0.0f);
        assert(associativeMemory.getCompletenessScore() > 0.0f);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Associative memory integration test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testMemorySystemCohesion() {
        std::cout << "\n[TEST] Memory System Cohesion" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test cross-system integration
        nlm::SimulationStep step = 0;
        
        // Working memory stores patterns that episodic memory references
        brain.getWorkingMemory().store("working_pattern", 0.8f, step, nlm::MemoryModule::Working);
        brain.getEpisodicMemory().encode(step, "episode_with_working_memory", "working_ref", "working_output");
        
        // Episodic memory patterns feed into associative memory
        brain.getAssociativeMemory().recordAssociation(step, 100, 0.9f, "episode_pattern");
        
        // Associative memory influences working memory activation
        brain.getWorkingMemory().activatePattern(0, "episode_pattern");
        
        // All systems interact through the brain's main memory interface
        brain.step(step);
        
        // Verify all systems are interconnected
        assert(brain.getWorkingMemory().getTotalPatterns() > 0);
        assert(brain.getEpisodicMemory().getCurrentEpisodeCount() > 0);
        assert(brain.getAssociativeMemory().getAssociationCount() > 0);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Memory system cohesion test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testMemoryPerformanceBenchmarks() {
        std::cout << "\n[TEST] Memory Performance Benchmarks" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        struct BenchmarkConfig {
            int neuron_count;
            int pattern_count;
            int episode_count;
            int association_count;
        };
        
        std::vector<BenchmarkConfig> configs = {
            {50, 10, 5, 10},
            {100, 25, 10, 25},
            {200, 50, 20, 50}
        };
        
        for (const auto& config : configs) {
            std::cout << "    Testing with " << config.neuron_count << " neurons..." << std::endl;
            
            auto test_config = std::make_shared<nlm::Config>();
            test_config->set("neuron_count", static_cast<int64_t>(config.neuron_count), nlm::ConfigSource::Default);
            
            nlm::Brain brain(test_config);
            assert(brain.initialize());
            
            nlm::SimulationStep step = 0;
            
            // Store working memory patterns
            for (int i = 0; i < config.pattern_count; ++i) {
                brain.getWorkingMemory().store(std::to_string(i), 0.5f, step, nlm::MemoryModule::Working);
            }
            
            // Store episodic memories
            for (int i = 0; i < config.episode_count; ++i) {
                brain.getEpisodicMemory().encode(step, std::to_string(i), "input", "output");
            }
            
            // Store associative memories
            for (int i = 0; i < config.association_count; ++i) {
                brain.getAssociativeMemory().recordAssociation(step, i, 0.5f, "pattern");
            }
            
            // Run simulation steps
            for (int i = 1; i < 5; ++i) {
                brain.step(step + i);
            }
        }
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Memory performance benchmarks completed in " << duration_ms << "ms" << std::endl;
    }
};

void runAllMemoryIntegrationTests() {
    MemorySystemIntegrationTest tester;
    tester.runAllTests();
}

} // namespace test_memory_integration
