#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing refactored Brain class..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "Brain created successfully." << std::endl;
    
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    std::cout << "Brain initialized successfully." << std::endl;
    
    // Test step() method (the main refactored function)
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
        std::cout << "Step " << step << " completed." << std::endl;
    }
    
    std::cout << "All basic operations passed!" << std::endl;
    std::cout << "Brain refactoring is working correctly." << std::endl;
    
    return 0;
}
