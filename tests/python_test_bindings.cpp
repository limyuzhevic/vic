// Python Bindings Integration Test
// Tests the integration between C++ core and Python bindings

#include <iostream>
#include <cassert>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
namespace test_python_bindings {

void testPythonBasicFunctionality() {
    std::cout << "Testing Python bindings basic functionality..." << std::endl;
    
    // Start Python interpreter
    pybind11::scoped_interpreter interpreter;
    
    // Import the pynlm module
    pybind11::module_ pynlm = pybind11::module_::import("pynlm");
    
    // Test basic functionality
    auto config = pynlm::createDefaultConfig();
    assert(config != nullptr);
    
    config.attr("set")("neuron_count", 100);
    config.attr("set")("region_count", 2);
    
    auto brain = pynlm::createBrain(config);
    assert(brain != nullptr);
    
    auto success = brain.attr("initialize")();
    assert(success.cast<bool>());
    
    assert(brain.attr("getTotalNeuronCount")().cast<int64_t>() == 100);
    assert(brain.attr("getRegionCount")().cast<int64_t>() == 2);
    
    std::cout << "  ✓ Basic functionality PASSED" << std::endl;
}

void testPythonConfig() {
    std::cout << "Testing Python config bindings..." << std::endl;
    
    pybind11::scoped_interpreter interpreter;
    pybind11::module_ pynlm = pybind11::module_::import("pynlm");
    
    auto config = pynlm::createDefaultConfig();
    
    // Test setting different value types
    config.attr("set")("int_value", 42);
    config.attr("set")("float_value", 3.14);
    config.attr("set")("string_value", "test");
    config.attr("set")("bool_value", true);
    
    assert(config.attr("has")("int_value").cast<bool>());
    assert(config.attr("has")("float_value").cast<bool>());
    assert(config.attr("has")("string_value").cast<bool>());
    assert(config.attr("has")("bool_value").cast<bool>());
    
    auto int_val = config.attr("getOr")("int_value", 0).cast<int>();
    assert(int_val == 42);
    
    auto default_val = config.attr("getOr")("missing_key", "default").cast<std::string>();
    assert(default_val == "default");
    
    std::cout << "  ✓ Config bindings PASSED" << std::endl;
}

void testPythonSensory() {
    std::cout << "Testing Python sensory bindings..." << std::endl;
    
    pybind11::scoped_interpreter interpreter;
    pybind11::module_ pynlm = pybind11::module_::import("pynlm");
    
    // Test Vision
    auto vision = pynlm::Vision(10, 10, 3);
    assert(vision.attr("getWidth")().cast<size_t>() == 10);
    assert(vision.attr("getHeight")().cast<size_t>() == 10);
    assert(vision.attr("getChannels")().cast<size_t>() == 3);
    
    auto test_data = pybind11::list();
    for (int i = 0; i < 300; ++i) {
        test_data.append(0.1f);
    }
    
    vision.attr("setData")(test_data);
    
    // Test Audio
    auto audio = pynlm::Audio(44100, 1024);
    assert(audio.attr("getSampleRate")().cast<size_t>() == 44100);
    assert(audio.attr("getNumSamples")().cast<size_t>() == 1024);
    
    audio.attr("setSampleRate")(48000);
    assert(audio.attr("getSampleRate")().cast<size_t>() == 48000);
    
    // Test InternalSignals
    auto internal = pynlm::InternalSignals();
    internal.attr("addSignal")(0.5);
    internal.attr("addSignal")(-0.3);
    
    std::cout << "  ✓ Sensory bindings PASSED" << std::endl;
}

void testPythonActions() {
    std::cout << "Testing Python action bindings..." << std::endl;
    
    pybind11::scoped_interpreter interpreter;
    pybind11::module_ pynlm = pybind11::module_::import("pynlm");
    
    // Test ActionType enum
    auto action_type = pynlm::ActionType_MoveForward;
    assert(action_type == pynlm::ActionType_MoveForward);
    
    // Test Action creation
    auto action = pynlm::Action(pynlm::ActionType_MoveForward);
    assert(action.attr("getType")() == pynlm::ActionType_MoveForward);
    assert(action.attr("getName")() == "MoveForward");
    
    // Test Action with parameters
    auto action2 = pynlm::Action(pynlm::ActionType_Custom, pybind11::list({0.1, 0.5, 0.9}));
    assert(action2.attr("getType")() == pynlm::ActionType_Custom);
    
    auto params = action2.attr("getParameters")();
    assert(params[0].cast<float>() == 0.1f);
    assert(params[1].cast<float>() == 0.5f);
    assert(params[2].cast<float>() == 0.9f);
    
    // Test action cloning
    auto action3 = action2.attr("clone")();
    assert(action3.attr("getType")() == action2.attr("getType")());
    assert(action3.attr("getParameters")() == action2.attr("getParameters")());
    
    std::cout << "  ✓ Action bindings PASSED" << std::endl;
}

void testPythonErrorHandling() {
    std::cout << "Testing Python error handling..." << std::endl;
    
    pybind11::scoped_interpreter interpreter;
    pybind11::module_ pynlm = pybind11::module_::import("pynlm");
    
    // Test that Python exceptions are properly handled
    bool invalid_id_caught = false;
    bool parameter_error_caught = false;
    
    try {
        auto neuron_id = pynlm::NeuronId(-1);
    } catch (pybind11::error_already_set& e) {
        invalid_id_caught = true;
    }
    
    assert(invalid_id_caught);
    std::cout << "  ✓ Invalid ID error handling PASSED" << std::endl;
    
    std::cout << "  ✓ Error handling PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Python Bindings Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    try {
        testPythonBasicFunctionality();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPythonConfig();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPythonSensory();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPythonActions();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPythonErrorHandling();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Python Bindings Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Python Bindings Tests FAILED ===" << std::endl;
    }
    
    // Return appropriate exit code
    if (allPassed) {
        return 0;
    } else {
        return 1;
    }
}

} // namespace test_python_bindings