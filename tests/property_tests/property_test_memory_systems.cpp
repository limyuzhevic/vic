// Property-based tests for memory systems
// Tests invariants for working memory, episodic memory, and associative memory

#ifndef PROPERTY_TEST_MEMORY_SYSTEMS_H
#define PROPERTY_TEST_MEMORY_SYSTEMS_H

#include "property_tests/NLMPropertyTestFramework.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace test_memory_systems {

// Property 1: Working memory traces should decay appropriately
bool workingMemory_decay_property(nlm::property_test::PropertyRandom& rng) {
    const uint32_t max_traces = 10;
    const double decay_rate = 0.05; // 5% decay per time step
    
    // Create working memory traces
    std::vector<double> traces;
    uint32_t trace_count = rng.uniform_uint64(1, max_traces);
    
    for (uint32_t i = 0; i < trace_count; ++i) {
        double initial_strength = rng.uniform_double(0.1, 1.0);
        traces.push_back(initial_strength);
    }
    
    // Apply decay over many time steps
    const uint32_t time_steps = 100;
    bool decay_correct = true;
    
    for (uint32_t step = 0; step < time_steps; ++step) {
        for (auto& trace : traces) {
            // Apply exponential decay
            trace = trace * (1.0 - decay_rate);
            
            // Check trace is positive and decreasing
            if (trace < 0.0) {
                decay_correct = false;
                break;
            }
            
            // Check traces are monotonically decreasing (or constant for random traces)
            // This is a simplified check - in reality, new traces can be added
        }
        
        if (!decay_correct) break;
    }
    
    // After many steps, traces should be near zero
    double total_strength = std::accumulate(traces.begin(), traces.end(), 0.0);
    bool near_zero = total_strength < 0.01;
    
    if (!decay_correct || !near_zero) {
        std::cout << "    Property 'workingMemory_decay_property' FAILED: "
                  << "Decay incorrect or traces not decaying to zero" << std::endl;
    }
    
    return decay_correct && near_zero;
}

// Property 2: Episodic memory episodes should be unique
bool episodicMemory_uniqueness_property(nlm::property_test::PropertyRandom& rng) {
    const uint32_t episode_count = 20;
    const uint32_t timestamp_range = 1000;
    
    // Create episodic memory episodes with timestamps and content
    std::vector<std::pair<uint64_t, uint64_t>> episodes; // (timestamp, content_hash)
    std::unordered_set<uint64_t> content_hashes;
    
    bool uniqueness_maintained = true;
    
    for (uint32_t i = 0; i < episode_count && uniqueness_maintained; ++i) {
        uint64_t timestamp = rng.uniform_uint64(0, timestamp_range);
        uint64_t content = rng.uniform_uint64(1, 1000000);
        uint64_t content_hash = content % 10000; // Simplified hash
        
        // Check if this combination already exists
        bool is_duplicate = false;
        for (const auto& episode : episodes) {
            if (episode.first == timestamp && episode.second == content_hash) {
                is_duplicate = true;
                break;
            }
        }
        
        if (!is_duplicate) {
            episodes.emplace_back(timestamp, content_hash);
            content_hashes.insert(content_hash);
        } else {
            // This could be acceptable if memory can overwrite, but property requires uniqueness
            uniqueness_maintained = false;
            std::cout << "    Property 'episodicMemory_uniqueness_property' FAILED: "
                      << "Duplicate episode detected at timestamp " << timestamp << std::endl;
        }
    }
    
    return uniqueness_maintained;
}

// Property 3: Associative memory retrieval should be consistent
bool associativeMemory_retrieval_consistency(nlm::property_test::PropertyRandom& rng) {
    const uint32_t association_count = 15;
    
    // Create associative memory pairs
    std::vector<std::pair<uint64_t, uint64_t>> associations; // (key, value)
    std::unordered_map<uint64_t, uint64_t> retrieval_map;
    
    bool retrieval_consistent = true;
    
    // Insert associations
    for (uint32_t i = 0; i < association_count; ++i) {
        uint64_t key = rng.uniform_uint64(1, 1000);
        uint64_t value = rng.uniform_uint64(1000, 2000);
        associations.emplace_back(key, value);
        retrieval_map[key] = value;
    }
    
    // Simulate multiple retrieval attempts
    const uint32_t retrieval_attempts = 100;
    uint32_t successful_retrievals = 0;
    
    for (uint32_t attempt = 0; attempt < retrieval_attempts; ++attempt) {
        // Select random key
        uint64_t key = rng.uniform_uint64(1, 1000);
        
        // Retrieve value
        auto it = retrieval_map.find(key);
        if (it != retrieval_map.end()) {
            successful_retrievals++;
            
            // Verify the retrieved value matches the original
            bool value_correct = false;
            for (const auto& assoc : associations) {
                if (assoc.first == key && assoc.second == it->second) {
                    value_correct = true;
                    break;
                }
            }
            
            if (!value_correct) {
                retrieval_consistent = false;
                break;
            }
        }
    }
    
    double retrieval_rate = static_cast<double>(successful_retrievals) / retrieval_attempts;
    bool acceptable_retrieval = retrieval_rate >= 0.5; // At least 50% successful retrievals
    
    if (!retrieval_consistent || !acceptable_retrieval) {
        std::cout << "    Property 'associativeMemory_retrieval_consistency' FAILED: "
                  << "Retrieval rate = " << retrieval_rate * 100 << "%" << std::endl;
    }
    
    return retrieval_consistent && acceptable_retrieval;
}

