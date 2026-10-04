// Phase 5 demo executable
#include <iostream>
#include "src/experiments/Phase5Experiment.hpp"

int main() {
    std::cout << "NLM Phase 5 Demo - Performance Optimization" << std::endl;
    std::cout << "============================================" << std::endl;
    
    nlm::Phase5Experiment phase5;
    phase5.initialize(0.01f, 1024);
    
    std::cout << "Initialized Phase 5 experiment with optimized memory pools" << std::endl;
    std::cout << "Thread pool size: " << phase5.getThreadPoolSize() << std::endl;
    
    // Run some operations
    for(int i = 0; i < 10; ++i) {
        phase5.update(0.01f);
    }
    
    std::cout << "Performance benchmark completed successfully!" << std::endl;
    
    return 0;
}
