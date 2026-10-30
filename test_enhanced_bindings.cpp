#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <iostream>

namespace py = pybind11;

void test_enhanced_bindings() {
    try {
        // Try to import the enhanced module
        py::module_ enhanced = py::module_::import("pynlm_enhanced");
        std::cout << "Successfully imported pynlm_enhanced" << std::endl;
        
        // Test ConfigBuilder
        auto config_builder = enhanced.attr("createConfig")();
        std::cout << "ConfigBuilder created" << std::endl;
        
        // Test EnhancedBrain creation
        auto brain = enhanced.attr("createEnhancedBrain")(config_builder);
        std::cout << "EnhancedBrain created" << std::endl;
        
        // Test Phase6Experiment
        auto experiment = enhanced.attr("createPhase6Experiment")();
        std::cout << "Phase6Experiment created" << std::endl;
        
        std::cout << "All tests passed!" << std::endl;
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    py::initialize_interpreter();
    
    try {
        test_enhanced_bindings();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