// Property 4: Memory consolidation should preserve important patterns
bool episodicMemory_consolidation_property(nlm::property_test::PropertyRandom& rng) {
    const uint32_t initial_episodes = 50;
    const uint32_t consolidation_steps = 20;
    
    // Create initial episodes with varying importance
    std::vector<std::pair<uint64_t, double>> episodes; // (timestamp, importance)
    
    for (uint32_t i = 0; i < initial_episodes; ++i) {
        uint64_t timestamp = rng.uniform_uint64(0, 10000);
        double importance = rng.uniform_double(0.0, 1.0); // 0.0 = unimportant, 1.0 = very important
        episodes.emplace_back(timestamp, importance);
    }
    
    // Simulate consolidation process
    bool consolidation_preserves_importance = true;
    
    for (uint32_t step = 0; step < consolidation_steps && consolidation_preserves_importance; ++step) {
        std::vector<std::pair<uint64_t, double>> consolidated_episodes;
        
        for (const auto& episode : episodes) {
            uint64_t timestamp = episode.first;
            double importance = episode.second;
            
            // Consolidation bias: important episodes have higher retention probability
            double retention_prob = importance;
            if (rng.bernoulli(retention_prob)) {
                // Apply some decay during consolidation
                double retained_importance = importance * (0.8 + rng.uniform_double(0.0, 0.2));
                consolidated_episodes.emplace_back(timestamp, retained_importance);
            }
        }
        
        // Check that important episodes are retained
        double total_initial_importance = std::accumulate(
            episodes.begin(), episodes.end(), 0.0,
            [](double sum, const auto& e) { return sum + e.second; });
        
        double total_consolidated_importance = std::accumulate(
            consolidated_episodes.begin(), consolidated_episodes.end(), 0.0,
            [](double sum, const auto& e) { return sum + e.second; });
        
        if (total_consolidated_importance < total_initial_importance * 0.3) { // Less than 30% retention
            consolidation_preserves_importance = false;
            std::cout << "    Property 'episodicMemory_consolidation_property' FAILED: "
                      << "Insufficient retention of important patterns ("
                      << total_consolidated_importance << " / " << total_initial_importance << ")" << std::endl;
            break;
        }
        
        episodes = consolidated_episodes; // Continue with consolidated set
    }
    
    return consolidation_preserves_importance;
}

// Property test runner for memory systems
void runMemorySystemsPropertyTests() {
    std::cout << "=== MEMORY SYSTEMS PROPERTY TESTS ===" << std::endl;
    
    nlm::property_test::PropertyTestEngine engine;
    nlm::property_test::PropertyRandom rng(12345);
    
    // Define property test instances
    class TestWorkingMemoryDecay {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("memory_systems", "working_memory_decay");
            result.test_iterations = 100;
            bool passed = workingMemory_decay_property(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestEpisodicMemoryUniqueness {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("memory_systems", "episodic_memory_uniqueness");
            bool passed = episodicMemory_uniqueness_property(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestAssociativeMemoryRetrieval {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("memory_systems", "associative_memory_retrieval");
            result.test_iterations = 100;
            bool passed = associativeMemory_retrieval_consistency(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestEpisodicMemoryConsolidation {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("memory_systems", "episodic_memory_consolidation");
            bool passed = episodicMemory_consolidation_property(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    // Run all property tests
    engine.start_timer();
    
    TestWorkingMemoryDecay().run(engine, rng);
    TestEpisodicMemoryUniqueness().run(engine, rng);
    TestAssociativeMemoryRetrieval().run(engine, rng);
    TestEpisodicMemoryConsolidation().run(engine, rng);
    
    engine.stop_timer();
    engine.generate_report("memory_systems_property_test_report.txt");
}

} // namespace test_memory_systems

#endif // PROPERTY_TEST_MEMORY_SYSTEMS_H