// Test file for enhanced Python bindings
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_python_bindings_advanced {

// Test Python-style API for training and evaluation
void testPythonTrainingAPI() {
    std::cout << "    testPythonTrainingAPI started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Simulate Python-style training API
    std::vector<std::vector<float>> training_data;
    training_data.reserve(100);
    for (int i = 0; i < 100; ++i) {
        std::vector<float> sample(50, 0.5f);  // 50 features
        training_data.push_back(sample);
    }
    
    // Test the train method (equivalent to pynlm.train_network())
    bool training_result = brain.train(training_data, 10, 0.01f);
    assert(training_result);
    
    // Check metrics like Python would
    auto metrics = brain.get_training_metrics();
    assert(metrics["samples_processed"] == 100);
    assert(metrics["epochs_completed"] == 10);
    assert(metrics["best_accuracy"] >= 0.0f);
    
    std::cout << "    testPythonTrainingAPI passed" << std::endl;
}

void testPythonEvaluationAPI() {
    std::cout << "    testPythonEvaluationAPI started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Simulate test data in Python format
    std::vector<std::vector<float>> test_data;
    test_data.reserve(50);
    for (int i = 0; i < 50; ++i) {
        std::vector<float> sample(20, 0.3f);
        test_data.push_back(sample);
    }
    
    // Test evaluation API
    float accuracy = brain.evaluate(test_data);
    assert(accuracy >= 0.0f && accuracy <= 1.0f);
    
    // In Python API, you'd typically get a dict of metrics
    auto metrics = brain.get_training_metrics();
    assert(metrics.find("neuron_count") != metrics.end());
    
    std::cout << "    testPythonEvaluationAPI passed" << std::endl;
}

void testPythonCheckpointAPI() {
    std::cout << "    testPythonCheckpointAPI started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Simulate Python checkpoint API
    std::string checkpoint_path = "/tmp/test_model_checkpoint";
    
    // Create checkpoint with metadata (Python-style dict)
    std::unordered_map<std::string, float> metadata;
    metadata["version"] = 1.0f;
    metadata["accuracy"] = 0.85f;
    metadata["loss"] = 0.15f;
    metadata["epoch"] = 50.0f;
    
    bool checkpoint_result = brain.checkpoint(checkpoint_path, metadata);
    assert(checkpoint_result);
    
    // Simulate loading checkpoint
    bool load_result = brain.restore_checkpoint(checkpoint_path);
    assert(load_result);
    
    // Clean up
    std::remove((checkpoint_path + ".bin").c_str());
    std::remove((checkpoint_path + ".meta").c_str());
    
    std::cout << "    testPythonCheckpointAPI passed" << std::endl;
}

void testPythonVisualizationAPI() {
    std::cout << "    testPythonVisualizationAPI started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test visualization with different formats (Python-style)
    bool png_result = brain.visualize("/tmp/brain_viz.png", "png", false);
    assert(png_result);
    
    // Test with animation (Python-style)
    bool animated_result = brain.visualize("/tmp/brain_animated.gif", "gif", true);
    assert(animated_result);
    
    // Clean up
    std::remove("/tmp/brain_viz.png");
    std::remove("/tmp/brain_animated.gif");
    
    std::cout << "    testPythonVisualizationAPI passed" << std::endl;
}

void testPythonPerformanceMonitoringAPI() {
    std::cout << "    testPythonPerformanceMonitoringAPI started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Start monitoring (Python-style context manager equivalent)
    brain.monitor();
    
    // Simulate work with some steps
    for (int step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Stop monitoring
    brain.stop_monitoring();
    
    // Get performance data (like Python would access)
    auto monitor_data = brain.get_monitor_data();
    assert(!monitor_data.empty());
    
    // Get stats (Python-style method calls)
    auto stats = brain.get_performance_stats();
    assert(stats["neuron_count"] > 0);
    assert(stats["computation_time_ms"] >= 0.0);
    
    std::cout << "    testPythonPerformanceMonitoringAPI passed" << std::endl;
}

void testPythonModelExportAPI() {
    std::cout << "    testPythonModelExportAPI started" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test model export (Python-style)
    std::string model_path = "/tmp/trained_model.nlm";
    
    bool export_result = brain.export_model(model_path);
    assert(export_result);
    
    // Verify file exists
    std::ifstream file(model_path, std::ios::binary);
    assert(file.good());
    file.close();
    
    // Clean up
    std::remove(model_path.c_str());
    
    std::cout << "    testPythonModelExportAPI passed" << std::endl;
}

void testPythonIntegrationWithConfig() {
    std::cout << "    testPythonIntegrationWithConfig started" << std::endl;
    
    // Create enhanced config (like Python's pynlm.create_default_config())
    auto config = std::make_shared<nlm::Config>();
    
    // Configure with Python-style settings
    config->set("neuron_count", 200, nlm::ConfigSource::Default);
    config->set("plasticity_rate", 0.01, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.001, nlm::ConfigSource::Default);
    
    // Add validation rules (like Python schema validation)
    config->addRangeValidator("learning_rate", 0.0, 0.1, "Learning rate validation");
    config->addTypeValidator("enable_plasticity", nlm::TypeValidator::ExpectedType::BOOL, "Plasticity flag");
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test advanced features with enhanced config
    std::vector<std::vector<float>> data;
    data.push_back(std::vector<float>(100, 0.7f));
    
    // Use Python-style API
    bool trained = brain.train(data, 5, 0.01f);
    assert(trained);
    
    float accuracy = brain.evaluate(data);
    assert(accuracy >= 0.0f);
    
    // Create checkpoint
    bool checkpointed = brain.checkpoint("/tmp/integration_checkpoint", 
                                        {{ "model_type", "neural_network" }});
    assert(checkpointed);
    
    std::cout << "    testPythonIntegrationWithConfig passed" << std::endl;
}

// Test Python binding compatibility (simulating pybind11 usage)
void testPybind11Compatibility() {
    std::cout << "    testPybind11Compatibility started" << std::endl;
    
    // Test that the C++ API can be called from Python
    // This simulates what pybind11 would expose
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    // This would be called from Python as: brain.train(training_data, epochs, learning_rate)
    nlm::Brain brain(config);
    
    std::vector<std::vector<float>> training_data;
    training_data.push_back(std::vector<float>(10, 0.8f));
    
    bool result = brain.train(training_data, 5, 0.01f);
    assert(result);
    
    // This would be called from Python as: brain.evaluate(test_data)
    float accuracy = brain.evaluate(training_data);
    assert(accuracy >= 0.0f);
    
    std::cout << "    testPybind11Compatibility passed" << std::endl;
}

// Comprehensive Python API test runner
void runAllPythonBindingsTests() {
    std::cout << "=== Running Python Bindings Advanced Tests ===" << std::endl;
    
    testPythonTrainingAPI();
    testPythonEvaluationAPI();
    testPythonCheckpointAPI();
    testPythonVisualizationAPI();
    testPythonPerformanceMonitoringAPI();
    testPythonModelExportAPI();
    testPythonIntegrationWithConfig();
    testPybind11Compatibility();
    
    std::cout << "=== All Python Bindings Tests Passed ===" << std::endl;
}

} // namespace test_python_bindings_advanced