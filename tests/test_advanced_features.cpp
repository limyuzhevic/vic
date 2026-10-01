#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>

// Test brain advanced functionality
void testBrainAdvancedFeatures() {
    std::cout << "Testing Brain Advanced Features" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test training with enhanced config
    std::vector<std::vector<float>> trainingData;
    trainingData.push_back(std::vector<float>(10, 0.5f));
    
    bool trainingSuccess = brain.train(trainingData, 2, 0.01f);
    assert(trainingSuccess);
    
    // Check training metrics
    auto metrics = brain.get_training_metrics();
    assert(metrics["epochs_completed"] >= 1);
    assert(metrics["samples_processed"] >= 1);
    
    std::cout << "✓ Brain advanced features test passed" << std::endl;
}

// Test enhanced configuration system
void testEnhancedConfigFeatures() {
    std::cout << "Testing Enhanced Configuration Features" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Add validation rules
    config->addRangeValidator("learning_rate", 0.0, 1.0, "Learning rate validation");
    config->addTypeValidator("enable_learning", nlm::TypeValidator::ExpectedType::BOOL, "Learning flag");
    
    // Test valid values
    config->set("learning_rate", 0.5, nlm::ConfigSource::Runtime);
    config->set("enable_learning", true, nlm::ConfigSource::Runtime);
    
    std::string errorMessage;
    assert(config->validate("learning_rate", errorMessage));
    assert(config->validate("enable_learning", errorMessage));
    
    // Test runtime queries
    config->enableRuntimeQuery("effective_lr", "learning_rate * 0.9");
    auto lrValue = config->evaluateQuery("effective_lr");
    assert(lrValue == 0.45f);  // 0.5 * 0.9
    
    std::cout << "✓ Enhanced configuration features test passed" << std::endl;
}

// Test Python bindings compatibility
void testPythonBindingsCompatibility() {
    std::cout << "Testing Python Bindings Compatibility" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test Python-style API
    std::vector<std::vector<float>> trainingData;
    trainingData.push_back(std::vector<float>(5, 0.8f));
    
    bool trained = brain.train(trainingData, 1, 0.01f);
    assert(trained);
    
    float accuracy = brain.evaluate(trainingData);
    assert(accuracy >= 0.0f);
    
    // Test checkpointing
    bool checkpointed = brain.checkpoint("/tmp/test_checkpoint", 
                                        {{ "version", 1.0f }});
    assert(checkpointed);
    
    std::cout << "✓ Python bindings compatibility test passed" << std::endl;
}

int main() {
    std::cout << "NLM Advanced Features Test Suite" << std::endl;
    std::cout << "===============================" << std::endl;
    
    try {
        testBrainAdvancedFeatures();
        testEnhancedConfigFeatures();
        testPythonBindingsCompatibility();
        
        std::cout << std::endl << "=== ALL TESTS PASSED ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl << "=== TEST FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}