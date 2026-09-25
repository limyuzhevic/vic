// Spike Callback Tests
// Tests for spike callback registration, removal, and memory management
// Following the project code style with Doxygen-style comments

#include "SpikeSystem.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_spike_system {

// Test 1: Basic callback registration and invocation
// Verifies that a registered spike handler is called when a spike is processed
void testBasicCallbackRegistration() {
    nlm::SpikeSystem spikeSystem;
    
    int callbackCount = 0;
    auto callback = [&callbackCount](const nlm::DetailedSpikeEvent& event) {
        callbackCount++;
        assert(event.source.value > 0);
        assert(event.timestamp > 0.0);
    };
    
    spikeSystem.registerHandler(callback);
    assert(spikeSystem.getSpikeHandlerCount() == 1);
    
    // Create and queue a spike
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 1.0;
    event.step = 0;
    spikeSystem.queueSpike(event);
    
    // Process spikes
    spikeSystem.processSpikes(0);
    
    assert(callbackCount == 1);
    assert(spikeSystem.getSpikeCount() == 1);
    
    std::cout << "    testBasicCallbackRegistration passed" << std::endl;
}

// Test 2: Multiple callbacks
// Verifies that multiple spike handlers can be registered and all are called
void testMultipleCallbacks() {
    nlm::SpikeSystem spikeSystem;
    
    int callback1Count = 0;
    int callback2Count = 0;
    
    auto callback1 = [&callback1Count](const nlm::DetailedSpikeEvent& event) {
        callback1Count++;
    };
    
    auto callback2 = [&callback2Count](const nlm::DetailedSpikeEvent& event) {
        callback2Count++;
    };
    
    spikeSystem.registerHandler(callback1);
    spikeSystem.registerHandler(callback2);
    
    assert(spikeSystem.getSpikeHandlerCount() == 2);
    
    // Queue and process a spike
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(2);
    event.timestamp = 2.0;
    event.step = 1;
    spikeSystem.queueSpike(event);
    spikeSystem.processSpikes(1);
    
    assert(callback1Count == 1);
    assert(callback2Count == 1);
    
    std::cout << "    testMultipleCallbacks passed" << std::endl;
}

// Test 3: Callback removal
// Tests the removal of spike handlers by index and verifies callback is no longer invoked
void testCallbackRemoval() {
    nlm::SpikeSystem spikeSystem;
    
    int callbackCount = 0;
    auto callback = [&callbackCount](const nlm::DetailedSpikeEvent& event) {
        callbackCount++;
    };
    
    // Register and remove immediately
    spikeSystem.registerHandler(callback);
    assert(spikeSystem.getSpikeHandlerCount() == 1);
    
    bool removed = spikeSystem.removeHandler(0);
    assert(removed);
    assert(spikeSystem.getSpikeHandlerCount() == 0);
    
    // Try to remove non-existent handler
    bool removed2 = spikeSystem.removeHandler(0);
    assert(!removed2);
    
    // Verify callback is not invoked
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(3);
    event.timestamp = 3.0;
    event.step = 2;
    spikeSystem.queueSpike(event);
    spikeSystem.processSpikes(2);
    
    assert(callbackCount == 0);
    
    std::cout << "    testCallbackRemoval passed" << std::endl;
}

// Test 4: Lambda function support
// Tests that lambda functions can be used as spike callbacks
void testLambdaSupport() {
    nlm::SpikeSystem spikeSystem;
    
    int capturedValue = 100;
    auto callback = [capturedValue](const nlm::DetailedSpikeEvent& event) {
        // Use capturedValue in callback
        assert(event.source.value == capturedValue);
    };
    
    spikeSystem.registerHandler(callback);
    assert(spikeSystem.getSpikeHandlerCount() == 1);
    
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(capturedValue);
    event.timestamp = 4.0;
    event.step = 3;
    spikeSystem.queueSpike(event);
    spikeSystem.processSpikes(3);
    
    std::cout << "    testLambdaSupport passed" << std::endl;
}

