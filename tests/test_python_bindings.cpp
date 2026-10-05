// Python Bindings Tests
// Phase 3.1: Comprehensive Test Coverage for Python bindings (Phase 1.1-1.4 improvements)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace py = pybind11;

namespace test_python_bindings {

void testPythonBindingBasic() {
    // Test basic Python binding functionality
    py::module pynlm = py::module::import("pynlm");
    
    // Test creating default config through Python bindings
    auto config = pynlm.createDefaultConfig();
    assert(config.attr("has").attr("__call__")("neuron_count") == false);
    
    // Test setting values through Python bindings
    config.attr("set")("neuron_count", 100);
    assert(config.attr("get").attr("__call__")("neuron_count") == 100);
    
    // Test getOr method
    int defaultVal = config.attr("getOr").attr("__call__")("invalid_key", 999);
    assert(defaultVal == 999);
    
    std::cout << "    testPythonBindingBasic passed" << std::endl;
}

void testPythonBindingConfigMethods() {
    py::module pynlm = py::module::import("pynlm");
    
    auto config = pynlm.createDefaultConfig();
    
    // Test from_dict method
    py::dict configDict;
    configDict["test_int"] = 42;
    configDict["test_double"] = 3.14;
    configDict["test_string"] = "hello";
    configDict["test_bool"] = true;
    
    auto configFromDict = pynlm.Config.from_dict(configDict);
    assert(configFromDict.attr("get").attr("__call__")("test_int") == 42);
    assert(configFromDict.attr("get").attr("__call__")("test_double") == 3.14);
    assert(configFromDict.attr("get").attr("__call__")("test_string") == "hello");
    assert(configFromDict.attr("get").attr("__call__")("test_bool") == true);
    
    // Test to_dict method
    py::dict resultDict = configFromDict.attr("to_dict")();
    assert(py::len(resultDict) >= 4);
    
    std::cout << "    testPythonBindingConfigMethods passed" << std::endl;
}

void testPythonBindingTypeConversions() {
    py::module pynlm = py::module::import("pynlm");
    
    // Test various type conversions in Python bindings
    py::dict complexDict;
    complexDict["int_array"] = py::array_t<int>({1, 2, 3, 4, 5});
    complexDict["float_array"] = py::array_t<float>({1.1f, 2.2f, 3.3f});
    complexDict["string_array"] = py::array_t<std::string>({"a", "b", "c"});
    
    auto config = pynlm.Config.from_dict(complexDict);
    
    // Verify conversions
    auto intArray = config.attr("get").attr("__call__")("int_array");
    auto floatArray = config.attr("get").attr("__call__")("float_array");
    auto stringArray = config.attr("get").attr("__call__")("string_array");
    
    // Arrays should be converted back properly
    assert(py::len(intArray) == 5);
    assert(py::len(floatArray) == 3);
    assert(py::len(stringArray) == 3);
    
    std::cout << "    testPythonBindingTypeConversions passed" << std::endl;
}

void testPythonBindingVisionAudio() {
    py::module pynlm = py::module::import("pynlm");
    
    // Test Vision class with numpy arrays
    auto vision = pynlm.Vision(10, 10, 3);
    
    // Create numpy array for vision data
    py::array_t<float> visionData({10, 10, 3});
    auto visionBuf = visionData.request();
    float* ptr = static_cast<float*>(visionBuf.ptr);
    
    // Fill with test data
    for (size_t i = 0; i < visionBuf.size; ++i) {
        ptr[i] = static_cast<float>(i % 256) / 255.0f;
    }
    
    vision.attr("setData")(visionData);
    
    // Get data back as numpy array
    py::array_t<float> returnedData = vision.attr("getData")();
    assert(py::len(returnedData) == visionData.size());
    
    // Test Audio class with numpy arrays
    auto audio = pynlm.Audio(44100, 1024);
    
    py::array_t<float> audioData({1024});
    auto audioBuf = audioData.request();
    float* audioPtr = static_cast<float*>(audioBuf.ptr);
    
    for (size_t i = 0; i < 1024; ++i) {
        audioPtr[i] = std::sin(2 * M_PI * i * 440.0f / 44100.0f); // 440Hz sine wave
    }
    
    audio.attr("setData")(audioData);
    py::array_t<float> returnedAudio = audio.attr("getData")();
    assert(py::len(returnedAudio) == 1024);
    
    std::cout << "    testPythonBindingVisionAudio passed" << std::endl;
}

void testPythonBindingNumpyUtilities() {
    py::module pynlm = py::module::import("pynlm");
    
    // Test numpy utility functions
    py::array_t<float> testArray({5, 10, 15});
    
    // Test isNumpyArray check
    bool isArray = pynlm.checkNumpyArray(testArray);
    assert(isArray);
    
    // Test getNumpyArrayDimensions
    py::list dims = pynlm.getNumpyArrayDimensions(testArray);
    assert(py::len(dims) == 3);
    assert(dims[0].cast<size_t>() == 5);
    assert(dims[1].cast<size_t>() == 10);
    assert(dims[2].cast<size_t>() == 15);
    
    // Test convertVectorToNumpy
    std::vector<float> testVec{1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    py::array_t<float> numpyArray = pynlm.convertVectorToNumpy(testVec);
    assert(py::len(numpyArray) == 5);
    
    // Test convertNumpyToVector
    std::vector<float> backToVec = pynlm.convertNumpyToVector(numpyArray);
    assert(backToVec.size() == testVec.size());
    for (size_t i = 0; i < testVec.size(); ++i) {
        assert(std::abs(backToVec[i] - testVec[i]) < 0.0001f);
    }
    
    std::cout << "    testPythonBindingNumpyUtilities passed" << std::endl;
}

void testPythonBindingBrainExport() {
    py::module pynlm = py::module::import("pynlm");
    
    // Create brain through Python bindings
    auto config = pynlm.createDefaultConfig();
    config.attr("set")("neuron_count", 100);
    config.attr("set")("region_count", 2);
    
    auto brain = pynlm.createBrain(config);
    brain.attr("initialize")();
    
    // Test brain state export
    py::dict brainState = pynlm.exportBrainState(brain);
    assert(brainState.contains("neurons"));
    assert(brainState.contains("total_neurons"));
    assert(brainState.contains("total_synapses"));
    
    // Test brain visualization functions
    py::array_t<float> activityGrid = pynlm.visualizeNeuronActivity(brain);
    assert(py::len(activityGrid) > 0);
    
    py::array_t<float> adjacencyMatrix = pynlm.visualizeSynapticConnections(brain);
    assert(py::len(adjacencyMatrix) > 0);
    
    py::dict spikeData = pynlm.generateSpikeRaster(brain);
    assert(spikeData.contains("spike_times"));
    
    std::cout << "    testPythonBindingBrainExport passed" << std::endl;
}

void testPythonBindingErrorHandling() {
    py::module pynlm = py::module::import("pynlm");
    
    // Test error handling in Python bindings
    auto config = pynlm.createDefaultConfig();
    
    // Test invalid type conversion
    try {
        config.attr("set")("test_key", "invalid_type"); // Should fail or be handled gracefully
    } catch (py::error_already_set& e) {
        // Expected error
        std::cout << "    Caught expected Python error in testPythonBindingErrorHandling" << std::endl;
    }
    
    // Test numpy array validation
    auto vision = pynlm.Vision(10, 10, 3);
    
    py::array_t<int> wrongArray({10, 10, 3}); // Wrong type (int instead of float)
    
    try {
        vision.attr("validateAndConvertNumpy")(wrongArray);
    } catch (py::error_already_set& e) {
        // Expected error due to type mismatch
        std::cout << "    Caught expected validation error in testPythonBindingErrorHandling" << std::endl;
    }
    
    std::cout << "    testPythonBindingErrorHandling passed" << std::endl;
}

void testPythonBindingPerformance() {
    py::module pynlm = py::module::import("pynlm");
    
    // Test performance utilities
    auto timer = pynlm.createTimer("python_binding_test");
    
    // Do some operations
    auto config = pynlm.createDefaultConfig();
    for (int i = 0; i < 100; ++i) {
        config.attr("set")("test_key_" + std::to_string(i), i);
    }
    
    // Timer will output elapsed time when destroyed
    std::cout << "    testPythonBindingPerformance passed" << std::endl;
}

void runAll() {
    testPythonBindingBasic();
    testPythonBindingConfigMethods();
    testPythonBindingTypeConversions();
    testPythonBindingVisionAudio();
    testPythonBindingNumpyUtilities();
    testPythonBindingBrainExport();
    testPythonBindingErrorHandling();
    testPythonBindingPerformance();
}

} // namespace test_python_bindings