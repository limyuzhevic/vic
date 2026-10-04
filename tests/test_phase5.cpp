// Phase 5 test
#include <iostream>
#include "../src/experiments/Phase5Experiment.hpp"

int test_phase5() {
    std::cout << "Phase 5 Test: Testing performance optimizations..." << std::endl;
    
    // Test Phase 5 experiment initialization
    nlm::Phase5Experiment phase5;
    phase5.initialize(0.01f, 1024);
    
    std::cout << "Phase 5 experiment initialized with dt: " << phase5.getTimeStep() << " and thread pool size: " << phase5.getThreadPoolSize() << std::endl;
    
    // Run a single timestep
    phase5.update(0.01f);
    
    std::cout << "Phase 5 timestep updated successfully" << std::endl;
    
    return 0;
}

int main() {
    return test_phase5();
}
