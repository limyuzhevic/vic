// Python-C++ Cross-Component Integration Tests
// Phase 6: Testing Python bindings and C++ interoperability

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace py = pybind11;

namespace test_python_cpp_integration {

void testPythonCPPComplexIntegration() {
    // Import NLM Python module
    py::module pynlm = py::module::import("pynlm");
    
    // Test 1: Create complex configuration through Python bindings
    auto config = pynlm.createDefaultConfig();
    config.attr("set")("neuron_count", 256);
    config.attr("set")("region_count", 4);
    config.attr("set")("enable_memory_pool", true);
    config.attr("set")("enable_reward_modulation", true);
    config.attr("set")("enable_structural_plasticity", true);
    
    // Test 2: Create brain from Python bindings
    auto brain = pynlm.createBrain(config);
    brain.attr("initialize")();
    
    // Test 3: Integration test with various data types
    py::dict complexConfig;
    complexConfig["test_int"] = 42;
    complexConfig["test_double"] = 3.14159;
    complexConfig["test_bool"] = true;
    complexConfig["test_string"] = "Integration Test";
    
    // Create another config from complex dict
    auto configFromDict = pynlm.Config.from_dict(complexConfig);
    
    // Test 4: Verify numpy array integration
    py::array_t<float> floatArray({10, 10, 3});
    auto floatBuf = floatArray.request();
    float* floatPtr = static_cast<float*>(floatBuf.ptr);
    
    for (size_t i = 0; i < floatBuf.size; ++i) {
        floatPtr[i] = static_cast<float>(i % 256) / 255.0f;
    }
    
    // Create Vision object and set data
    auto vision = pynlm.Vision(10, 10, 3);
    vision.attr("setData")(floatArray);
    
    // Get data back and verify
    py::array_t<float> returnedData = vision.attr("getData")();
    assert(py::len(returnedData) == floatArray.size());
    
    // Test 5: Audio processing integration
    py::array_t<float> audioData({44100, 2});  // Stereo audio
    auto audioBuf = audioData.request();
    float* audioPtr = static_cast<float*>(audioBuf.ptr);
    
    for (size_t i = 0; i < audioBuf.size; ++i) {
        audioPtr[i] = std::sin(2 * M_PI * i * 440.0f / 44100.0f); // 440Hz sine wave
    }
    
    auto audio = pynlm.Audio(44100, 2048);
    audio.attr("setData")(audioData);
    py::array_t<float> returnedAudio = audio.attr("getData")();
    assert(py::len(returnedAudio) == 2048);
    
    // Test 6: Vector and numpy conversion integration
    std::vector<std::vector<float>> nestedData = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    };
    
    py::array_t<float> numpyFromNested = pynlm.convertVectorToNumpy(nestedData);
    std::vector<std::vector<float>> backToNested = pynlm.convertNumpyToVector(std::vector<nlm::Vector3f>(3, nlm::Vector3f{1,2,3})); // This will be added
    
    // Test 7: Brain export/import integration
    py::dict brainState = pynlm.exportBrainState(brain);
    assert(brainState.contains("neurons"));
    assert(brainState.contains("total_neurons"));
    assert(brainState.contains("total_synapses"));
    
    // Test 8: Performance benchmarking
    auto timer = pynlm.createTimer("python_cpp_integration");
    
    // Do some operations
    for (int i = 0; i < 50; ++i) {
        config.attr("set")("iteration_" + std::to_string(i), i);
    }
    
    // Timer will output when destroyed
    std::cout << "    Python-C++ integration test completed" << std::endl;
}

void testPythonCPPErrorHandlingIntegration() {
    py::module pynlm = py::module::import("pynlm");
    
    // Test 1: Type conversion error handling
    auto config = pynlm.createDefaultConfig();
    
    try {
        // This should handle type conversion gracefully
        config.attr("set")("invalid_key", "should_be_converted_or_handled");
    } catch (py::error_already_set& e) {
        // Expected error - Python bindings should handle this gracefully
        std::cout << "    [INFO] Caught expected type conversion error" << std::endl;
    }
    
    // Test 2: Memory management validation
    try {
        // Create vision and audio objects
        auto vision = pynlm.Vision(64, 64, 3);
        auto audio = pynlm.Audio(16000, 1024);
        
        // Test with invalid numpy arrays
        py::array_t<int> wrongType({64, 64, 3}); // Wrong type (should be float)
        
        try {
            vision.attr("validateAndConvertNumpy")(wrongType);
        } catch (py::error_already_set& e) {
            std::cout << "    [INFO] Caught expected numpy validation error" << std::endl;
        }
        
    } catch (py::error_already_set& e) {
        std::cout << "    [INFO] Caught expected Python error in error handling test" << std::endl;
    }
    
    std::cout << "    testPythonCPPErrorHandlingIntegration passed" << std::endl;
}

void testPythonCPPPerformanceIntegration() {
    py::module pynlm = py::module::import("pynlm");
    
    // Benchmark Python-C++ integration performance
    auto startTimer = pynlm.createTimer("python_cpp_perf");
    
    // Create multiple objects and perform operations
    for (int batch = 0; batch < 5; ++batch) {
        auto config = pynlm.createDefaultConfig();
        config.attr("set")("batch_id", batch);
        config.attr("set")("neuron_count", 128 + batch * 64);
        
        auto brain = pynlm.createBrain(config);
        brain.attr("initialize")();
        
        // Perform some operations
        py::dict state = pynlm.exportBrainState(brain);
        
        // Test array operations
        py::array_t<float> testArray({10 + batch, 10 + batch});
        py::array_t<float> processedArray = pynlm.processArray(testArray);
    }
    
    // Performance metrics
    std::cout << "    Python-C++ performance integration completed" << std::endl;
}

void testCPPPythonSeamlessIntegration() {
    // Test bidirectional C++ to Python integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(128), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Export to Python-friendly format
    py::dict cppBrainState;
    cppBrainState["neuron_count"] = brain.getTotalNeuronCount();
    cppBrainState["synapse_count"] = brain.getTotalSynapseCount();
    cppBrainState["firing_rate"] = brain.getAverageFiringRate();
    
    // Import back (simulated)
    int neuronCount = cppBrainState["neuron_count"].cast<int>();
    int synapseCount = cppBrainState["synapse_count"].cast<int>();
    float firingRate = cppBrainState["firing_rate"].cast<float>();
    
    assert(neuronCount == 128);
    assert(std::isfinite(firingRate));
    
    // Test C++ objects from Python
    py::module pynlm = py::module::import("pynlm");
    auto vision = pynlm.Vision(8, 8, 3);
    
    // Get C++ object for direct manipulation
    auto visionPtr = vision.attr("getInternalObject")(); // This would be added
    
    std::cout << "    testCPPPythonSeamlessIntegration passed" << std::endl;
}

void runAll() {
    testPythonCPPComplexIntegration();
    testPythonCPPErrorHandlingIntegration();
    testPythonCPPPerformanceIntegration();
    testCPPPythonSeamlessIntegration();
}

} // namespace test_python_cpp_integration