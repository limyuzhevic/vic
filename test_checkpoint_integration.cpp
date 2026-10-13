#include "Brain.hpp"
#include "CheckpointSystem.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "Testing CheckpointSystem Integration" << std::endl;
    
    // Create a simple config (this would normally come from actual config system)
    class MockConfig {
    public:
        template<typename T>
        std::optional<T> get(const std::string& key) const { return std::nullopt; }
        
        template<typename T>
        T getOr(const std::string& key, const T& defaultValue) const { return defaultValue; }
    };
    
    auto config = std::make_shared<MockConfig>();
    
    // Create brain with checkpoint system
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Create some neurons in the brain
    auto region1 = brain.addRegion("TestRegion");
    auto region2 = brain.addRegion("TestRegion2");
    
    // Simulate some steps
    for (size_t step = 1; step <= 10; ++step) {
        brain.step(step);
    }
    
    // Test save functionality
    std::cout << "Testing save functionality..." << std::endl;
    bool saveResult = brain.save("test_checkpoint");
    if (!saveResult) {
        std::cerr << "Failed to save checkpoint" << std::endl;
        return 1;
    }
    
    // Create a new brain instance and test load
    nlm::Brain brain2(config);
    if (!brain2.initialize()) {
        std::cerr << "Failed to initialize brain for loading" << std::endl;
        return 1;
    }
    
    std::cout << "Testing load functionality..." << std::endl;
    bool loadResult = brain2.load("test_checkpoint");
    if (!loadResult) {
        std::cerr << "Failed to load checkpoint" << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed! CheckpointSystem integration is working correctly." << std::endl;
    return 0;
}
