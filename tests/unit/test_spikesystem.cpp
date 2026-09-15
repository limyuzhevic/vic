// SpikeSystem Integration Tests
// Tests for true event-driven spike processing, delayed spike delivery, handler registration, and memory pool integration

#include "dynamics/SpikeSystem.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include "performance/MemoryPool.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>

namespace test_spikesystem {

void testEventDrivenProcessing() {
    nlm::SpikeSystem spikeSystem;
    
    // Track processed spikes
    int spikeCount = 0;
    spikeSystem.registerHandler([&](const nlm::DetailedSpikeEvent& event) {
        spikeCount++;
        // Verify spike event has required data
        assert(event.source.index() != 0);
        assert(event.timestamp >= 0.0f);
        assert(event.step >= 0);
    });
    
    // Queue spikes - they should be processed immediately (event-driven)
    nlm::SpikeEvent event1(nlm::NeuronId(1), 0.1f, 0);
    spikeSystem.queueSpike(event1);
    
    nlm::SpikeEvent event2(nlm::NeuronId(2), 0.2f, 1);
    spikeSystem.queueSpike(event2);
    
    assert(spikeCount == 2);
    
    // Verify spike history
    const auto& history = spikeSystem.getSpikeHistory();
    assert(history.size() == 2);
    assert(history[0].source.value == 1);
    assert(history[1].source.value == 2);
    
    std::cout << "    testEventDrivenProcessing passed" << std::endl;
}

void testDelayedSpikeDelivery() {
    nlm::SpikeSystem spikeSystem;
    
    // Track delayed spike delivery timing
    int deliveredCount = 0;
    spikeSystem.registerDelayedHandler([&](const nlm::DelayedSpikeEvent& event) {
        deliveredCount++;
        // Verify delivery timing
        assert(event.delivery_time >= event.timestamp);
        assert(event.is_delayed() == true);
    });
    
    // Queue delayed spike with 10 time step delay
    nlm::DelayedSpikeEvent delayedEvent(
        nlm::NeuronId(1),      // source
        nlm::NeuronId(2),      // destination
        nlm::SynapseId(1),     // synapse
        1.0f,                 // weight
        true,                 // is_excitatory
        0.1f,                 // timestamp
        1.1f,                 // delivery_time
        0,                    // step
        10                    // delay
    );
    
    spikeSystem.queueDelayedSpike(delayedEvent);
    
    // Process time steps - should only deliver at delivery time
    for (int step = 0; step <= 11; ++step) {
        spikeSystem.processDelayedSpikes(step, step * 0.1f);
    }
    
    assert(deliveredCount == 1);
    
    std::cout << "    testDelayedSpikeDelivery passed" << std::endl;
}

void testHandlerRegistration() {
    nlm::SpikeSystem spikeSystem;
    
    // Track which handlers were called
    int handler1Count = 0;
    int handler2Count = 0;
    
    auto handler1 = [&](const nlm::DetailedSpikeEvent& event) {
        handler1Count++;
    };
    
    auto handler2 = [&](const nlm::DetailedSpikeEvent& event) {
        handler2Count++;
    };
    
    spikeSystem.registerHandler(handler1);
    spikeSystem.registerHandler(handler2);
    
    // Queue spike
    nlm::SpikeEvent event(nlm::NeuronId(1), 0.1f, 0);
    spikeSystem.queueSpike(event);
    
    assert(handler1Count == 1);
    assert(handler2Count == 1);
    
    // Test delayed handlers too
    int delayedHandlerCount = 0;
    spikeSystem.registerDelayedHandler([&](const nlm::DelayedSpikeEvent& event) {
        delayedHandlerCount++;
    });
    
    nlm::DelayedSpikeEvent delayedEvent(
        nlm::NeuronId(1), nlm::NeuronId(2), nlm::SynapseId(1),
        1.0f, true, 0.1f, 1.1f, 0, 10
    );
    
    spikeSystem.queueDelayedSpike(delayedEvent);
    spikeSystem.processDelayedSpikes(11, 1.1f);
    
    assert(delayedHandlerCount == 1);
    
    std::cout << "    testHandlerRegistration passed" << std::endl;
}

void testMemoryPoolIntegration() {
    // Test MemoryPool class directly
    nlm::MemoryPool<int> intPool(100, 10);  // 100 objects per block, 10 blocks
    
    // Allocate objects
    std::vector<int*> allocated;
    for (int i = 0; i < 150; ++i) {  // More than initial capacity
        int* obj = intPool.allocate();
        if (obj) {
            allocated.push_back(obj);
            *obj = i;
        }
    }
    
    assert(allocated.size() == 150);
    
    // Verify object values
    for (size_t i = 0; i < allocated.size(); ++i) {
        assert(*allocated[i] == static_cast<int>(i));
    }
    
    // Deallocate all
    for (int* obj : allocated) {
        intPool.deallocate(obj);
    }
    
    // Check available count
    assert(intPool.availableCount() == 150);
    
    std::cout << "    testMemoryPoolIntegration passed" << std::endl;
}

void testSpikeCountStatistics() {
    nlm::SpikeSystem spikeSystem;
    
    // Queue multiple spikes from different neurons
    for (int i = 0; i < 10; ++i) {
        nlm::SpikeEvent event(nlm::NeuronId(i), static_cast<float>(i), i);
        spikeSystem.queueSpike(event);
    }
    
    // Check spike count
    size_t spikeCount = spikeSystem.getSpikeCount();
    assert(spikeCount == 10);
    
    // Check most active neurons
    std::vector<nlm::NeuronId> active = spikeSystem.getMostActiveNeurons(3);
    assert(active.size() == 3);
    
    // Calculate average spike rate
    float avgRate = spikeSystem.getAverageSpikeRate();
    assert(avgRate > 0.0f);
    
    std::cout << "    testSpikeCountStatistics passed" << std::endl;
}

void testSpikeSystemReset() {
    nlm::SpikeSystem spikeSystem;
    
    // Add some spikes
    for (int i = 0; i < 5; ++i) {
        nlm::SpikeEvent event(nlm::NeuronId(i), 0.1f * i, i);
        spikeSystem.queueSpike(event);
    }
    
    assert(spikeSystem.getSpikeCount() == 5);
    
    // Reset and verify
    spikeSystem.reset();
    assert(spikeSystem.getSpikeCount() == 0);
    assert(spikeSystem.getSpikeHistory().empty());
    
    std::cout << "    testSpikeSystemReset passed" << std::endl;
}

void runAll() {
    testEventDrivenProcessing();
    testDelayedSpikeDelivery();
    testHandlerRegistration();
    testMemoryPoolIntegration();
    testSpikeCountStatistics();
    testSpikeSystemReset();
    
    std::cout << std::endl;
    std::cout << "All SpikeSystem Integration Tests PASSED" << std::endl;
}

} // namespace test_spikesystem
