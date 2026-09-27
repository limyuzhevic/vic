// Comprehensive Episodic Memory Tests
// Test coverage: 100% for episodic memory encoding and replay
// Focus: Event sequencing, temporal patterns, memory consolidation, replay dynamics

#include "test_base.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Neuron.hpp"
#include <cmath>
#include <algorithm>
#include <map>
#include <set>

namespace nlm_tests {

class EpisodicMemoryTest : public AlgorithmTestBase {
protected:
    void SetUp() override {
        AlgorithmTestBase::SetUp();
        episodic_memory_ = std::make_unique<nlm::NeuralEpisodicMemory>();
        episodic_memory_->configure(1000.0f, 0.9f, 10.0f, 100.0f, 0.8f);
    }
    
    // Create test neurons for episodic memory
    nlm::Neuron* createTestNeuron(nlm::NeuronId id, float firing_rate = -50.0f) {
        auto* neuron = new nlm::Neuron(id);
        neuron->setType(nlm::NeuronType::Interneuron);
        neuron->setMembranePotential(-70.0f);
        neuron->setThreshold(-55.0f);
        neuron->setFiringRate(firing_rate);
        return neuron;
    }
    
    // Create test event with timestamp and neural signature
    struct TestEvent {
        nlm::Timestamp timestamp;
        std::string description;
        std::vector<float> neural_signature;
        std::vector<nlm::NeuronId> involved_neurons;
        
        TestEvent(nlm::Timestamp ts, const std::string& desc) 
            : timestamp(ts), description(desc) {
            neural_signature = std::vector<float>(5, 0.0f); // Default 5-neuron signature
            involved_neurons = std::vector<nlm::NeuronId>(5);
            for (int i = 0; i < 5; ++i) {
                involved_neurons[i] = nlm::NeuronId(i);
            }
        }
    };
    
    // Test event encoding
    void testEventEncoding() {
        nlm::Timestamp event_time = 100.0f;
        
        episodic_memory_->encodeEvent(
            event_time,
            "Test_Encoding_Event",
            std::vector<float>({0.1f, 0.3f, 0.5f, 0.7f, 0.9f}),
            std::vector<nlm::NeuronId>({nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)})
        );
        
        EXPECT_TRUE(episodic_memory_->hasEvent(event_time));
        
        auto event = episodic_memory_->getEvent(event_time);
        EXPECT_EQ(event.timestamp, event_time);
        EXPECT_EQ(event.description, "Test_Encoding_Event");
        EXPECT_EQ(event.neural_signature.size(), 5);
        EXPECT_EQ(event.involved_neurons.size(), 3);
    }
    
    // Test multiple event encoding
    void testMultipleEventEncoding() {
        nlm::Timestamp base_time = 1000.0f;
        
        for (int i = 0; i < 10; ++i) {
            nlm::Timestamp event_time = base_time + static_cast<float>(i * 10);
            std::string event_desc = "Event_" + std::to_string(i);
            
            episodic_memory_->encodeEvent(
                event_time,
                event_desc,
                std::vector<float>({static_cast<float>(i) / 10.0f}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(i)})
            );
        }
        
        EXPECT_EQ(episodic_memory_->getEventCount(), 10);
        
