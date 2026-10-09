// Test for Performance Infrastructure Integration (MemoryPool, EventQueue, SparseConnectivity)
// Covers requirement: 6. Performance Infrastructure Tests

#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "performance/SparseConnectivity.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_performance_infrastructure {

void testMemoryPoolBasic() {
    nlm::MemoryPool pool;
    
    // Test initial state
    assert(pool.getAllocatedBlocks() == 0);
    assert(pool.getAvailableBlocks() == 0);
    
    // Allocate memory
    void* ptr = pool.allocate(sizeof(float));
    assert(ptr != nullptr);
    assert(pool.getAllocatedBlocks() == 1);
    
    // Deallocate
    pool.deallocate(ptr);
    assert(pool.getAllocatedBlocks() == 0);
    
    std::cout << "    testMemoryPoolBasic passed" << std::endl;
}

void testEventQueueBasic() {
    nlm::EventQueue queue;
    
    // Test initial state
    assert(queue.getEventCount() == 0);
    assert(queue.isEmpty());
    
    // Add event
    std::function<void()> event = [](){ std::cout << "Event executed" << std::endl; };
    queue.push(event);
    assert(queue.getEventCount() == 1);
    assert(!queue.isEmpty());
    
    // Execute event
    queue.executeNext();
    assert(queue.getEventCount() == 0);
    
    std::cout << "    testEventQueueBasic passed" << std::endl;
}

void testEventQueuePriority() {
    nlm::EventQueue queue;
    
    // Add multiple events
    std::vector<bool> executed(3, false);
    
    auto event1 = [&](){ executed[0] = true; };
    auto event2 = [&](){ executed[1] = true; };
    auto event3 = [&](){ executed[2] = true; };
    
    queue.push(event1);
    queue.push(event2);
    queue.push(event3);
    
    // Execute all events
    while (!queue.isEmpty()) {
        queue.executeNext();
    }
    
    // All events should have been executed
    assert(executed[0]);
    assert(executed[1]);
    assert(executed[2]);
    
    std::cout << "    testEventQueuePriority passed" << std::endl;
}

void testSparseConnectivityBasic() {
    nlm::SparseConnectivity connectivity;
    
    // Test initial state
    assert(connectivity.getConnectionCount() == 0);
    assert(connectivity.getTotalPossibleConnections() > 0);
    
    // Add connection
    nlm::NeuronId pre = nlm::NeuronId(1);
    nlm::NeuronId post = nlm::NeuronId(2);
    bool connected = connectivity.addConnection(pre, post, 0.5f);
    assert(connected);
    assert(connectivity.getConnectionCount() == 1);
    assert(connectivity.getConnectionStrength(pre, post) == 0.5f);
    
    // Get connections
    auto connections = connectivity.getConnectionsFrom(pre);
    assert(connections.size() == 1);
    assert(connections[0] == post);
    
    // Check if connected
    assert(connectivity.areConnected(pre, post));
    
    // Update connection strength
    connectivity.updateConnectionStrength(pre, post, 0.2f);
    assert(connectivity.getConnectionStrength(pre, post) == 0.7f);
    
    // Remove connection
    connectivity.removeConnection(pre, post);
    assert(connectivity.getConnectionCount() == 0);
    assert(!connectivity.areConnected(pre, post));
    
    std::cout << "    testSparseConnectivityBasic passed" << std::endl;
}

void testSparseConnectivityEfficiency() {
    nlm::SparseConnectivity connectivity;
    
    // Create many potential connections
    const int numNeurons = 100;
    const float connectionProbability = 0.1f;
    
    for (int pre = 0; pre < numNeurons; ++pre) {
        for (int post = pre + 1; post < numNeurons; ++post) {
            if ((pre + post) % 5 == 0) { // ~10% chance
                connectivity.addConnection(
                    nlm::NeuronId(pre), 
                    nlm::NeuronId(post), 
                    static_cast<float>(rand()) / RAND_MAX
                );
            }
        }
    }
    
    // Sparse connectivity should have much fewer connections than complete graph
    int totalPossible = numNeurons * (numNeurons - 1) / 2;
    int actualConnections = connectivity.getConnectionCount();
    
    // Should be much less than total possible
    assert(actualConnections < totalPossible * 0.5f);
    
    std::cout << "    testSparseConnectivityEfficiency passed" << std::endl;
}

void testPerformanceInfrastructureIntegration() {
    nlm::MemoryPool pool;
    nlm::EventQueue queue;
    nlm::SparseConnectivity connectivity;
    
    // Test integrated performance
    // 1. Memory pool for storing events
    void* eventBuffer = pool.allocate(1024);
    assert(eventBuffer != nullptr);
    
    // 2. Event queue for neural events
    std::vector<bool> processed;
    auto neuralEvent = [&]() { processed.push_back(true); };
    queue.push(neuralEvent);
    
    // 3. Sparse connectivity for neural connections
    connectivity.addConnection(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
    
    // Execute event
    queue.executeNext();
    assert(processed.size() == 1);
    
    // All systems working together
    assert(connectivity.getConnectionCount() == 1);
    pool.deallocate(eventBuffer);
    
    std::cout << "    testPerformanceInfrastructureIntegration passed" << std::endl;
}

void runAll() {
    testMemoryPoolBasic();
    testEventQueueBasic();
    testEventQueuePriority();
    testSparseConnectivityBasic();
    testSparseConnectivityEfficiency();
    testPerformanceInfrastructureIntegration();
}

} // namespace test_performance_infrastructure
