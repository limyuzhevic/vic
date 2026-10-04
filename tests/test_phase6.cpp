// Phase 6 test
#include <iostream>
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

int main() {
    std::cout << "Phase 6 Test: Final Integration" << std::endl;
    std::cout << "=" << std::string(25, '=') << std::endl;
    
    nlm::Phase6IntegratedExperiment phase6;
    phase6.initialize(0.01f, 100, 1024);
    
    std::cout << "Neurons: " << phase6.getNumNeurons() << std::endl;
    std::cout << "Connections: " << phase6.getNumConnections() << std::endl;
    std::cout << "Thread pool: " << phase6.getThreadPoolSize() << std::endl;
    
    // Test integration
    for(int i = 0; i < 10; ++i) {
        phase6.update(0.01f);
    }
    
    std::cout << "Phase 6 integration test completed successfully!" << std::endl;
    
    return 0;
}
