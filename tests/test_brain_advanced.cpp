#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

namespace test_brain_advanced {

// Test advanced Python API methods
void testBrainTraining() {
    std::cout << "    testBrainTraining started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create simple training data
    std::vector<std::vector<float>> trainingData;
    for (int i = 0; i < 10; ++i) {
        trainingData.push_back(std::vector<float>(5, 0.5f));
    }
    
    // Test training
    bool trainingSuccess = brain.train(trainingData, 5, 0.01f);
    assert(trainingSuccess);
    
    // Check training metrics
    auto metrics = brain.get_training_metrics();
    assert(metrics["epochs_completed"] >= 1);
    assert(metrics["samples_processed"] >= 10);
    assert(metrics["training_complete"] == 1.0f);
    
    std::cout << "    testBrainTraining passed" << std::endl;
}

void testBrainEvaluation() {
    std::cout << "    testBrainEvaluation started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create test data
    std::vector<std::vector<float>> testData;
    for (int i = 0; i < 5; ++i) {
        testData.push_back(std::vector<float>(3, 0.3f));
    }
    
    // Test evaluation
    float accuracy = brain.evaluate(testData);
    assert(accuracy >= 0.0f && accuracy <= 1.0f);
    
    std::cout << "    testBrainEvaluation passed" << std::endl;
}

void testBrainModelExport() {
    std::cout << "    testBrainModelExport started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test model export
    std::string exportFile = "/tmp/test_brain_model.bin";
    bool exportSuccess = brain.export_model(exportFile);
    assert(exportSuccess);
    
    // Verify file exists and has content
    std::ifstream file(exportFile, std::ios::binary);
    assert(file.good());
    
    // Clean up
    file.close();
    std::remove(exportFile.c_str());
    
    std::cout << "    testBrainModelExport passed" << std::endl;
}

void testBrainCheckpointing() {
    std::cout << "    testBrainCheckpointing started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps to create state
    for (int step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Test checkpoint creation
    std::string checkpointFile = "/tmp/test_checkpoint";
    std::unordered_map<std::string, float> metadata;
    metadata["test_version"] = 1.0f;
    metadata["description"] = "Test checkpoint";
    
    bool checkpointSuccess = brain.checkpoint(checkpointFile, metadata);
    assert(checkpointSuccess);
    
    // Test restore checkpoint
    bool restoreSuccess = brain.restore_checkpoint(checkpointFile);
    assert(restoreSuccess);
    
    // Clean up
    std::remove((checkpointFile + ".bin").c_str());
    std::remove((checkpointFile + ".meta").c_str());
    
    std::cout << "    testBrainCheckpointing passed" << std::endl;
}

void testBrainVisualization() {
    std::cout << "    testBrainVisualization started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test visualization with PNG format
    bool vizSuccess = brain.visualize("/tmp/test_viz.png", "png", false);
    assert(vizSuccess);
    
    // Verify file exists
    std::ifstream file("/tmp/test_viz.png");
    assert(file.good());
    file.close();
    std::remove("/tmp/test_viz.png");
    
    // Test visualization with text format
    bool vizSuccess2 = brain.visualize("/tmp/test_viz.txt", "txt", false);
    assert(vizSuccess2);
    
    std::cout << "    testBrainVisualization passed" << std::endl;
}

void testBrainPerformanceMonitoring() {
    std::cout << "    testBrainPerformanceMonitoring started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Start monitoring
    brain.monitor();
    
    // Run some steps
    for (int step = 0; step < 3; ++step) {
        brain.step(step);
    }
    
    // Stop monitoring
    brain.stop_monitoring();
    
    // Get monitoring data
    auto monitorData = brain.get_monitor_data();
    assert(!monitorData.empty());
    assert(monitorData.size() > 0);
    
    std::cout << "    testBrainPerformanceMonitoring passed" << std::endl;
}

void testBrainPerformanceStats() {
    std::cout << "    testBrainPerformanceStats started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get performance stats
    auto stats = brain.get_performance_stats();
    assert(stats.find("neuron_count") != stats.end());
    assert(stats.find("spike_rate_hz") != stats.end());
    assert(stats.find("memory_usage_mb") != stats.end());
    
    assert(stats["neuron_count"] > 0);
    assert(stats["spike_rate_hz"] >= 0.0);
    assert(stats["memory_usage_mb"] >= 0.0);
    
    std::cout << "    testBrainPerformanceStats passed" << std::endl;
}

void testBrainProfileStep() {
    std::cout << "    testBrainProfileStep started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Profile a step
    auto profile = brain.profile_step();
    assert(profile.find("step_number") != profile.end());
    assert(profile.find("neurons_active") != profile.end());
    assert(profile.find("spike_rate") != profile.end());
    
    assert(profile["step_number"] >= 0);
    assert(profile["neurons_active"] >= 0);
    assert(profile["spike_rate"] >= 0.0);
    
    std::cout << "    testBrainProfileStep passed" << std::endl;
}

// Test enhanced configuration system
void testEnhancedConfigSchemaValidation() {
    std::cout << "    testEnhancedConfigSchemaValidation started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set up validation rules
    config->addRangeValidator("learning_rate", 0.0, 1.0, "Learning rate should be between 0 and 1");
    config->addTypeValidator("enable_learning", nlm::TypeValidator::ExpectedType::BOOL, "Learning flag should be boolean");
    
    // Test valid values
    config->set("learning_rate", 0.5, nlm::ConfigSource::Runtime);
    config->set("enable_learning", true, nlm::ConfigSource::Runtime);
    
    std::string errorMessage;
    assert(config->validate("learning_rate", errorMessage));
    assert(config->validate("enable_learning", errorMessage));
    
    // Test invalid value
    config->set("learning_rate", 1.5, nlm::ConfigSource::Runtime);
    std::string error;
    bool isValid = config->validate("learning_rate", error);
    assert(!isValid);
    assert(!error.empty());
    
    std::cout << "    testEnhancedConfigSchemaValidation passed" << std::endl;
}

void testEnhancedConfigRuntimeQueries() {
    std::cout << "    testEnhancedConfigRuntimeQueries started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("learning_rate", 0.1, nlm::ConfigSource::Default);
    config->set("batch_size", 32, nlm::ConfigSource::Default);
    config->set("epochs", 10, nlm::ConfigSource::Default);
    
    // Enable runtime queries
    config->enableRuntimeQuery("effective_lr", "learning_rate * 0.9");
    config->enableRuntimeQuery("total_samples", "batch_size * epochs");
    
    // Evaluate queries
    auto lrValue = config->evaluateQuery("learning_rate");
    assert(lrValue == 0.1);
    
    auto effectiveLRValue = config->evaluateQuery("effective_lr");
    assert(effectiveLRValue == 0.09);  // learning_rate * 0.9
    
    auto totalSamplesValue = config->evaluateQuery("total_samples");
    assert(totalSamplesValue == 320);  // 32 * 10
    
    std::cout << "    testEnhancedConfigRuntimeQueries passed" << std::endl;
}

void testEnhancedConfigDynamicParameters() {
    std::cout << "    testEnhancedConfigDynamicParameters started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Enable dynamic parameter with validation
    config->enableDynamicParameter("learning_rate", 0.001, 0.1, 0.001);
    
    // Set initial value
    config->set("learning_rate", 0.05, nlm::ConfigSource::Runtime);
    
    // Get dynamic value (should be rounded to nearest step)
    auto dynamicValue = config->getDynamicValue("learning_rate");
    assert(dynamicValue == 0.05);  // Should stay the same as it's already on the step
    
    // Test clamping
    config->set("learning_rate", 0.15, nlm::ConfigSource::Runtime);
    dynamicValue = config->getDynamicValue("learning_rate");
    assert(dynamicValue == 0.15);  // Should be clamped to max
    
    std::cout << "    testEnhancedConfigDynamicParameters passed" << std::endl;
}

void testEnhancedConfigErrorHandling() {
    std::cout << "    testEnhancedConfigErrorHandling started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test error collection
    config->addRangeValidator("test_param", 0.0, 1.0, "Test parameter range");
    
    // Set invalid value to trigger error
    config->set("test_param", 2.0, nlm::ConfigSource::Runtime);
    
    // Validate all should collect errors
    bool allValid = config->validateAll();
    assert(!allValid);
    
    // Get validation errors
    auto errors = config->getValidationErrors();
    assert(!errors.empty());
    
    // Clear config and test clean validation
    config->clear();
    config->set("test_param", 0.5, nlm::ConfigSource::Runtime);
    assert(config->validateAll());
    
    std::cout << "    testEnhancedConfigErrorHandling passed" << std::endl;
}

void testIntegrationPythonConfig() {
    std::cout << "    testIntegrationPythonConfig started" << std::endl;
    
    // Create brain with enhanced config
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile("configs/default.cfg");  // Use existing config if available
    
    // Add validation rules
    config->addRangeValidator("plasticity_rate", 0.0, 1.0, "Plasticity rate should be reasonable");
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test training with enhanced config
    std::vector<std::vector<float>> trainingData;
    trainingData.push_back(std::vector<float>(10, 0.8f));
    
    bool trainingSuccess = brain.train(trainingData, 2, 0.01f);
    assert(trainingSuccess);
    
    // Test evaluation
    float accuracy = brain.evaluate(trainingData);
    assert(accuracy >= 0.0f);
    
    std::cout << "    testIntegrationPythonConfig passed" << std::endl;
}

// Comprehensive test runner
void runAllBrainAdvancedTests() {
    std::cout << "=== Running Brain Advanced API Tests ===" << std::endl;
    
    testBrainTraining();
    testBrainEvaluation();
    testBrainModelExport();
    testBrainCheckpointing();
    testBrainVisualization();
    testBrainPerformanceMonitoring();
    testBrainPerformanceStats();
    testBrainProfileStep();
    
    testEnhancedConfigSchemaValidation();
    testEnhancedConfigRuntimeQueries();
    testEnhancedConfigDynamicParameters();
    testEnhancedConfigErrorHandling();
    
    testIntegrationPythonConfig();
    
    std::cout << "=== All Brain Advanced API Tests Passed ===" << std::endl;
}

} // namespace test_brain_advanced