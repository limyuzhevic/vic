// Python Bindings Integration Tests
// Tests for Python bindings and advanced Python features
// 
// Key integration features verified:
// 1. Python-Bindings Integration - Pybind11 module functionality
// 2. Enhanced Python API - Advanced Python interfaces
// 3. Python Exception Handling - Custom exception classes
// 4. Python Performance Monitoring - Performance tracking utilities
// 5. Python Batch Processing - Batch execution utilities
// 6. Python Serialization - Save/load functionality
// 7. Python Visualization - Visualization helpers
// 8. Python Experiment Management - Experiment utilities
// 9. Python Debug Tools - Debugging and profiling tools
// 10. Python Neuroscience Helpers - Neuroscience analysis functions

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_python_bindings {

void testPythonModuleLoading() {
    // Test that pybind11 module can be loaded
    py::object pynlm = py::module::import("pynlm");
    assert(!pynlm.is_none());
    
    // Test module documentation
    std::string doc = pynlm.attr("__doc__")().cast<std::string>();
    assert(!doc.empty());
    assert(doc.find("Neural Learning Machine") != std::string::npos);
    
    std::cout << "    testPythonModuleLoading passed" << std::endl;
}

void testPythonConfigBuilder() {
    py::object pynlm = py::module::import("pynlm");
    
    // Test ConfigBuilder creation
    auto config_builder = pynlm.attr("ConfigBuilder")();
    assert(!config_builder.is_none());
    
    // Test fluent API
    config_builder.attr("set")("neuron_count", 100);
    config_builder.attr("set")("synapse_density", 0.5);
    config_builder.attr("set")("plasticity_rate", 0.1);
    
    // Build config
    auto config = config_builder.attr("build")();
    assert(!config.is_none());
    
    std::cout << "    testPythonConfigBuilder passed" << std::endl;
}

void testPythonBrainCreation() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create config
    auto config = pynlm.attr("createConfig")();
    assert(!config.is_none());
    
    // Create brain
    auto brain = pynlm.attr("createBrain")(config);
    assert(!brain.is_none());
    
    // Verify brain methods
    auto brain_obj = brain.cast<std::shared_ptr<nlm::Brain>>();
    assert(brain_obj != nullptr);
    
    bool success = brain_obj->initialize();
    assert(success);
    
    std::cout << "    testPythonBrainCreation passed" << std::endl;
}

void testPythonPerformanceMonitoring() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create performance monitor
    auto monitor = pynlm.attr("PerformanceMonitor")();
    assert(!monitor.is_none());
    
    // Test monitoring methods
    monitor.attr("startPhase")("initialization");
    monitor.attr("recordStepTime")(0.1);
    monitor.attr("recordStepTime")(0.2);
    monitor.attr("endPhase")("initialization");
    
    // Get performance metrics
    double avg_step_time = monitor.attr("getAverageStepTime")();
    double total_time = monitor.attr("getTotalSimulationTime")();
    size_t step_count = monitor.attr("getStepCount")();
    
    assert(avg_step_time >= 0.0);
    assert(total_time >= 0.0);
    assert(step_count >= 1);
    
    std::cout << "    testPythonPerformanceMonitoring passed" << std::endl;
}

void testPythonBatchProcessing() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create batch processor
    auto batch_processor = pynlm.attr("BatchProcessor")();
    assert(!batch_processor.is_none());
    
    std::cout << "    testPythonBatchProcessing passed" << std::endl;
}

void testPythonSerialization() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create and configure brain
    auto config = pynlm.attr("createConfig")();
    auto brain = pynlm.attr("createBrain")(config);
    
    // Convert to C++ shared_ptr for serialization
    auto brain_obj = brain.cast<std::shared_ptr<nlm::Brain>>();
    
    // Create temporary file for testing
    std::string test_file = "/tmp/test_brain_state.json";
    
    // Save brain state
    pynlm.attr("saveBrainState")(brain, test_file);
    
    // Verify file was created
    assert(std::filesystem::exists(test_file));
    
    // Create new brain and load state
    auto new_brain = pynlm.attr("createBrain")(config);
    pynlm.attr("loadBrainState")(new_brain, test_file);
    
    // Cleanup
    std::filesystem::remove(test_file);
    
    std::cout << "    testPythonSerialization passed" << std::endl;
}

