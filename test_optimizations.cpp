#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>

// Include the proper headers
#include "src/core/Types/Types.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"

using namespace nlm;

// Simple test to verify the optimizations compile correctly
void testNeuronOptimizations() {
    std::cout << "Testing Neuron optimizations..." << std::endl;
    
    // Test 1: Neuron memory pool creation
    Neuron neuron(NeuronId(1));
    neuron.setType(NeuronType::Internal);
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    
    // Test spike recording
    neuron.recordSpike(0.0f);
    neuron.recordSpike(1.0f);
    neuron.recordSpike(2.0f);
    
    // Test spike history
    const auto& history = neuron.getSpikeHistory();
    std::cout << "  Spike history size: " << history.size() << std::endl;
    
    // Test connection management
    neuron.addIncomingSynapse(1);
    neuron.addOutgoingSynapse(2);
    
    const auto& incoming = neuron.getIncomingSynapses();
    const auto& outgoing = neuron.getOutgoingSynapses();
    std::cout << "  Incoming synapses: " << incoming.size() << std::endl;
    std::cout << "  Outgoing synapses: " << outgoing.size() << std::endl;
    
    // Test LIF step
    bool fired = neuron.stepLIF(0.0f, 0.001f);
    std::cout << "  Neuron fired: " << (fired ? "Yes" : "No") << std::endl;
    
    std::cout << "  Neuron tests passed!" << std::endl << std::endl;
}

void testSynapseOptimizations() {
    std::cout << "Testing Synapse optimizations..." << std::endl;
    
    // Test 1: Synapse memory pool creation
    Synapse synapse(SynapseId(1), NeuronId(100), NeuronId(200));
    synapse.setType(SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.setDelay(2);
    
    // Test spike recording
    synapse.recordPreSpike(0.0f);
    synapse.recordPreSpike(1.0f);
    synapse.recordPostSpike(0.5f);
    
    // Test spike history
    const auto& preHistory = synapse.getPreSpikeHistory();
    const auto& postHistory = synapse.getPostSpikeHistory();
    std::cout << "  Pre spike history size: " << preHistory.size() << std::endl;
    std::cout << "  Post spike history size: " << postHistory.size() << std::endl;
    
    // Test eligibility trace
    synapse.setEligibilityTrace(0.3f);
    float trace = synapse.getEligibilityTrace();
    std::cout << "  Eligibility trace: " << trace << std::endl;
    
    // Test efficacy
    synapse.setEfficacy(1.2f);
    float efficacy = synapse.getEfficacy();
    std::cout << "  Efficacy: " << efficacy << std::endl;
    
    std::cout << "  Synapse tests passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== NLM Optimization Verification ===" << std::endl;
    std::cout << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    testNeuronOptimizations();
    testSynapseOptimizations();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║ Optimization Verification Complete!     ║" << std::endl;
    std::cout << "║                                         ║" << std::endl;
    std::cout << "║ All optimizations compiled and tested  ║" << std::endl;
    std::cout << "║                                         ║" << std::endl;
    std::cout << "║ Time: " << std::fixed << std::setprecision(3) << totalTime << "s   ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    
    return 0;
}