// Test 5: Delayed spike callbacks
// Tests delayed spike handler registration and invocation
void testDelayedSpikeCallbacks() {
    nlm::SpikeSystem spikeSystem;
    
    int delayedCallbackCount = 0;
    auto delayedCallback = [&delayedCallbackCount](const nlm::DelayedSpikeEvent& event) {
        delayedCallbackCount++;
        assert(event.source_neuron.value > 0);
        assert(event.destination_neuron.value > 0);
    };
    
    spikeSystem.registerDelayedHandler(delayedCallback);
    assert(spikeSystem.getDelayedSpikeHandlerCount() == 1);
    
    // Create and queue a delayed spike
    nlm::DelayedSpikeEvent delayedEvent;
    delayedEvent.source_neuron = nlm::NeuronId(4);
    delayedEvent.destination_neuron = nlm::NeuronId(5);
    delayedEvent.synapse_id = nlm::SynapseId(1);
    delayedEvent.weight = 0.5f;
    delayedEvent.synapse_type = nlm::SynapseType::Excitatory;
    delayedEvent.timestamp = 5.0;
    delayedEvent.delivery_time = 5.5;
    delayedEvent.step = 4;
    delayedEvent.delivery_step = 5;
    delayedEvent.is_excitatory = true;
    
    spikeSystem.queueDelayedSpike(delayedEvent);
    assert(spikeSystem.getPendingDelayedCount() == 1);
    
    // Process delayed spikes
    spikeSystem.processDelayedSpikes(5, 5.5);
    
    assert(delayedCallbackCount == 1);
    assert(spikeSystem.getPendingDelayedCount() == 0);
    
    std::cout << "    testDelayedSpikeCallbacks passed" << std::endl;
}

// Test 6: Callback exception handling
// Tests that exceptions in callbacks don't crash the system
void testCallbackExceptionHandling() {
    nlm::SpikeSystem spikeSystem;
    
    int normalCallbackCount = 0;
    auto normalCallback = [&normalCallbackCount](const nlm::DetailedSpikeEvent& event) {
        normalCallbackCount++;
    };
    
    auto exceptionCallback = [](const nlm::DetailedSpikeEvent& event) {
        throw std::runtime_error("Test exception in callback");
    };
    
    spikeSystem.registerHandler(normalCallback);
    spikeSystem.registerHandler(exceptionCallback);
    
    // Queue and process a spike
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(6);
    event.timestamp = 6.0;
    event.step = 6;
    spikeSystem.queueSpike(event);
    
    // This should not crash even though one callback throws
    spikeSystem.processSpikes(6);
    
    // Normal callback should have been called
    assert(normalCallbackCount == 1);
    
    std::cout << "    testCallbackExceptionHandling passed" << std::endl;
}

// Test 7: Thread safety
// Tests that spike handlers can be registered and invoked safely from multiple threads
void testThreadSafety() {
    nlm::SpikeSystem spikeSystem;
    
    const int numCallbacks = 10;
    std::vector<int> callbackCounts(numCallbacks, 0);
    std::vector<std::function<void(const nlm::DetailedSpikeEvent&)>> callbacks;
    
    for (int i = 0; i < numCallbacks; ++i) {
        int index = i;
        auto callback = [index, &callbackCounts](const nlm::DetailedSpikeEvent& event) {
            callbackCounts[index]++;
        };
        callbacks.push_back(callback);
    }
    
    // Register all callbacks
    for (auto& callback : callbacks) {
        spikeSystem.registerHandler(callback);
    }
    
    assert(spikeSystem.getSpikeHandlerCount() == numCallbacks);
    
    // Create and queue spikes from multiple threads
    std::vector<std::thread> threads;
    const int numThreads = 5;
    const int spikesPerThread = 10;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&spikeSystem, t]() {
            for (int i = 0; i < spikesPerThread; ++i) {
                nlm::SpikeEvent event;
                event.source_neuron = nlm::NeuronId(t * 100 + i);
                event.timestamp = static_cast<double>(t * 100 + i);
                event.step = t * 10 + i;
                spikeSystem.queueSpike(event);
            }
        });
    }
    
    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Process all spikes
    for (int step = 0; step < 50; ++step) {
        spikeSystem.processSpikes(step);
    }
    
    // Verify all callbacks were called (thread safety)
    int totalCallbackInvocations = 0;
    for (int count : callbackCounts) {
        totalCallbackInvocations += count;
    }
    
    // All threads should have completed, so callbacks should have been called
    assert(totalCallbackInvocations == numCallbacks * numThreads * spikesPerThread);
    
    std::cout << "    testThreadSafety passed" << std::endl;
}

