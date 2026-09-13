#include <iostream>
#include "brain/Neuron.hpp"

int main() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test 1: Basic neuron creation
    std::cout << "Test 1: Neuron ID: " << neuron.getId().index() << std::endl;
    assert(neuron.getId() == nlm::NeuronId(1));
    
    // Test 2: Set and get membrane potential
    neuron.setMembranePotential(-60.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    // Test 3: Threshold
    neuron.setThreshold(-50.0f);
    assert(neuron.getThreshold() == -50.0f);
    
    // Test 4: Resting and reset potentials
    neuron.setRestingPotential(-70.0f);
    assert(neuron.getRestingPotential() == -70.0f);
    
    neuron.setResetPotential(-70.0f);
    assert(neuron.getResetPotential() == -70.0f);
    
    // Test 5: Current injection
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    // Test 6: Clear current
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test 7: Excitatory and inhibitory inputs
    neuron.receiveExcitatoryInput(3.0f);
    assert(neuron.getTotalCurrent() == 3.0f);
    
    neuron.receiveInhibitoryInput(2.0f);
    assert(neuron.getTotalCurrent() == 1.0f);
    
    // Test 8: Spike history
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    assert(neuron.getSpikeHistory().size() == 2);
    assert(neuron.getSpikeHistory()[0] == 0.1);
    assert(neuron.getSpikeHistory()[1] == 0.2);
    
    // Test 9: Clear spike history
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    // Test 10: LIF step without firing
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(!fired);
    
    // Test 11: LIF step that should fire
    neuron.setMembranePotential(-40.0f);  // Above threshold
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    
    fired = neuron.stepLIF(0.0, 0.001);
    assert(fired);
    assert(neuron.getMembranePotential() == -70.0f); // Should be reset
    assert(!neuron.isFiring()); // Should be refractory
    assert(neuron.isRefractory()); // Should be refractory
    
    // Test 12: Refractory period
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
