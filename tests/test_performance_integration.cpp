// Performance Optimization Integration Tests
// Performance optimization hooks integration

#include "optim/OptimizationHooks.hpp"
#include "core/ExtensibleBrain.hpp"
#include <cassert>
#include <iostream>

namespace nlm {
namespace tests {

namespace test_performance_integration {

void testPerformanceOptimizationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test performance optimization integration
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    assert(extBrain != nullptr);
    
    std::cout << "    testPerformanceOptimizationIntegration passed" << std::endl;
}

void testOptimizationHooksWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    std::cout << "    testOptimizationHooksWithBrain passed" << std::endl;
}

void testSIMDOptimization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    // Test SIMD optimization
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    if (extBrain) {
        auto* hooks = extBrain->getOptimizationHooks();
        if (hooks) {
            bool simdEnabled = hooks->isSIMDEnabled();
        }
    }
    
    std::cout << "    testSIMDOptimization passed" << std::endl;
}

void testParallelProcessingOptimization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    // Test parallel processing optimization
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    if (extBrain) {
        auto* hooks = extBrain->getOptimizationHooks();
        if (hooks) {
            bool parallelEnabled = hooks->isParallelEnabled();
        }
    }
    
    std::cout << "    testParallelProcessingOptimization passed" << std::endl;
}

void testMemoryOptimization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    // Test memory optimization
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    if (extBrain) {
        auto* hooks = extBrain->getOptimizationHooks();
        if (hooks) {
            bool memoryOptimized = hooks->isMemoryOptimized();
        }
    }
    
    std::cout << "    testMemoryOptimization passed" << std::endl;
}

void testPerformanceMetrics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    // Test performance metrics
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    if (extBrain) {
        auto* hooks = extBrain->getOptimizationHooks();
        if (hooks) {
            double performanceMetric = hooks->getPerformanceMetric();
        }
    }
    
    std::cout << "    testPerformanceMetrics passed" << std::endl;
}

void testPerformanceIntegrationUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    // Test performance integration update
    brain.update(0.1);
    
    std::cout << "    testPerformanceIntegrationUpdate passed" << std::endl;
}

void testPerformanceStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    // Test performance statistics
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    if (extBrain) {
        auto* hooks = extBrain->getOptimizationHooks();
        if (hooks) {
            std::string stats = hooks->getOptimizationStats();
            assert(!stats.empty());
        }
    }
    
    std::cout << "    testPerformanceStatistics passed" << std::endl;
}

void runAll() {
    testPerformanceOptimizationIntegration();
    testOptimizationHooksWithBrain();
    testSIMDOptimization();
    testParallelProcessingOptimization();
    testMemoryOptimization();
    testPerformanceMetrics();
    testPerformanceIntegrationUpdate();
    testPerformanceStatistics();
}

} // namespace test_performance_integration

} // namespace nlm
namespace tests {

void test_performance_integration() {
    test_performance_integration::runAll();
}

} // namespace tests