// Test 8: Callback index management
// Tests that callback indices remain correct after removal operations
void testCallbackIndexManagement() {
    nlm::SpikeSystem spikeSystem;
    
    // Register multiple callbacks
    for (int i = 0; i < 5; ++i) {
        auto callback = [i](const nlm::DetailedSpikeEvent& event) {
            assert(event.source.value == i);
        };
        spikeSystem.registerHandler(callback);
    }
    
    assert(spikeSystem.getSpikeHandlerCount() == 5);
    
    // Remove from middle
    bool removed = spikeSystem.removeHandler(2);
    assert(removed);
    assert(spikeSystem.getSpikeHandlerCount() == 4);
    
    // Verify remaining indices are correct
    bool removed2 = spikeSystem.removeHandler(0);
    assert(removed2);
    assert(spikeSystem.getSpikeHandlerCount() == 3);
    
    // Try to remove from now-out-of-bounds index
    bool removed3 = spikeSystem.removeHandler(0);  // This is now index 2 in original
    assert(removed3);
    assert(spikeSystem.getSpikeHandlerCount() == 2);
    
    std::cout << "    testCallbackIndexManagement passed" << std::endl;
}

// Test 9: Spike history tracking
// Tests that spike history is maintained correctly across multiple spikes
void testSpikeHistoryTracking() {
    nlm::SpikeSystem spikeSystem;
    
    // Process multiple spikes
    for (int i = 0; i < 15; ++i) {
        nlm::SpikeEvent event;
        event.source_neuron = nlm::NeuronId(i);
        event.timestamp = static_cast<double>(i);
        event.step = i;
        spikeSystem.queueSpike(event);
    }
    
    for (int step = 0; step < 15; ++step) {
        spikeSystem.processSpikes(step);
    }
    
    // History should contain all processed spikes
    assert(spikeSystem.getSpikeCount() == 15);
    assert(spikeSystem.getSpikeHistory().size() == 15);
    
    // Clear history
    spikeSystem.clearHistory();
    assert(spikeSystem.getSpikeCount() == 0);
    assert(spikeSystem.getSpikeHistory().empty());
    
    // Reset system
    spikeSystem.reset();
    assert(spikeSystem.getPendingSpikeCount() == 0);
    assert(spikeSystem.getSpikeCount() == 0);
    
    std::cout << "    testSpikeHistoryTracking passed" << std::endl;
}

// Test 10: Statistics and metrics
// Tests that spike statistics are calculated correctly
void testStatisticsAndMetrics() {
    nlm::SpikeSystem spikeSystem;
    
    // Process spikes with varying timing
    std::vector<double> timestamps = {1.0, 2.0, 3.0, 4.0, 5.0};
    for (size_t i = 0; i < timestamps.size(); ++i) {
        nlm::SpikeEvent event;
        event.source_neuron = nlm::NeuronId(i);
        event.timestamp = timestamps[i];
        event.step = i;
        spikeSystem.queueSpike(event);
    }
    
    for (size_t step = 0; step < timestamps.size(); ++step) {
        spikeSystem.processSpikes(static_cast<nlm::SimulationStep>(step));
    }
    
    // Test statistics
    assert(spikeSystem.getSpikeCount() == 5);
    
    float avgRate = spikeSystem.getAverageSpikeRate();
    assert(avgRate > 0.0f);
    
    auto mostActive = spikeSystem.getMostActiveNeurons(3);
    assert(mostActive.size() <= 3);
    
    std::cout << "    testStatisticsAndMetrics passed" << std::endl;
}

void runAll() {
    testBasicCallbackRegistration();
    testMultipleCallbacks();
    testCallbackRemoval();
    testLambdaSupport();
    testDelayedSpikeCallbacks();
    testCallbackExceptionHandling();
    testThreadSafety();
    testCallbackIndexManagement();
    testSpikeHistoryTracking();
    testStatisticsAndMetrics();
}

} // namespace test_spike_system