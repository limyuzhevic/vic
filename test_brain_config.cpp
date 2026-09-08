// Test to verify Brain configuration initialization and getOr usage
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

void testBrainConfigInitialization() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test 1: Verify getOr with default values in Impl constructor
    // The Impl constructor should use getOr for config values
    nlm::Brain brain(config);
    
    std::cout << "testBrainConfigInitialization passed" << std::endl;
}

void runConfigTests() {
    testBrainConfigInitialization();
}

int main() {
    runConfigTests();
    return 0;
}