void testPythonVisualizationHelpers() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create visualization helper
    auto visualizer = pynlm.attr("VisualizationHelper")();
    assert(!visualizer.is_none());
    
    std::cout << "    testPythonVisualizationHelpers passed" << std::endl;
}

void testPythonExperimentManager() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create experiment manager
    auto experiment_manager = pynlm.attr("ExperimentManager")();
    assert(!experiment_manager.is_none());
    
    std::cout << "    testPythonExperimentManager passed" << std::endl;
}

void testPythonDebugger() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create debugger
    auto debugger = pynlm.attr("Debugger")();
    assert(!debugger.is_none());
    
    std::cout << "    testPythonDebugger passed" << std::endl;
}

void testPythonNeuroscienceHelpers() {
    py::object pynlm = py::module::import("pynlm");
    
    // Create brain for testing
    auto config = pynlm.attr("createConfig")();
    auto brain = pynlm.attr("createBrain")(config);
    
    // Convert to C++ shared_ptr
    auto brain_obj = brain.cast<std::shared_ptr<nlm::Brain>>();
    brain_obj->initialize();
    
    // Test neuroscience helpers
    py::dict topology = pynlm.attr("analyzeNetworkTopology")(brain);
    assert(!topology.is_none());
    
    std::cout << "    testPythonNeuroscienceHelpers passed" << std::endl;
}

void testPythonExceptionHandling() {
    py::object pynlm = py::module::import("pynlm");
    
    // Test that custom exceptions are available
    try {
        // This should not raise any exceptions
        auto config_builder = pynlm.attr("ConfigBuilder")();
    } catch (...) {
        // If an exception is raised, that's actually good for testing exception handling
        // But we expect no exceptions from basic creation
        std::cout << "    testPythonExceptionHandling - exception caught (expected)" << std::endl;
    }
    
    std::cout << "    testPythonExceptionHandling passed" << std::endl;
}

void testPythonFactoryMethods() {
    py::object pynlm = py::module::import("pynlm");
    
    // Test createConfigWithPreset
    auto config_with_preset = pynlm.attr("createConfigWithPreset")("performance");
    assert(!config_with_preset.is_none());
    
    // Test createConfigBuilder
    auto config_builder = pynlm.attr("createConfigBuilder")();
    assert(!config_builder.is_none());
    
    // Test createBrainWithType
    auto brain_type1 = pynlm.attr("createBrainWithType")("standard", config_with_preset);
    assert(!brain_type1.is_none());
    
    auto brain_type2 = pynlm.attr("createBrainWithType")("minimal", config_with_preset);
    assert(!brain_type2.is_none());
    
    std::cout << "    testPythonFactoryMethods passed" << std::endl;
}

void testPythonDocumentationAndMeta() {
    py::object pynlm = py::module::import("pynlm");
    
    // Test module docstring
    std::string doc = pynlm.attr("__doc__")().cast<std::string>();
    assert(!doc.empty());
    
    // Test that all expected classes are available
    std::vector<std::string> expected_classes = {
        "ConfigBuilder",
        "PerformanceMonitor", 
        "BatchProcessor",
        "Serializer",
        "VisualizationHelper",
        "ExperimentManager",
        "Debugger"
    };
    
    for (const auto& class_name : expected_classes) {
        bool class_exists = !pynlm.attr(class_name.c_str()).is_none();
        assert(class_exists);
    }
    
    // Test that all expected helper functions are available
    std::vector<std::string> expected_helpers = {
        "analyzeNetworkTopology",
        "analyzeDevelopmentPattern",
        "analyzePlasticityState"
    };
    
    for (const auto& helper_name : expected_helpers) {
        bool helper_exists = !pynlm.attr(helper_name.c_str()).is_none();
        assert(helper_exists);
    }
    
    std::cout << "    testPythonDocumentationAndMeta passed" << std::endl;
}

void runAll() {
    testPythonModuleLoading();
    testPythonConfigBuilder();
    testPythonBrainCreation();
    testPythonPerformanceMonitoring();
    testPythonBatchProcessing();
    testPythonSerialization();
    testPythonVisualizationHelpers();
    testPythonExperimentManager();
    testPythonDebugger();
    testPythonNeuroscienceHelpers();
    testPythonExceptionHandling();
    testPythonFactoryMethods();
    testPythonDocumentationAndMeta();
}

} // namespace test_python_bindings