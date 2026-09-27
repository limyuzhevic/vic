// Example: Using Enhanced NLM Python Bindings
// This file demonstrates how to use the enhanced Python bindings
// in a C++ context for performance-critical applications.

#include <pybind11/embed.h>
#include <iostream>
#include <string>

namespace py = pybind11;

void demonstrate_basic_usage() {
    std::cout << "=== Enhanced NLM Python Bindings Example ===" << std::endl;
    std::cout << std::endl;
    
    // Initialize Python interpreter
    py::initialize_interpreter();
    
    try {
        // Import the enhanced bindings
        py::module_ pynlm = py::module_::import("pynlm");
        
        std::cout << "1. Creating enhanced configuration..." << std::endl;
        py::object createDefaultConfig = pynlm.attr("createDefaultConfig");
        py::object config = createDefaultConfig();
        
        config.attr("set")("neuron_count", 1000);
        config.attr("set")("connection_probability", 0.1);
        config.attr("set")("enable_development", true);
        
        std::cout << "   ✓ Configuration created with enhanced type safety" << std::endl;
        
        std::cout << std::endl;
        std::cout << "2. Creating brain with error handling..." << std::endl;
        py::object createBrain = pynlm.attr("createBrain");
        py::object brain = createBrain(config);
        
        brain.attr("initialize")();
        
        std::cout << "   ✓ Brain created and initialized successfully" << std::endl;
        std::cout << "   ✓ Total neurons: " << brain.attr("getTotalNeuronCount")() << std::endl;
        
        std::cout << std::endl;
        std::cout << "3. Demonstrating enhanced error handling..." << std::endl;
        py::object Action = pynlm.attr("Action");
        py::object invalid_action = Action.attr("__init__")();
        
        try {
            invalid_action.attr("setParameter")(100, 5.0);  // This should raise InvalidParameterError
            std::cout << "   ✗ Should have raised InvalidParameterError" << std::endl;
        } catch (py::error_already_set) {
            py::object err_type = py::str("InvalidParameterError");
            std::cout << "   ✓ Correctly caught InvalidParameterError" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "4. Demonstrating enhanced action API..." << std::endl;
        py::object ActionType = pynlm.attr("ActionType");
        py::object MoveForward = ActionType.attr("MoveForward");
        
        py::object action = Action.attr("__init__")(MoveForward, py::list({0.5, 0.8, 1.2}));
        
        std::cout << "   ✓ Action created: " << py::str(action.attr("getName")()) << std::endl;
        std::cout << "   ✓ Parameters: " << py::str(action.attr("getParameters")()) << std::endl;
        std::cout << "   ✓ Parameter count: " << action.attr("getParameterCount")() << std::endl;
        std::cout << "   ✓ Action ready: " << (action.attr("isReady")() ? "true" : "false") << std::endl;
        
        // Demonstrate safe parameter access
        try {
            py::object param = action.attr("getParameter")(1);
            std::cout << "   ✓ Safe parameter access: " << py::str(param) << std::endl;
        } catch (py::error_already_set) {
            std::cout << "   ✓ Caught invalid parameter error" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "5. Demonstrating enhanced world system..." << std::endl;
        py::object createSimpleWorld = pynlm.attr("createSimpleWorld");
        py::object world = createSimpleWorld();
        
        world.attr("initialize")(64, 64, 32, 32);
        
        // Create world objects
        py::object WorldObject = pynlm.attr("WorldObject");
        py::object WorldObjectType = pynlm.attr("WorldObjectType");
        
        py::object resource = WorldObject.attr("__init__")(
            10.0, 10.0, WorldObjectType.attr("Resource")(), 100.0, 0.8);
        
        world.attr("addObject")(resource);
        
        py::object distance = resource.attr("getDistance")(15.0, 15.0);
        std::cout << "   ✓ Distance from resource to (15,15): " << py::str(distance) << std::endl;
        
        std::cout << std::endl;
        std::cout << "6. Demonstrating enhanced configuration API..." << std::endl;
        py::object all_config = config.attr("getAll")();
        std::cout << "   ✓ Configuration keys: " << len(all_config.attr("keys")()) << std::endl;
        
        std::cout << std::endl;
        std::cout << "7. MemoryView support for large data..." << std::endl;
        py::object Vision = pynlm.attr("Vision");
        py::object vision = Vision.attr("__init__")(32, 32, 3);
        
        py::list vision_data;
        for (int i = 0; i < 3072; ++i) {
            vision_data.append(py::float_(0.1));
        }
        
        vision.attr("setData")(vision_data);
        py::object data_view = vision.attr("getDataPtr")();
        std::cout << "   ✓ Memory view created for vision data" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Example Complete ===" << std::endl;
        std::cout << "✓ All demonstrations completed successfully!" << std::endl;
        
    } catch (py::error_already_set& e) {
        std::cout << "Error: " << py::str(e.what()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Runtime error: " << e.what() << std::endl;
    }
    
    // Finalize Python interpreter
    py::finalize_interpreter();
}

int main() {
    demonstrate_basic_usage();
    return 0;
}