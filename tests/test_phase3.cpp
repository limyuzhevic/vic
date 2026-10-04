// Phase 3 test
#include <iostream>
#include "../src/experiments/Phase3Experiment.hpp"

int test_phase3() {
    std::cout << "Phase 3 Test: Testing world interaction capabilities..." << std::endl;
    
    // Test Phase 3 experiment initialization
    nlm::Phase3Experiment phase3;
    phase3.initialize(0.01f);
    
    std::cout << "Phase 3 experiment initialized with dt: " << phase3.getTimeStep() << std::endl;
    
    // Run a single timestep
    phase3.update(0.01f);
    
    std::cout << "Phase 3 timestep updated successfully" << std::endl;
    
    return 0;
}

int main() {
    return test_phase3();
}