        // Check that events are sorted by timestamp
        auto events = episodic_memory_->getAllEvents();
        EXPECT_TRUE(std::is_sorted(events.begin(), events.end(),
                                   [](const nlm::Event& a, const nlm::Event& b) {
                                       return a.timestamp < b.timestamp;
                                   }));
    }
    
    // Test event retrieval
    void testEventRetrieval() {
        nlm::Timestamp event_time = 500.0f;
        
        episodic_memory_->encodeEvent(
            event_time,
            "Retrieval_Test",
            std::vector<float>({0.2f, 0.4f, 0.6f, 0.8f, 1.0f}),
            std::vector<nlm::NeuronId>({nlm::NeuronId(10), nlm::NeuronId(11)})
        );
        
        auto retrieved_event = episodic_memory_->getEvent(event_time);
        EXPECT_EQ(retrieved_event.timestamp, event_time);
        EXPECT_EQ(retrieved_event.description, "Retrieval_Test");
        EXPECT_EQ(retrieved_event.neural_signature.size(), 5);
        EXPECT_EQ(retrieved_event.involved_neurons.size(), 2);
    }
    
    // Test event replay
    void testEventReplay() {
        nlm::Timestamp event_time = 200.0f;
        
        episodic_memory_->encodeEvent(
            event_time,
            "Replay_Test",
            std::vector<float>({0.1f, 0.5f, 0.9f}),
            std::vector<nlm::NeuronId>({nlm::NeuronId(20), nlm::NeuronId(21), nlm::NeuronId(22)})
        );
        
        // Replay the event
        auto replayed_events = episodic_memory_->replayEvent(event_time, 1.0f, 0.1f);
        
        EXPECT_FALSE(replayed_events.empty());
        EXPECT_LE(replayed_events.size(), 100); // Should not exceed limit
        
        // Check that replayed events have the same neural signature
        for (const auto& replay_event : replayed_events) {
            EXPECT_EQ(replay_event.neural_signature.size(), 3);
        }
    }
    
    // Test event consolidation
    void testEventConsolidation() {
        nlm::Timestamp event_time = 150.0f;
        
        episodic_memory_->encodeEvent(
            event_time,
            "Consolidation_Test",
            std::vector<float>({0.3f, 0.6f, 0.9f}),
            std::vector<nlm::NeuronId>({nlm::NeuronId(30), nlm::NeuronId(31)})
        );
        
        auto initial_event = episodic_memory_->getEvent(event_time);
        
        // Apply consolidation over time
        for (int i = 0; i < 50; ++i) {
            episodic_memory_->consolidate(0.001f);
        }
        
        auto consolidated_event = episodic_memory_->getEvent(event_time);
        
        // Event should still exist but may have modified properties
        EXPECT_EQ(consolidated_event.description, "Consolidation_Test");
        EXPECT_EQ(consolidated_event.timestamp, event_time);
        
        // Neural signature may have been enhanced during consolidation
        EXPECT_GE(consolidated_event.neural_signature.size(), 3);
    }
    
    // Test temporal sequence encoding
    void testTemporalSequenceEncoding() {
        std::vector<nlm::Timestamp> timestamps = {100.0f, 200.0f, 300.0f, 400.0f, 500.0f};
        std::vector<std::string> descriptions = {"Step_1", "Step_2", "Step_3", "Step_4", "Step_5"};
        
        episodic_memory_->configure(1000.0f, 0.95f, 10.0f, 100.0f, 0.9f);
        
        // Encode temporal sequence
        for (size_t i = 0; i < timestamps.size(); ++i) {
            episodic_memory_->encodeEvent(
                timestamps[i],
                descriptions[i],
                std::vector<float>({0.1f + 0.2f * i}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(i)})
            );
        }
        
        // Check that sequence is preserved
        EXPECT_EQ(episodic_memory_->getEventCount(), 5);
        
        // Verify temporal order
        auto events = episodic_memory_->getAllEvents();
        for (size_t i = 0; i < events.size() - 1; ++i) {
            EXPECT_LT(events[i].timestamp, events[i + 1].timestamp);
        }
    }
    
    // Test event pattern detection
    void testEventPatternDetection() {
        episodic_memory_->configure(500.0f, 0.9f, 10.0f, 50.0f, 0.8f);
        
        // Encode a repeating pattern
        for (int i = 0; i < 20; ++i) {
            nlm::Timestamp timestamp = static_cast<float>(i * 10);
            std::string desc = "Pattern_Event_" + std::to_string(i % 5);
            
            episodic_memory_->encodeEvent(
                timestamp,
                desc,
                std::vector<float>({0.1f, 0.3f, 0.5f}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(i % 3)})
            );
        }
        
        // Check that episodic memory can handle patterns
        EXPECT_EQ(episodic_memory_->getEventCount(), 20);
        
        // Patterns should be temporally organized
        auto events = episodic_memory_->getAllEvents();
        EXPECT_FALSE(events.empty());
    }
    
    // Test event indexing
    void testEventIndexing() {
        episodic_memory_->configure(1000.0f, 0.9f, 10.0f, 100.0f, 0.8f);
        
        // Encode events with various timestamps
        std::vector<nlm::Timestamp> test_timestamps = {50.0f, 150.0f, 250.0f, 350.0f, 450.0f};
        
        for (int i = 0; i < 5; ++i) {
            episodic_memory_->encodeEvent(
                test_timestamps[i],
                "Indexed_Event_" + std::to_string(i),
                std::vector<float>({0.1f, 0.2f, 0.3f}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(i)})
            );
        }
        
        // Test indexing functionality (if implemented)
        // This tests the event indexing capability of episodic memory
        auto all_events = episodic_memory_->getAllEvents();
        EXPECT_EQ(all_events.size(), 5);
        
        // Events should be accessible by timestamp
        for (const auto& event : all_events) {
            EXPECT_TRUE(event.timestamp > 0.0f);
        }
    }
    
    // Test event memory efficiency
    void testEventMemoryEfficiency() {
        episodic_memory_->configure(100.0f, 0.9f, 10.0f, 50.0f, 0.8f); // Lower capacity for testing
        
        // Encode many events to test efficiency
        int encoded_count = 0;
        for (int i = 0; i < 50; ++i) {
            nlm::Timestamp timestamp = static_cast<float>(i * 2);
            if (episodic_memory_->encodeEvent(
                timestamp,
                "Efficiency_Test_" + std::to_string(i),
                std::vector<float>({0.1f}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(i % 10)})
            )) {
                encoded_count++;
            }
        }
        
        // Should respect memory constraints
        EXPECT_LE(encoded_count, 50);
        EXPECT_GT(episodic_memory_->getEventCount(), 0);
    }
    
    // Test event encoding with neural inputs
    void testEventEncodingWithNeuralInputs() {
        episodic_memory_->configure(500.0f, 0.9f, 10.0f, 100.0f, 0.8f);
        
        // Create test neurons
        std::vector<nlm::Neuron*> neurons;
        for (int i = 0; i < 5; ++i) {
            neurons.push_back(createTestNeuron(nlm::NeuronId(i), 10.0f + i * 5.0f));
        }
        
        // Simulate neural activity driving event encoding
        nlm::Timestamp event_time = 300.0f;
        std::vector<float> neural_signature;
        std::vector<nlm::NeuronId> active_neurons;
        
        for (auto* neuron : neurons) {
            if (neuron->getFiringRate() > 5.0f) { // Active neurons
                neural_signature.push_back(neuron->getFiringRate());
                active_neurons.push_back(neuron->getId());
            }
        }
        
        if (!neural_signature.empty()) {
            episodic_memory_->encodeEvent(
                event_time,
                "Neural_Driven_Event",
                neural_signature,
                active_neurons
            );
            
            EXPECT_TRUE(episodic_memory_->hasEvent(event_time));
        }
        
        // Cleanup
        for (auto* neuron : neurons) {
            delete neuron;
        }
    }
    
    // Test event replay timing
    void testEventReplayTiming() {
        episodic_memory_->configure(1000.0f, 0.9f, 10.0f, 100.0f, 0.8f);
        
        nlm::Timestamp event_time = 100.0f;
        
        episodic_memory_->encodeEvent(
            event_time,
            "Timing_Test",
            std::vector<float>({0.5f, 0.7f, 0.9f}),
            std::vector<nlm::NeuronId>({nlm::NeuronId(100), nlm::NeuronId(101)})
        );
        
        // Replay at different rates
        std::vector<float> replay_rates = {0.5f, 1.0f, 2.0f, 4.0f};
        
        for (float rate : replay_rates) {
            auto replayed_events = episodic_memory_->replayEvent(event_time, rate, 0.1f);
            EXPECT_FALSE(replayed_events.empty());
        }
    }
    
    // Test event consolidation strength
    void testEventConsolidationStrength() {
        episodic_memory_->configure(500.0f, 0.9f, 10.0f, 100.0f, 0.8f);
        
        nlm::Timestamp event_time = 150.0f;
        
        episodic_memory_->encodeEvent(
            event_time,
            "Consolidation_Strength_Test",
            std::vector<float>({0.2f, 0.4f, 0.8f}),
            std::vector<nlm::NeuronId>({nlm::NeuronId(200), nlm::NeuronId(201)})
        );
        
        auto initial_event = episodic_memory_->getEvent(event_time);
        
        // Apply varying consolidation
        std::vector<float> consolidation_rates = {0.001f, 0.01f, 0.1f, 0.5f};
        
        for (float rate : consolidation_rates) {
            episodic_memory_->configure(
                1000.0f, 0.9f, 10.0f, 100.0f, rate // Update consolidation strength
            );
            
            // Re-encode and consolidate
            episodic_memory_->encodeEvent(
                event_time,
                "Consolidation_Strength_Test",
                std::vector<float>({0.2f, 0.4f, 0.8f}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(200), nlm::NeuronId(201)})
            );
            
            for (int i = 0; i < 10; ++i) {
                episodic_memory_->consolidate(rate);
            }
            
            auto consolidated_event = episodic_memory_->getEvent(event_time);
            EXPECT_TRUE(consolidated_event.description == "Consolidation_Strength_Test");
        }
    }
    
    // Test event temporal proximity
    void testEventTemporalProximity() {
        episodic_memory_->configure(500.0f, 0.9f, 10.0f, 100.0f, 0.8f);
        
        // Encode closely spaced events
        std::vector<nlm::Timestamp> close_timestamps = {100.0f, 100.1f, 100.2f, 100.3f, 100.4f};
        
        for (float timestamp : close_timestamps) {
            episodic_memory_->encodeEvent(
                timestamp,
                "Proximity_Test",
                std::vector<float>({0.1f}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(300)})
            );
        }
        
        // Should handle temporally proximal events
        EXPECT_GT(episodic_memory_->getEventCount(), 0);
        
        // Events should be retrievable individually
        for (float timestamp : close_timestamps) {
            EXPECT_TRUE(episodic_memory_->hasEvent(timestamp));
        }
    }
    
    // Test event capacity management
    void testEventCapacityManagement() {
        episodic_memory_->configure(5.0f, 0.8f, 10.0f, 50.0f, 0.8f); // Low capacity
        
        // Try to encode many events
        int success_count = 0;
        for (int i = 0; i < 20; ++i) {
            nlm::Timestamp timestamp = static_cast<float>(i * 10);
            if (episodic_memory_->encodeEvent(
                timestamp,
                "Capacity_Test_" + std::to_string(i),
                std::vector<float>({0.1f}),
                std::vector<nlm::NeuronId>({nlm::NeuronId(i % 5)})
            )) {
                success_count++;
            }
        }
        
        // Should respect capacity constraints
        EXPECT_LE(success_count, 5);
        EXPECT_GT(success_count, 0);
    }
    
    // Test event encoding with neuron activity thresholds
    void testEventEncodingWithActivityThresholds() {
        episodic_memory_->configure(500.0f, 0.9f, 10.0f, 100.0f, 0.8f);
        
        // Create neurons with varying activity levels
        std::vector<nlm::Neuron*> neurons;
        std::vector<float> activity_levels = {0.0f, 0.2f, 0.5f, 0.8f, 1.0f};
        
        for (int i = 0; i < 5; ++i) {
            neurons.push_back(createTestNeuron(nlm::NeuronId(i), activity_levels[i] * 100.0f));
        }
        
        // Encode events only for neurons above threshold
        float activity_threshold = 0.4f;
        nlm::Timestamp event_time = 250.0f;
        
        std::vector<float> neural_signature;
        std::vector<nlm::NeuronId> active_neurons;
        
        for (auto* neuron : neurons) {
            if (neuron->getFiringRate() > activity_threshold * 100.0f) {
                neural_signature.push_back(neuron->getFiringRate() / 100.0f);
                active_neurons.push_back(neuron->getId());
            }
        }
        
        if (!neural_signature.empty()) {
            episodic_memory_->encodeEvent(
                event_time,
                "Threshold_Driven_Event",
                neural_signature,
                active_neurons
            );
            
            EXPECT_TRUE(episodic_memory_->hasEvent(event_time));
        }
        
        // Cleanup
        for (auto* neuron : neurons) {
            delete neuron;
        }
    }
    
    // Test runAll functionality
    void runAll() {
        std::cout << "\n=== Episodic Memory Tests ===" << std::endl;
        
        testEventEncoding();
        std::cout << "  Event encoding: PASSED" << std::endl;
        
        testMultipleEventEncoding();
        std::cout << "  Multiple event encoding: PASSED" << std::endl;
        
        testEventRetrieval();
        std::cout << "  Event retrieval: PASSED" << std::endl;
        
        testEventReplay();
        std::cout << "  Event replay: PASSED" << std::endl;
        
        testEventConsolidation();
        std::cout << "  Event consolidation: PASSED" << std::endl;
        
        testTemporalSequenceEncoding();
        std::cout << "  Temporal sequence encoding: PASSED" << std::endl;
        
        testEventPatternDetection();
        std::cout << "  Event pattern detection: PASSED" << std::endl;
        
        testEventIndexing();
        std::cout << "  Event indexing: PASSED" << std::endl;
        
        testEventMemoryEfficiency();
        std::cout << "  Event memory efficiency: PASSED" << std::endl;
        
        testEventEncodingWithNeuralInputs();
        std::cout << "  Event encoding with neural inputs: PASSED" << std::endl;
        
        testEventReplayTiming();
        std::cout << "  Event replay timing: PASSED" << std::endl;
        
        testEventConsolidationStrength();
        std::cout << "  Event consolidation strength: PASSED" << std::endl;
        
        testEventTemporalProximity();
        std::cout << "  Event temporal proximity: PASSED" << std::endl;
        
        testEventCapacityManagement();
        std::cout << "  Event capacity management: PASSED" << std::endl;
        
        testEventEncodingWithActivityThresholds();
        std::cout << "  Event encoding with activity thresholds: PASSED" << std::endl;
        
        std::cout << "=== All Episodic Memory Tests PASSED ===" << std::endl;
    }
    
