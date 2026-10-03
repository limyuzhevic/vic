// Quick test to verify SpikeHistoryManager refactoring
// Tests that Neuron and Synapse now use shared spike history management

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>

int main() {
    std::cout << "Testing SpikeHistoryManager refactoring..." << std::endl;
    
    // Test 1: Neuron spike history management
    nlm::NeuronId neuronId(1);
    nlm::Neuron neuron(neuronId);
    
    // Record some spike events
    neuron.recordSpike(100.0);
    neuron.recordSpike(200.0);
    neuron.recordSpike(300.0);
    
    // Verify spike history
    const auto& neuronHistory = neuron.getSpikeHistory();
    std::cout << "Neuron spike history size: " << neuronHistory.size() << std::endl;
    
    if (neuronHistory.size() == 3) {
        std::cout << "✓ Neuron spike history management working correctly" << std::endl;
    } else {
        std::cout << "✗ Neuron spike history management failed" << std::endl;
        return 1;
    }
    
    // Test 2: Synapse spike history management
    nlm::SynapseId synapseId(1);
    nlm::NeuronId sourceId(1);
    nlm::NeuronId destId(2);
    
    nlm::Synapse synapse(synapseId, sourceId, destId);
    
    // Record pre and post spike events
    synapse.recordPreSpike(100.0);
    synapse.recordPreSpike(200.0);
    synapse.recordPostSpike(150.0);
    synapse.recordPostSpike(250.0);
    
    // Verify spike history
    const auto& preHistory = synapse.getPreSpikeHistory();
    const auto& postHistory = synapse.getPostSpikeHistory();
    
    std::cout << "Synapse pre-spike history size: " << preHistory.size() << std::endl;
    std::cout << "Synapse post-spike history size: " << postHistory.size() << std::endl;
    
    if (preHistory.size() == 2 && postHistory.size() == 2) {
        std::cout << "✓ Synapse spike history management working correctly" << std::endl;
    } else {
        std::cout << "✗ Synapse spike history management failed" << std::endl;
        return 1;
    }
    
    // Test 3: Clear history
    synapse.clearHistory();
    
    if (synapse.getPreSpikeHistory().empty() && synapse.getPostSpikeHistory().empty()) {
        std::cout << "✓ History clearing working correctly" << std::endl;
    } else {
        std::cout << "✗ History clearing failed" << std::endl;
        return 1;
    }
    
    std::cout << "\nAll SpikeHistoryManager refactoring tests passed!" << std::endl;
    return 0;
}
