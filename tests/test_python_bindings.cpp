// Python Bindings Test Suite
// Simple test that verifies the enhanced NLM Python bindings
// This test uses the existing pynlm module, not the bindings directly

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace test_python_bindings {

PYBIND11_MODULE(test_pynlm, m) {
    m.doc() = "Simple test module for verifying NLM Python bindings functionality";
    
    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Simple test function to verify basic Python bindings functionality
    m.def("test_basic_bindings", []() {
        std::cout << "Testing basic NLM Python bindings functionality..." << std::endl;
        
        try {
            // Try to import the main pynlm module
            py::module_ pynlm = py::module_::import("pynlm");
            
            // Test creating default config
            auto config = pynlm.attr("createDefaultConfig")();
            std::cout << "✓ Default config created" << std::endl;
            
            // Test creating brain
            auto brain = pynlm.attr("createBrain")(config);
            std::cout << "✓ Brain created" << std::endl;
            
            // Test brain initialization
            bool initSuccess = brain.attr("initialize")();
            std::cout << "✓ Brain initialized: " << (initSuccess ? "true" : "false") << std::endl;
            
            // Test world creation
            auto world = pynlm.attr("createSimpleWorld")();
            std::cout << "✓ World created" << std::endl;
            
            // Test agent creation
            auto agent = pynlm.attr("createAgentBrain")(brain);
            std::cout << "✓ Agent created" << std::endl;
            
            // Test agent initialization
            agent.attr("initialize")(world);
            std::cout << "✓ Agent initialized" << std::endl;
            
            // Enable learning features
            agent.attr("enableRewardModulation")(true);
            agent.attr("enableCuriosity")(true);
            agent.attr("enableDevelopment")(true);
            agent.attr("enableStructuralPlasticity")(true);
            std::cout << "✓ Learning features enabled" << std::endl;
            
            // Run simple simulation
            for (int i = 0; i < 5; ++i) {
                world.attr("update")(0.1);
                auto percept = world.attr("getSensoryPercept")();
                agent.attr("processSensoryInput")(percept);
                brain.attr("step")(i);
                auto action = agent.attr("decodeMotorCommand")();
                world.attr("applyMotorCommand")(action, world.attr("getSimulationTime")());
            }
            
            auto total_spikes = brain.attr("getTotalSpikeCount")();
            auto firing_rate = brain.attr("getAverageFiringRate")();
            
            return py::dict(
                "config_created", true,
                "brain_created", true,
                "brain_initialized", initSuccess,
                "world_created", true,
                "agent_created", true,
                "agent_initialized", true,
                "features_enabled", true,
                "simulation_completed", true,
                "total_spikes", total_spikes,
                "firing_rate", firing_rate,
                "test_status", "PASSED"
            );
        } catch (pybind11::error_already_set &e) {
            std::string error_msg = std::string("Python error: ") + e.what();
            return py::dict(
                "test_status", "FAILED",
                "error_message", error_msg
            );
        } catch (...) {
            return py::dict(
                "test_status", "FAILED",
                "error_message", "Unknown error occurred during Python bindings test"
            );
        }
    });
}

} // namespace test_python_bindings
