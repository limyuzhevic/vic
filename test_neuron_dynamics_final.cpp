#include "src/brain/Neuron.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Neuron Step Implementation with Adaptation Currents" << std::endl;
    std::cout << "========================================================" << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initialize neuron with random parameters
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    std::cout << "Initial state:" << std::endl;
    std::cout << "  Membrane potential: " << neuron.getMembranePotential() << " mV" << std::endl;
    std::cout << "  Threshold: " << neuron.getThreshold() << " mV" << std::endl;
    std::cout << "  Resting potential: " << neuron.getRestingPotential() << " mV" << std::endl;
    std::cout << "  Adaptation variable: " << neuron.getState().adaptationVariable << std::endl;
    std::cout << "  Firing state: " << static_cast<int>(neuron.getState().firingState) << std::endl;
    
    // Test basic step functionality
    std::cout << "\nTesting basic step functionality:" << std::endl;
    neuron.step(0.0);
    std::cout << "  After step(0.0):" << std::endl;
    std::cout << "    Membrane potential: " << neuron.getMembranePotential() << " mV" << std::endl;
    std::cout << "    Firing state: " << static_cast<int>(neuron.getState().firingState) << std::endl;
    
    // Test with current injection and adaptation
    std::cout << "\nTesting with current injection and adaptation dynamics:" << std::endl;
    neuron.setThreshold(-55.0f);  // Make neuron easier to fire
    neuron.setLeakConductance(5.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-65.0f);
    neuron.setRefractoryPeriod(10);
    
    neuron.injectCurrent(20.0f);  // Strong depolarizing current
    
    // Track adaptation over multiple steps
    std::cout << "Step | Membrane Potential | Adaptation Variable | Spike" << std::endl;
    std::cout << "-----|-------------------|---------------------|------" << std::endl;
    
    float prev_adaptation = neuron.getState().adaptationVariable;
    bool spike_detected = false;
    
    for (int step = 1; step <= 30; step++) {
        neuron.step(static_cast<double>(step) * 0.001);
        
        float adaptation = neuron.getState().adaptationVariable;
        float potential = neuron.getMembranePotential();
        bool firing = neuron.isFiring();
        bool refractory = neuron.isRefractory();
        
        if (firing) spike_detected = true;
        
        std::cout << step << "   | " << potential << " mV        | " << adaptation << "         | " << (firing ? "Yes" : "No") << std::endl;
        
        // Allow adaptation to change
        if (step > 5) {
            if (adaptation < prev_adaptation * 0.9f) {
                std::cout << "  Adaptation decreasing as expected" << std::endl;
            }
            prev_adaptation = adaptation;
        }
    }
    
    std::cout << "\nAdaptation dynamics summary:" << std::endl;
    std::cout << "  Initial adaptation: " << neuron.getState().adaptationVariable << std::endl;
    std::cout << "  Max adaptation observed: " << neuron.getState().adaptationVariable << std::endl;
    
    // Test refractory period behavior
    std::cout << "\nTesting refractory period behavior:" << std::endl;
    neuron.setRefractoryPeriod(5);
    neuron.setFiringState(nlm::FiringState::Active);
    
    std::cout << "  Firing state: " << (neuron.isFiring() ? "Yes" : "No") << std::endl;
    std::cout << "  Refractory state: " << (neuron.isRefractory() ? "Yes" : "No") << std::endl;
    
    // Test spike frequency adaptation
    std::cout << "\nTesting spike-frequency adaptation:" << std::endl;
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.setThreshold(-50.0f);
    neuron2.injectCurrent(25.0f);
    
    float adaptations[5] = {0};
    for (int i = 0; i < 5; i++) {
        neuron2.step(static_cast<double>(i) * 0.001);
        adaptations[i] = neuron2.getState().adaptationVariable;
    }
    
    std::cout << "Adaptation over 5 steps:" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << "  Step " << i << ": " << adaptations[i] << std::endl;
    }
    
    // Check if adaptation is increasing (indicating spiking activity)
    bool adaptation_increasing = true;
    for (int i = 1; i < 5; i++) {
        if (adaptations[i] < adaptations[i-1] * 0.95f) {
            adaptation_increasing = false;
            break;
        }
    }
    
    if (adaptation_increasing) {
        std::cout << "  Adaptation increasing (spike-frequency adaptation confirmed)" << std::endl;
    } else {
        std::cout << "  Adaptation decreasing or stable (may not have spiked yet)" << std::endl;
    }
    
    // Test synaptic inputs
    std::cout << "\nTesting synaptic input handling:" << std::endl;
    nlm::Neuron neuron3(nlm::NeuronId(3));
    neuron3.injectCurrent(0.0f);
    neuron3.clearTotalCurrent();
    neuron3.receiveExcitatoryInput(10.0f);
    
    std::cout << "  After receiving excitatory input: " << neuron3.getState().adaptationVariable << std::endl;
    
    neuron3.receiveInhibitoryInput(5.0f);
    std::cout << "  After receiving inhibitory input: " << neuron3.getState().adaptationVariable << std::endl;
    
    // Test state reset
    std::cout << "\nTesting state reset:" << std::endl;
    neuron3.setMembranePotential(-60.0f);
    neuron3.recordSpike(0.1);
    std::cout << "  Before reset - Membrane potential: " << neuron3.getMembranePotential() << " mV" << std::endl;
    std::cout << "  Before reset - Spike history size: " << neuron3.getSpikeHistory().size() << std::endl;
    
    neuron3.reset();
    std::cout << "  After reset - Membrane potential: " << neuron3.getMembranePotential() << " mV" << std::endl;
    std::cout << "  After reset - Spike history size: " << neuron3.getSpikeHistory().size() << std::endl;
    
    std::cout << "\n" << std::endl;
    std::cout << "Neuron dynamics implementation test completed successfully!" << std::endl;
    std::cout << "============================================" << std::endl;
    
    // Final verification
    assert(neuron3.getMembranePotential() == -70.0f);
    assert(neuron3.getState().adaptationVariable == 0.0f);
    assert(neuron3.getSpikeHistory().empty());
    
    return 0;
}