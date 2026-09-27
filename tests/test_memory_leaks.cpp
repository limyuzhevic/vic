// Memory Leak Tests
// Tests for memory management fixes in Pimpl classes
// 
// These tests ensure that:
// 1. Memory is properly freed when objects are destroyed
// 2. Move operations work correctly without memory leaks
// 3. Multiple object creation/destruction cycles don't leak memory
// 
// Memory leak detection:
// - Use AddressSanitizer (ASan) for production builds
// - For manual testing, check valgrind or similar tools
// - Monitor memory usage over time

#include "brain/Neuron.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/AntiHebbian.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory_leaks {

void testNeuronMemoryLeak() {
    // Test 1: Multiple neuron creation/destruction
    for (int i = 0; i < 100; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i));
        neuron.setType(nlm::NeuronType::Excitatory);
        neuron.setMembranePotential(-65.0f + static_cast<float>(i));
        
        // Use neuron
        neuron.setFiringState(nlm::FiringState::Active);
        neuron.setThreshold(-50.0f);
        
        // Neuron goes out of scope here - should be destroyed
    }
    
    // If we reach here without crashes, the basic memory management works
    std::cout << "    testNeuronMemoryLeak passed" << std::endl;
}

void testHebbianMemoryLeak() {
    // Test 1: Multiple Hebbian object creation/destruction
    for (int i = 0; i < 100; ++i) {
        nlm::Hebbian hebbian;
        hebbian.setLearningRate(0.01f + static_cast<float>(i) * 0.001f);
        hebbian.setMaxWeight(1.0f + static_cast<float>(i) * 0.1f);
        
        // Use hebbian
        nlm::SynapseHandle synapseHandle(1 + i);
        hebbian.applyWeightChange(synapseHandle, 0.1f);
        
        // Hebbian goes out of scope here - should be destroyed
    }
    
    std::cout << "    testHebbianMemoryLeak passed" << std::endl;
}

void testSTDPMemoryLeak() {
    // Test 1: Multiple STDP object creation/destruction
    for (int i = 0; i < 100; ++i) {
        nlm::STDP stdp;
        stdp.setLTPWeight(0.01f + static_cast<float>(i) * 0.001f);
        stdp.setLTDWeight(0.012f + static_cast<float>(i) * 0.001f);
        stdp.setTimeConstant(20.0f + static_cast<float>(i));
        
        // Use stdp
        nlm::SynapseHandle synapseHandle(2 + i);
        stdp.applyWeightChange(synapseHandle, 0.1f);
        
        // STDP goes out of scope here - should be destroyed
    }
    
    std::cout << "    testSTDPMemoryLeak passed" << std::endl;
}

void testAntiHebbianMemoryLeak() {
    // Test 1: Multiple AntiHebbian object creation/destruction
    for (int i = 0; i < 100; ++i) {
        nlm::AntiHebbianRule antiHebbian;
        antiHebbian.setLearningRate(0.01f + static_cast<float>(i) * 0.001f);
        antiHebbian.setDepressionRate(0.012f + static_cast<float>(i) * 0.001f);
        antiHebbian.setMinWeight(-1.0f - static_cast<float>(i));
        antiHebbian.setMaxWeight(1.0f + static_cast<float>(i));
        
        // Use antiHebbian
        nlm::SynapseHandle synapseHandle(3 + i);
        antiHebbian.applyWeightChange(synapseHandle, -0.1f);
        
        // AntiHebbian goes out of scope here - should be destroyed
    }
    
    std::cout << "    testAntiHebbianMemoryLeak passed" << std::endl;
}

void testMoveOperationsMemorySafety() {
    // Test 1: Move operations don't cause memory leaks or double frees
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2 = std::move(neuron1);  // Should not crash
    
    nlm::Hebbian hebbian1;
    nlm::Hebbian hebbian2 = std::move(hebbian1);  // Should not crash
    
    nlm::STDP stdp1;
    nlm::STDP stdp2 = std::move(stdp1);  // Should not crash
    
    nlm::AntiHebbianRule anti1;
    nlm::AntiHebbianRule anti2 = std::move(anti1);  // Should not crash
    
    std::cout << "    testMoveOperationsMemorySafety passed" << std::endl;
}

} // namespace test_memory_leaks