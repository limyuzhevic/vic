#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing sleep/rest functionality..." << std::endl;
    
    // Create brain with configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Enable sleep functionality
    config->set("sleep_enabled", true, nlm::ConfigSource::Default);
    config->set("sleep_duration_steps", 100, nlm::ConfigSource::Default);
    config->set("sleep_consolidation_threshold", 0.3f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    std::cout << "Brain initialization: PASSED" << std::endl;
    
    // Test that brain is not sleeping initially
    assert(!brain.isSleeping());
    std::cout << "Initial state (not sleeping): PASSED" << std::endl;
    
    // Test enabling sleep
    brain.enableSleep(true);
    assert(brain.isSleeping());
    std::cout << "Enable sleep: PASSED" << std::endl;
    
    // Test waking brain
    brain.wake();
    assert(!brain.isSleeping());
    std::cout << "Wake brain: PASSED" << std::endl;
    
    // Test disabling sleep
    brain.enableSleep(false);
    assert(!brain.isSleeping());
    std::cout << "Disable sleep: PASSED" << std::endl;
    
    // Test development method exists
    brain.develop();
    std::cout << "Development: PASSED" << std::endl;
    
    std::cout << "All sleep/rest functionality tests PASSED!" << std::endl;
    return 0;
}