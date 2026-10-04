// Unit test for dynamics functionality
#include <iostream>
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"

int test_unit_dynamics() {
    std::cout << "Unit Dynamics Test: Testing neural dynamics..." << std::endl;
    
    // Test NeuralDynamics
    nlm::NeuralDynamics dynamics;
    dynamics.setTimeStep(0.001f);
    dynamics.setConductance(0.1f);
    
    std::cout << "NeuralDynamics configured with dt: " << dynamics.getTimeStep() << " and g: " << dynamics.getConductance() << std::endl;
    
    // Test SpikeSystem
    nlm::SpikeSystem spikesystem;
    spikesystem.setThreshold(-55.0f);
    spikesystem.setResetPotential(-70.0f);
    
    std::cout << "SpikeSystem configured with threshold: " << spikesystem.getThreshold() << " and reset: " << spikesystem.getResetPotential() << std::endl;
    
    return 0;
}

int main() {
    return test_unit_dynamics();
}