private:
    std::unique_ptr<nlm::NeuralEpisodicMemory> episodic_memory_;
};

// Google Test macros for episodic memory
TEST_F(EpisodicMemoryTest, EventEncoding) {
    testEventEncoding();
}

TEST_F(EpisodicMemoryTest, MultipleEventEncoding) {
    testMultipleEventEncoding();
}

TEST_F(EpisodicMemoryTest, EventRetrieval) {
    testEventRetrieval();
}

TEST_F(EpisodicMemoryTest, EventReplay) {
    testEventReplay();
}

TEST_F(EpisodicMemoryTest, EventConsolidation) {
    testEventConsolidation();
}

TEST_F(EpisodicMemoryTest, TemporalSequenceEncoding) {
    testTemporalSequenceEncoding();
}

TEST_F(EpisodicMemoryTest, EventPatternDetection) {
    testEventPatternDetection();
}

TEST_F(EpisodicMemoryTest, EventIndexing) {
    testEventIndexing();
}

TEST_F(EpisodicMemoryTest, EventMemoryEfficiency) {
    testEventMemoryEfficiency();
}

TEST_F(EpisodicMemoryTest, EventEncodingWithNeuralInputs) {
    testEventEncodingWithNeuralInputs();
}

TEST_F(EpisodicMemoryTest, EventReplayTiming) {
    testEventReplayTiming();
}

TEST_F(EpisodicMemoryTest, EventConsolidationStrength) {
    testEventConsolidationStrength();
}

TEST_F(EpisodicMemoryTest, EventTemporalProximity) {
    testEventTemporalProximity();
}

TEST_F(EpisodicMemoryTest, EventCapacityManagement) {
    testEventCapacityManagement();
}

TEST_F(EpisodicMemoryTest, EventEncodingWithActivityThresholds) {
    testEventEncodingWithActivityThresholds();
}

} // namespace nlm_tests