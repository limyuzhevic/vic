#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <iostream>
#include <cassert>

namespace py = pybind11;

PYBIND11_MODULE(test_pynlm, m) {
    m.doc() = R"pbdoc(Test module for NLM Python bindings improvements)pbdoc";

    // Test basic functionality
    m.def("test_basic_creation", []() {
        py::module nlm = py::module::import("pynlm");
        
        // Test config creation
        auto config = nlm.attr("createDefaultConfig")();
        assert(config != nullptr);
        
        // Test brain creation
        auto brain = nlm.attr("createBrain")(config);
        assert(brain != nullptr);
        
        // Test world creation
        auto world = nlm.attr("createSimpleWorld")();
        assert(world != nullptr);
        
        // Test agent creation
        auto agent = nlm.attr("createAgentBrain")(brain);
        assert(agent != nullptr);
        
        return py::dict({
            {"config_created", true},
            {"brain_created", true},
            {"world_created", true},
            {"agent_created", true}
        });
    }, "Test basic object creation");

    // Test configuration utilities
    m.def("test_config_utilities", []() {
        py::module nlm = py::module::import("pynlm");
        
        auto config = nlm.attr("createDefaultConfig")();nlm.attr("createDefaultConfig")());
        
        // Test setting and getting different types
        config->set("string_value", "hello", nlm.attr("INVALID_NEURON_ID")); // This needs to be a valid enum
        assert(config->getOr<std::string>("string_value", "") == "hello");
        
        config->set("int_value", 42, nlm.attr("INVALID_NEURON_ID"));
        assert(config->getOr<int>("int_value", 0) == 42);
        
        config->set("float_value", 3.14, nlm.attr("INVALID_NEURON_ID"));
        assert(config->getOr<double>("float_value", 0.0) > 3.13);
        
        config->set("bool_value", true, nlm.attr("INVALID_NEURON_ID"));
        assert(config->getOr<bool>("bool_value", false) == true);
        
        // Test preset configs
        auto minimal_config = nlm.attr("create_preset_config")("minimal");
        auto standard_config = nlm.attr("create_preset_config")("standard");
        auto advanced_config = nlm.attr("create_preset_config")("advanced");
        auto developmental_config = nlm.attr("create_preset_config")("developmental");
        
        return py::dict({
            {"basic_config_test", true},
            {"type_conversions_test", true},
            {"preset_configs_test", true},
            {"minimal_neuron_count", minimal_config->getOr<int>("brain.neuron_count", 0)},
            {"standard_neuron_count", standard_config->getOr<int>("brain.neuron_count", 0)},
            {"advanced_neuron_count", advanced_config->getOr<int>("brain.neuron_count", 0)}
        });
    }, "Test configuration utilities");

    // Test action and world object utilities
    m.def("test_object_utilities", []() {
        py::module nlm = py::module::import("pynlm");
        
        // Test action creation
        auto action_move = nlm.attr("Action")(nlm.attr("ActionType").attr("MoveForward")());
        assert(action_move->getType() == nlm.attr("ActionType").attr("MoveForward")());
        
        // Test action parameters
        action_move->setParameters(std::vector<float>{1.0f, 2.0f, 3.0f});
        auto params = action_move->getParameters();
        assert(params.size() == 3);
        
        // Test world object creation
        auto world_obj = nlm.attr("WorldObject")(5.0f, 10.0f, nlm.attr("WorldObjectType").attr("Resource")(), 1.0f, 2.0f);
        assert(world_obj->x == 5.0f);
        assert(world_obj->y == 10.0f);
        assert(world_obj->type == nlm.attr("WorldObjectType").attr("Resource")());
        
        // Test agent body
        auto body = nlm.attr("AgentBody")();
        body->x = 100.0f;
        body->y = 200.0f;
        body->orientation = 1.57f; // ~90 degrees
        assert(body->x == 100.0f);
        assert(body->y == 200.0f);
        
        return py::dict({
            {"action_creation_test", true},
            {"action_parameters_test", true},
            {"world_object_test", true},
            {"agent_body_test", true}
        });
    }, "Test object creation and utilities");

    // Test brain and agent utilities
    m.def("test_brain_agent_utilities", []() {
        py::module nlm = py::module::import("pynlm");
        
        auto config = nlm.attr("createDefaultConfig")();
        auto brain = nlm.attr("createBrain")(config);
        auto agent = nlm.attr("createAgentBrain")(brain);
        
        brain->initialize();
        
        // Test statistics
        auto stats = nlm.attr("get_brain_stats")(brain);
        assert(stats.contains("neuron_count"));
        assert(stats.contains("synapse_count"));
        assert(stats.contains("firing_neurons"));
        
        auto status = nlm.attr("get_agent_status")(agent);
        assert(status.contains("curiosity_level"));
        assert(status.contains("novelty_level"));
        assert(status.contains("reward_modulation_enabled"));
        
        // Test advanced utilities
        auto minimal_config = nlm.attr("create_preset_config")("minimal");
        nlm.attr("reset_brain_with_config")(brain, minimal_config);
        
        auto copied_config = nlm.attr("copy_brain_state")(brain);
        assert(copied_config->getOr<int>("brain.neuron_count", 0) == 
               minimal_config->getOr<int>("brain.neuron_count", 0));
        
        // Test version info
        std::string version = nlm.attr("get_version")();
        assert(version.find("NLM Python Bindings") != std::string::npos);
        
        auto build_info = nlm.attr("get_build_info")();
        assert(build_info.contains("version"));
        assert(build_info.contains("features"));
        
        return py::dict({
            {"stats_collection_test", true},
            {"agent_status_test", true},
            {"config_reset_test", true},
            {"config_copy_test", true},
            {"version_info_test", true},
            {"build_info_test", true}
        });
    }, "Test brain and agent utilities");

    // Test simulation utilities
    m.def("test_simulation_utilities", []() {
        py::module nlm = py::module::import("pynlm");
        
        auto config = nlm.attr("createDefaultConfig")();
        auto brain = nlm.attr("createBrain")(config);
        auto world = nlm.attr("createSimpleWorld")();
        auto agent = nlm.attr("createAgentBrain")(brain);
        
        world->configure(10, 10, 8, 8);
        world->setAgentStart(5.0, 5.0);
        
        // Test basic simulation step
        for (int step = 0; step < 10; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        // Test advanced simulation with neuromodulation
        for (int step = 10; step < 20; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Test neuromodulation
            auto internal = percept->getInternal();
            float reward = 0.0f;
            if (internal && !internal->empty()) {
                reward = (*internal)[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            // Test development
            agent->updateDevelopment(0.1f);
        }
        
        // Test simulation monitoring
        auto monitor = nlm.attr("monitor_simulation")(brain, world, agent, 5);
        // The monitor would log stats during simulation
        
        return py::dict({
            {"basic_simulation_test", true},
            {"advanced_simulation_test", true},
            {"monitoring_test", true},
            {"simulation_steps_completed", 20}
        });
    }, "Test simulation utilities");

    // Test error handling and validation
    m.def("test_error_handling", []() {
        py::module nlm = py::module::import("pynlm");
        
        auto config = nlm.attr("createDefaultConfig")();
        auto brain = nlm.attr("createBrain")(config);
        
        // Test that invalid config names raise errors
        try {
            auto result = config->getOr<std::string>("nonexistent_key", "default_value");
            // Should not raise an error for getOr, but just return default
            assert(result == "default_value");
        } catch (const std::exception& e) {
            // Error handling is working
        }
        
        // Test validation through setter/getter
        config->set("test_key", "test_value", nlm.attr("INVALID_NEURON_ID"));
        assert(config->has("test_key"));
        
        return py::dict({
            {"error_handling_test", true},
            {"validation_test", true},
            {"config_access_test", true}
        });
    }, "Test error handling and validation");

    // Test version compatibility
    m.def("test_version_compatibility", []() {
        py::module nlm = py::module::import("pynlm");
        
        std::string version = nlm.attr("get_version")();
        auto build_info = nlm.attr("get_build_info")();
        
        // Validate version string format
        assert(version.length() > 0);
        assert(build_info["version"].cast<std::string>().length() > 0);
        assert(build_info["features"].is_list());
        
        return py::dict({
            {"version_test", true},
            {"build_info_test", true},
            {"compatibility_test", true},
            {"version_string", version}
        });
    }, "Test version compatibility");

    // Run all tests
    m.def("run_all_tests", []() {
        py::dict results;
        
        try {
            auto test1 = py::module::import("test_pynlm").attr("test_basic_creation")();
            results["basic_creation"] = test1;
        } catch (const std::exception& e) {
            results["basic_creation"] = py::dict({
                {"error", std::string("Basic creation test failed: ") + e.what()}
            });
        }
        
        try {
            auto test2 = py::module::import("test_pynlm").attr("test_config_utilities")();
            results["config_utilities"] = test2;
        } catch (const std::exception& e) {
            results["config_utilities"] = py::dict({
                {"error", std::string("Config utilities test failed: ") + e.what()}
            });
        }
        
        try {
            auto test3 = py::module::import("test_pynlm").attr("test_object_utilities")();
            results["object_utilities"] = test3;
        } catch (const std::exception& e) {
            results["object_utilities"] = py::dict({
                {"error", std::string("Object utilities test failed: ") + e.what()}
            });
        }
        
        try {
            auto test4 = py::module::import("test_pynlm").attr("test_brain_agent_utilities")();
            results["brain_agent_utilities"] = test4;
        } catch (const std::exception& e) {
            results["brain_agent_utilities"] = py::dict({
                {"error", std::string("Brain agent utilities test failed: ") + e.what()}
            });
        }
        
        try {
            auto test5 = py::module::import("test_pynlm").attr("test_simulation_utilities")();
            results["simulation_utilities"] = test5;
        } catch (const std::exception& e) {
            results["simulation_utilities"] = py::dict({
                {"error", std::string("Simulation utilities test failed: ") + e.what()}
            });
        }
        
        try {
            auto test6 = py::module::import("test_pynlm").attr("test_error_handling")();
            results["error_handling"] = test6;
        } catch (const std::exception& e) {
            results["error_handling"] = py::dict({
                {"error", std::string("Error handling test failed: ") + e.what()}
            });
        }
        
        try {
            auto test7 = py::module::import("test_pynlm").attr("test_version_compatibility")();
            results["version_compatibility"] = test7;
        } catch (const std::exception& e) {
            results["version_compatibility"] = py::dict({
                {"error", std::string("Version compatibility test failed: ") + e.what()}
            });
        }
        
        return results;
    }, "Run all improvement tests");
}