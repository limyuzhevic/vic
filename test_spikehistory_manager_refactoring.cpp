// Test to verify SpikeHistoryManager refactoring
// This test verifies that the refactored Neuron and Synapse classes
// now use the shared SpikeHistoryManager to eliminate code duplication

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>

namespace test_spikehistory_manager {

void testNeuronSpikeHistoryManagement() {
    std::cout << "  Testing Neuron spike history management..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test that spike history starts empty
    assert(neuron.getSpikeHistory().empty());
    assert(neuron.getSpikeHistory().size() == 0);
    
    // Record spike events
    neuron.recordSpike(100.0);
    neuron.recordSpike(200.0);
    neuron.recordSpike(300.0);
    
    // Verify spike history contains the recorded events
    assert(neuron.getSpikeHistory().size() == 3);
    assert(neuron.getSpikeHistory()[0] == 100.0);
    assert(neuron.getSpikeHistory()[1] == 200.0);
    assert(neuron.getSpikeHistory()[2] == 300.0);
    
    // Test history clearing
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    ✓ Neuron spike history management working correctly" << std::endl;
}

void testSynapseSpikeHistoryManagement() {
    std::cout << "  Testing Synapse spike history management..." << std::endl;
    
    nlm::SynapseId synapseId(1);
    nlm::NeuronId sourceId(1);
    nlm::NeuronId destId(2);
    
    nlm::Synapse synapse(synapseId, sourceId, destId);
    
    // Test that pre-spike history starts empty
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    // Record pre and post spike events
    synapse.recordPreSpike(100.0);
    synapse.recordPreSpike(200.0);
    synapse.recordPostSpike(150.0);
    synapse.recordPostSpike(250.0);
    
    // Verify pre-spike history
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPreSpikeHistory()[0] == 100.0);
    assert(synapse.getPreSpikeHistory()[1] == 200.0);
    
    // Verify post-spike history
    assert(synapse.getPostSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory()[0] == 150.0);
    assert(synapse.getPostSpikeHistory()[1] == 250.0);
    
    // Test history clearing
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    ✓ Synapse spike history management working correctly" << std::endl;
}

void testSpikeHistoryManagerFunctionality() {
    std::cout << "  Testing SpikeHistoryManager functionality..." << std::endl;
    
    // Test the base functionality from SpikeHistoryManager
    nlm::SpikeHistoryManager history;
    
    // Test initial state
    assert(history.isEmpty());
    assert(history.getHistorySize() == 0);
    
    // Record spikes
    history.recordSpike(100.0);
    history.recordSpike(200.0);
    
    // Verify size
    assert(history.getHistorySize() == 2);
    
    // Verify access to history
    const auto& hist = history.getHistory();
    assert(hist.size() == 2);
    assert(hist[0] == 100.0);
    assert(hist[1] == 200.0);
    
    // Test clearing
    history.clearHistory();
    assert(history.isEmpty());
    assert(history.getHistorySize() == 0);
    
    std::cout << "    ✓ SpikeHistoryManager functionality working correctly" << std::endl;
}

void runAllTests() {
    std::cout << "Running SpikeHistoryManager refactoring tests..." << std::endl;
    
    testSpikeHistoryManagerFunctionality();
    testNeuronSpikeHistoryManagement();
    testSynapseSpikeHistoryManagement();
    
    std::cout << "\nAll SpikeHistoryManager refactoring tests passed! ✓" << std::endl;
}

} // namespace test_spikehistory_manager

int main() {
    test_spikehistory_manager::runAllTests();
    return 0;
}
