// Python Binding Tests
// Tests for pynlm Python module functionality
// Includes validation, error handling, and integration tests

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace test_python {

void testPythonBindingsLoaded() {
    // Test that Python module can be loaded
    try {
        pybind11::module_::module m = pybind11::module::import("pynlm");
        assert(m.attr("__version__").cast<std::string>() == "0.1.0");
        
        std::cout << "    testPythonBindingsLoaded passed" << std::endl;
    } catch (const pybind11::error_already_set& e) {
        std::cerr << "Python bindings not available: " << e.what() << std::endl;
        throw;
    }
}

void testPythonCreationFunctions() {
    pybind11::module_::module m = pybind11::module::import("pynlm");
    
    // Test creation functions
    auto config = m.attr("createDefaultConfig");
    auto brain = m.attr("createBrain");
    auto world = m.attr("createSimpleWorld");
    auto agent = m.attr("createAgentBrain");
    
    assert(config.ptr() != nullptr);
    assert(brain.ptr() != nullptr);
    assert(world.ptr() != nullptr);
    assert(agent.ptr() != nullptr);
    
    std::cout << "    testPythonCreationFunctions passed" << std::endl;
}

void testPythonSensoryCreation() {
    pybind11::module_::module m = pybind11::module::import("pynlm");
    
    // Test enhanced sensory classes
    pybind11::object Vision = m.attr("Vision");
    pybind11::object Audio = m.attr("Audio");
    pybind11::object InternalSignals = m.attr("InternalSignals");
    
    // Create instances
    pybind11::object vision = Vision(pybind11::arg(10), pybind11::arg(20), pybind11::arg(3));
    pybind11::object audio = Audio(pybind11::arg(44100), pybind11::arg(1024));
    pybind11::object internal = InternalSignals();
    
    // Test basic properties
    auto vision_width = vision.attr("getWidth")();
    auto audio_samples = audio.attr("getNumSamples")();
    
    assert(vision_width.cast<size_t>() == 10);
    assert(audio_samples.cast<size_t>() == 1024);
    
    std::cout << "    testPythonSensoryCreation passed" << std::endl;
}

void testPythonSensoryDataValidation() {
    pybind11::module_::module m = pybind11::module::import("pynlm");
    
    pybind11::object Vision = m.attr("Vision");
    pybind11::object Audio = m.attr("Audio");
    
    // Create sensory objects
    pybind11::object vision = Vision(pybind11::arg(10), pybind11::arg(10));
    pybind11::object audio = Audio(pybind11::arg(44100), pybind11::arg(100));
    
    // Test data setting
    std::vector<float> valid_data(100, 0.5f);
    pybind11::object data = pybind11::cast(valid_data);
    
    vision.attr("setData")(data);
    audio.attr("setData")(data);
    
    // Verify data was set
    auto vision_result = vision.attr("getData")();
    auto audio_result = audio.attr("getData")();
    
    assert(vision_result.cast<std::vector<float>>().size() == 100);
    assert(audio_result.cast<std::vector<float>>().size() == 100);
    
    std::cout << "    testPythonSensoryDataValidation passed" << std::endl;
}

void testPythonHelperFunctions() {
    pybind11::module_::module m = pybind11::module::import("pynlm");
    
    // Test helper functions
    pybind11::object validateFunc = m.attr("validateSensoryData");
    pybind11::object createFunc = m.attr("createSensoryFromVector");
    pybind11::object typesFunc = m.attr("getAvailableSensoryTypes");
    pybind11::object configFunc = m.attr("getDefaultBrainConfig");
    
    // Test validateSensoryData
    std::vector<float> valid_data = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    pybind11::object data_obj = pybind11::cast(valid_data);
    bool result = validateFunc(data_obj).cast<bool>();
    assert(result == true);
    
    // Test getAvailableSensoryTypes
    pybind11::object types_list = typesFunc();
    auto types = types_list.cast<std::vector<std::string>>();
    assert(types.size() == 3); // vision, audio, internal
    
    std::cout << "    testPythonHelperFunctions passed" << std::endl;
}

void testPythonErrorHandling() {
    pybind11::module_::module m = pybind11::module::import("pynlm");
    
    pybind11::object Vision = m.attr("Vision");
    pybind11::object Audio = m.attr("Audio");
    
    // Create sensory objects
    pybind11::object vision = Vision(pybind11::arg(10), pybind11::arg(10));
    pybind11::object audio = Audio(pybind11::arg(44100), pybind11::arg(100));
    
    // Test error handling for empty data
    std::vector<float> empty_data;
    pybind11::object empty_obj = pybind11::cast(empty_data);
    
    // Vision should throw for empty data
    try {
        vision.attr("setData")(empty_obj);
        assert(false && "Vision.setData() should throw for empty data");
    } catch (const pybind11::error_already_set& e) {
        std::string error_msg = e.what();
        assert(error_msg.find("cannot be empty") != std::string::npos);
    }
    
    // Audio should throw for empty data  
    try {
        audio.attr("setData")(empty_obj);
        assert(false && "Audio.setData() should throw for empty data");
    } catch (const pybind11::error_already_set& e) {
        std::string error_msg = e.what();
        assert(error_msg.find("cannot be empty") != std::string::npos);
    }
    
    std::cout << "    testPythonErrorHandling passed" << std::endl;
}

void testPythonIntegration() {
    pybind11::module_::module m = pybind11::module::import("pynlm");
    
    // Create full enhanced simulation
    auto config = m.attr("createDefaultConfig")();
    auto brain = m.attr("createBrain")(config);
    brain.attr("initialize")();
    
    auto world = m.attr("createSimpleWorld")();
    world.attr("configure")(10, 10, 8, 8);
    world.attr("reset")();
    
    auto agent = m.attr("createAgentBrain")(brain);
    agent.attr("initialize")(world);
    
    // Enable advanced features
    agent.attr("enableRewardModulation")(true);
    agent.attr("enableCuriosity")(true);
    
    // Run simulation with enhanced sensory
    for (int step = 0; step < 20; ++step) {
        world.attr("update")(0.1);
        auto percept = world.attr("getSensoryPercept")();
        agent.attr("processSensoryInput")(percept);
        brain.attr("step")(step);
        
        auto action = agent.attr("decodeMotorCommand")();
        world.attr("applyMotorCommand")(action, world.attr("getSimulationTime")());
        
        // Print progress
        if (step % 5 == 0) {
            auto firing = brain.attr("getFiringNeuronCount")();
            auto curiosity = agent.attr("getCuriosityLevel")();
            std::cout << "      Step " << step << ": "
                      << "Firing: " << firing.cast<size_t>() << ", "
                      << "Curiosity: " << curiosity.cast<float>() << std::endl;
        }
    }
    
    std::cout << "    testPythonIntegration passed" << std::endl;
}

void testPythonEnhancedSensoryAPI() {
    pybind11::module_::module m = pybind11::module::import("pynlm");
    
    // Test enhanced sensory API
    pybind11::object VisionProcessor = m.attr("VisionProcessor");
    pybind11::object AudioProcessor = m.attr("AudioProcessor");
    pybind11::object InternalSignalsProcessor = m.attr("InternalSignalsProcessor");
    
    // Create processors
    pybind11::object vp = VisionProcessor();
    pybind11::object ap = AudioProcessor();
    pybind11::object ip = InternalSignalsProcessor();
    
    // Verify they have expected methods
    vp.attr("getFeatureDimensions")();  // Should not crash
    ap.attr("getFeatureDimensions")();  // Should not crash
    ip.attr("getProcessedSignals")();  // Should not crash
    
    std::cout << "    testPythonEnhancedSensoryAPI passed" << std::endl;
}

void runAll() {
    testPythonBindingsLoaded();
    testPythonCreationFunctions();
    testPythonSensoryCreation();
    testPythonSensoryDataValidation();
    testPythonHelperFunctions();
    testPythonErrorHandling();
    testPythonIntegration();
    testPythonEnhancedSensoryAPI();
}

} // namespace test_python