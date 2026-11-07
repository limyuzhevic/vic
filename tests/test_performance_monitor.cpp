// Performance Monitor Tests
#include <pybind11/pybind11.h>
#include <cassert>
#include <iostream>
#include <vector>

namespace test_performance {

void testPerformanceMonitorBasic() {
    nlm::Config config;
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create performance monitor
    auto monitor = std::make_shared<nlm::PerformanceMonitor>(brain);
    
    // Test basic functionality
    assert(monitor->getTotalSteps() == 0);
    assert(monitor->getTotalSpikes() == 0);
    assert(monitor->getStepsPerformed() == 0);
    
    // Record some steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        monitor->recordStep(step);
    }
    
    assert(monitor->getTotalSteps() == 10);
    assert(monitor->getStepsPerformed() == 10);
    
    std::string summary = monitor->getSummary();
    assert(summary.find("Performance Summary:") != std::string::npos);
    assert(summary.find("Total steps: 10") != std::string::npos);
    
    std::cout << "    testPerformanceMonitorBasic passed" << std::endl;
}

void testPerformanceMonitorStepsPerSecond() {
    nlm::Config config;
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto monitor = std::make_shared<nlm::PerformanceMonitor>(brain);
    
    // Start monitoring
    monitor->start();
    
    // Perform some work
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        monitor->recordStep(step);
    }
    
    // Stop monitoring and get elapsed time
    double elapsed = monitor->stop();
    assert(elapsed >= 0.0);
    
    // Calculate performance metrics
    double steps_per_second = monitor->getStepsPerSecond(elapsed);
    double spikes_per_second = monitor->getSpikesPerSecond(elapsed);
    
    assert(steps_per_second >= 0.0);
    assert(spikes_per_second >= 0.0);
    
    std::cout << "    testPerformanceMonitorStepsPerSecond passed" << std::endl;
}

void runAll() {
    testPerformanceMonitorBasic();
    testPerformanceMonitorStepsPerSecond();
}

} // namespace test_performance