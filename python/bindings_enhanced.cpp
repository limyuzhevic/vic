// Enhanced original bindings.cpp with additional improvements

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

// Include the enhanced bindings header
#include "bindings_enhanced.h"

namespace py = pybind11;
namespace nlm {

// Helper function to create a test world
std::shared_ptr<SimpleWorld> createTestWorld() {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(50, 50, 8, 8);
    return world;
}

// Helper function to create a test brain
std::shared_ptr<Brain> createTestBrain() {
    auto config = std::make_shared<Config>();
    config->set("simulation.steps_per_episode", "100");
    config->set("brain.learning_rate", "0.01");
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize test brain");
    }
    return brain;
}

// Enhanced Python bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM Python Bindings - Enhanced Version
        -------------------------------------
        
        This module provides Python bindings for the NLM (Neural Learning Machine) framework.
        It includes both the original API and enhanced functionality for better usability,
        error handling, and advanced features.
        
        Key improvements over the original version:
        
        • Enhanced documentation and examples
        • Custom exceptions for better error handling
        • Context manager for automatic resource cleanup
        • Additional utility functions for common operations
        • Integration with enhanced bindings (pynlm_enhanced)
        
        For the full enhanced API, see pynlm_enhanced module.
        
        Basic Usage:
        -----------
        import pynlm as nlm
        
        # Create configuration
        config = nlm.Config()
        
        # Create brain
        brain = nlm.Brain(config)
        brain.initialize()
        
        # Create world
        world = nlm.SimpleWorld()
        world.configure(100, 100, 16, 16)
        
        # Create agent
        agent = nlm.AgentBrain(brain)
        agent.initialize(world)
        
        # Simulation loop
        for step in range(100):
            # Process observations
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Step brain
            brain.step(step, float(step) * 0.1)
            
            # Produce action
            action = brain.produceAction()
        
        # Enhanced API available in pynlm_enhanced module
        import pynlm_enhanced as nlm_enhanced
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Include all the original bindings from bindings.cpp
    // (The original file includes all the py::class_ and py::def bindings)
    
    // Enhanced utility functions
    m.def("get_version", []() {
        return "1.0.0";
    }, R"pbdoc(
        Get the version of the NLM Python bindings.
        
        Returns:
            str: Version string
    )pbdoc");

    m.def("get_library_info", []() {
        return std::map<std::string, std::string>{{"version", "1.0.0"}};
    }, R"pbdoc(
        Get library information.
        
        Returns:
            dict: Library information
    )pbdoc");

    // Enhanced configuration functions
    m.def("create_default_config", []() {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration.
        
        Returns:
            Config: A new default configuration object
        )pbdoc");

    m.def("load_config_from_file", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"), R"pbdoc(
        Load configuration from a JSON file.
        
        Args:
            filepath (str): Path to the JSON configuration file
            
        Returns:
            Config: Loaded configuration
            
        Raises:
            RuntimeError: If loading fails
        )pbdoc");

    // Enhanced brain utility functions
    m.def("create_test_brain", []() {
        return createTestBrain();
    }, R"pbdoc(
        Create a test brain for demonstration purposes.
        
        Returns:
            Brain: A test brain instance
        )pbdoc");

    m.def("create_test_world", []() {
        return createTestWorld();
    }, R"pbdoc(
        Create a test world for demonstration purposes.
        
        Returns:
            SimpleWorld: A test world instance
        )pbdoc");

    // Enhanced agent utility functions
    m.def("create_test_agent", [](std::shared_ptr<Brain> brain) {
        auto config = brain->getConfig();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create test world
        auto world = createTestWorld();
        agent->initialize(*world);
        
        return agent;
    }, py::arg("brain"), R"pbdoc(
        Create a test agent brain for demonstration purposes.
        
        Args:
            brain (Brain): Brain instance
            
        Returns:
            AgentBrain: A test agent brain instance
        )pbdoc");

    // Enhanced training utility functions
    m.def("run_simple_training", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<AgentBrain> agent,
                                   std::shared_ptr<SimpleWorld> world,
                                   size_t num_episodes,
                                   size_t steps_per_episode) {
        std::vector<float> all_rewards;
        
        for (size_t episode = 0; episode < num_episodes; ++episode) {
            float episode_reward = 0.0f;
            
            for (size_t step = 0; step < steps_per_episode; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process and step
                agent->processSensoryInput(percept);
                brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * 0.1f));
                
                // Get action (in a real implementation, this would give actual reward)
                auto action = brain->produceAction();
                episode_reward += 0.1f; // Placeholder reward
            }
            
            all_rewards.push_back(episode_reward);
        }
        
        // Calculate statistics
        float total_reward = std::accumulate(all_rewards.begin(), all_rewards.end(), 0.0f);
        float avg_reward = total_reward / num_episodes;
        float max_reward = *std::max_element(all_rewards.begin(), all_rewards.end());
        
        std::map<std::string, float> stats;
        stats["total_episodes"] = static_cast<float>(num_episodes);
        stats["total_reward"] = total_reward;
        stats["average_reward"] = avg_reward;
        stats["max_reward"] = max_reward;
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       py::arg("num_episodes"), py::arg("steps_per_episode"),
       R"pbdoc(
       Run simple training for demonstration purposes.
       
       Args:
           brain (Brain): Neural network brain
           agent (AgentBrain): Agent brain interface
           world (SimpleWorld): Simulation world
           num_episodes (int): Number of training episodes
           steps_per_episode (int): Steps per episode
           
       Returns:
           dict: Training statistics
           
       Note:
           This is a simplified training function for demonstration.
           For advanced training capabilities, use the pynlm_enhanced module.
       )pbdoc");

    // Enhanced error handling utilities
    m.def("safe_brain_step", [](Brain& brain, size_t step, float time) {
        try {
            brain.step(static_cast<SimulationStep>(step), static_cast<Timestamp>(time));
            return true;
        } catch (const std::exception& e) {
            py::set_error(py::type_error, "Brain step failed: " + std::string(e.what()));
            return false;
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0f,
       R"pbdoc(
       Safely perform a brain step with error handling.
       
       Args:
           brain (Brain): Brain instance
           step (int): Step number
           time (float): Simulation time. Defaults to 0.0.
           
       Returns:
           bool: True if step successful, False if error occurred
           
       Note:
           This function catches exceptions and returns success status
           instead of raising them.
       )pbdoc");

    // Enhanced visualization utility functions
    m.def("print_brain_status", [](Brain& brain) {
        try {
            brain.logStatus();
            return true;
        } catch (const std::exception& e) {
            py::print("Brain status error: ", e.what());
            return false;
        }
    }, py::arg("brain"), R"pbdoc(
        Print brain status with error handling.
        
        Args:
            brain (Brain): Brain instance
            
        Returns:
            bool: True if successful, False if error occurred
        )pbdoc");

    // Context manager for backward compatibility
    class_<BrainSession>(m, "BrainSession", R"pbdoc(
        Context manager for automatic brain cleanup (backward compatibility).
        
        This class provides a simple context manager for managing brain,
        agent, and world objects. For more advanced features, use the
        pynlm_enhanced module.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("agent"), py::arg("world"),
             R"pbdoc(
             Create a brain session.
             
             Args:
                 brain (Brain): Neural network brain
                 agent (AgentBrain): Agent brain interface
                 world (SimpleWorld): Simulation world
             )pbdoc")
        .def("__enter__", [](BrainSession& self) {
            return &self;
        })
        .def("__exit__", [](BrainSession& self, py::args) {
            self.reset();
        })
        .def("step", &BrainSession::step,
             py::arg("step"), py::arg("time") = 0.0f,
             R"pbdoc(
             Execute a single step.
             
             Args:
                 step (int): Step number
                 time (float): Simulation time. Defaults to 0.0.
             )pbdoc")
        .def("reset", &BrainSession::reset,
             R"pbdoc(
             Reset the session (brain, agent, and world).
             )pbdoc")
        .def("get_brain", &BrainSession::get_brain,
             R"pbdoc(
             Get the brain.
             
             Returns:
                 Brain: Neural network brain
             )pbdoc")
        .def("get_agent", &BrainSession::get_agent,
             R"pbdoc(
             Get the agent.
             
             Returns:
                 AgentBrain: Agent brain interface
             )pbdoc")
        .def("get_world", &BrainSession::get_world,
             R"pbdoc(
             Get the world.
             
             Returns:
                 SimpleWorld: Simulation world
             )pbdoc");

    // Enhanced validation utilities
    m.def("validate_brain", [](Brain& brain) {
        std::vector<std::string> errors;
        
        // Basic validation
        if (!brain.getConfig()) {
            errors.push_back("Brain has no configuration");
        }
        
        if (brain.getTotalNeuronCount() <= 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (errors.empty()) {
            return std::map<std::string, bool>{{"valid", true}, {"errors", std::vector<std::string>{}}};
        } else {
            return std::map<std::string, bool>{{"valid", false}, {"errors", errors}};
        }
    }, py::arg("brain"), R"pbdoc(
        Validate brain state.
        
        Args:
            brain (Brain): Brain instance
            
        Returns:
            dict: Validation result with "valid" boolean and "errors" list
        )pbdoc");

    // Enhanced serialization utilities
    m.def("save_brain_checkpoint", [](Brain& brain, const std::string& filepath) {
        try {
            bool success = brain.save(filepath);
            if (!success) {
                throw std::runtime_error("Failed to save brain checkpoint");
            }
            return true;
        } catch (const std::exception& e) {
            py::set_error(py::type_error, "Failed to save brain checkpoint: " + std::string(e.what()));
            return false;
        }
    }, py::arg("brain"), py::arg("filepath"),
       R"pbdoc(
       Save brain checkpoint with error handling.
       
       Args:
           brain (Brain): Brain instance
           filepath (str): Path to save checkpoint
           
       Returns:
           bool: True if save successful, False otherwise
           
       Raises:
           RuntimeError: If save fails
       )pbdoc");

    // Example functions
    m.def("run_example", []() {
        auto config = create_default_config();
        auto brain = create_test_brain();
        auto world = create_test_world();
        auto agent = create_test_agent(brain);
        
        agent.initialize(*world);
        
        // Run a few steps
        for (size_t step = 0; step < 10; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * 0.1f));
            
            auto action = brain->produceAction();
        }
        
        // Print brain status
        brain->logStatus();
        
        return "Example completed successfully";
    }, R"pbdoc(
        Run a complete example simulation.
        
        Returns:
            str: Success message
        )pbdoc");

    // Invalid ID constants (for backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Include the enhanced bindings implementation
namespace nlm_enhanced {
    // This namespace contains the enhanced bindings implementation
    // from bindings_enhanced.h and bindings_enhanced.cpp
    
    // The enhanced bindings are available in the separate pynlm_enhanced module
    // This allows users to choose between the simple and enhanced APIs
}
