// Phase 4 test
#include <iostream>
#include "../src/experiments/Phase4Experiment.hpp"

int main() {
    std::cout << "Phase 4 Test: Advanced Cognition" << std::endl;
    std::cout << "=" << std::string(30, '=') << std::endl;
    
    nlm::Phase4Experiment phase4;
    phase4.initialize(0.01f, 100);
    
    std::cout << "Memory capacity: " << phase4.getMemoryCapacity() << std::endl;
    std::cout << "Working memory size: " << phase4.getWorkingMemorySize() << std::endl;
    
    // Test operations
    for(int i = 0; i < 5; ++i) {
        phase4.update(0.01f);
    }
    
    std::cout << "Phase 4 test completed successfully!" << std::endl;
    
    return 0;
}
