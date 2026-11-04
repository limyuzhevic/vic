#pragma once

#include "EnhancedPythonBindings.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bindEnhancedPythonAPI(py::module& m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python API
        ========================
        
        A modern, Pythonic interface to the NLM (Neural Learning Machine) neural simulation framework.
        
        This enhanced API provides:
        
        1. **Simplified Configuration**
           - Factory functions for common use cases
           - Preset configurations (fast, detailed, learning, stable)
           - Configuration validation and error handling
        
        2. **Enhanced Brain API**
           - Simple initialization and simulation
           - Statistics collection and monitoring
           - State save/load with error handling
           - Development tracking
        
        3. **Rich Agent API**
           - Intuitive sensory processing
           - Motor command decoding
           - Neuromodulation control
           - Statistics collection
        
        4. **Interactive World API**
           - Simplified world configuration
           - Sensory percept access as Python dictionaries
           - Motor command application
           - World statistics
        
        5. **Convenience Functions**
           - One-line setup for common scenarios
           - Preset configuration application
           - Constants access
    )pbdoc";
    
    // === CONFIGURATION API ===
    
    // Main configuration factory
    py::class_<nlm::EnhancedConfigFactory>(m, "ConfigFactory")
        .def_static("create_config", &nlm::EnhancedConfigFactory::createConfig,
                   py::arg("neuron_count") = nlm::Constants::BRAIN_DEFAULT_NEURON_COUNT,
                   py::arg("connection_probability") = nlm::Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
                   py::arg("simulation_timestep") = nlm::Constants::DEFAULT_TIMESTEP,
                   py::arg("random_seed") = nlm::Constants::SIMULATION_DEFAULT_RANDOM_SEED,
                   py::arg("preset") = "default",
                   "Create a brain configuration with specified parameters and preset")
        
        .def_static("create_config_from_json", &nlm::EnhancedConfigFactory::createConfigFromJson,
                   "Create configuration from JSON string")
        
        .def_static("builder", &nlm::EnhancedConfigFactory::builder,
                   "Create a configuration builder for fluent API")
        
        .def_static("get_available_presets", &nlm::EnhancedConfigFactory::getAvailablePresets,
                   "Get list of available preset configurations")
        
        .def_static("validate_config", &nlm::EnhancedConfigFactory::validateConfig,
                   "Validate configuration and return errors if any")
        
        .def_static("apply_presets", &nlm::EnhancedConfigFactory::applyPresets,
                   "Apply multiple presets to a configuration")
        
        .def_static("configs_equal", &nlm::EnhancedConfigFactory::configsEqual,
                   "Compare two configurations for equality")
        
        .def_static("serialize_config", &nlm::EnhancedConfigFactory::serializeConfig,
                   "Serialize configuration to string")
        
        .def_static("deserialize_config", &nlm::EnhancedConfigFactory::deserializeConfig,
                   "Deserialize configuration from string");
    
    // Configuration Builder
    py::class_<nlm::EnhancedConfigFactory::ConfigBuilder>(m, "ConfigBuilder")
        .def("set_neuron_count", &nlm::EnhancedConfigFactory::ConfigBuilder::setNeuronCount,
             "Set number of neurons")
        .def("set_connection_probability", &nlm::EnhancedConfigFactory::ConfigBuilder::setConnectionProbability,
             "Set connection probability")
        .def("set_simulation_timestep", &nlm::EnhancedConfigFactory::ConfigBuilder::setSimulationTimestep,
             "Set simulation timestep")
        .def("set_random_seed", &nlm::EnhancedConfigFactory::ConfigBuilder::setRandomSeed,
             "Set random seed for reproducibility")
        .def("enable_plasticity", &nlm::EnhancedConfigFactory::ConfigBuilder::enablePlasticity,
             "Enable/disable plasticity")
        .def("set_learning_rate", &nlm::EnhancedConfigFactory::ConfigBuilder::setLearningRate,
             "Set STDP learning rate")
        .def("preset", &nlm::EnhancedConfigFactory::ConfigBuilder::preset,
             "Apply a preset configuration")
        .def("build", &nlm::EnhancedConfigFactory::ConfigBuilder::build,
             "Build and return the configuration");
    
    // === BRAIN API ===
    
    py::class_<nlm::PythonBrainWrapper>(m, "Brain")
        .def("initialize", &nlm::PythonBrainWrapper::initialize,
             "Initialize the brain with current configuration")
        
        .def("run_simulation", &nlm::PythonBrainWrapper::runSimulation,
             py::arg("steps"), py::arg("log_progress") = false,
             "Run simulation for specified number of steps")
        
        .def("get_statistics", &nlm::PythonBrainWrapper::getStatistics,
             "Get brain statistics as dictionary")
        
        .def("save_state", &nlm::PythonBrainWrapper::saveState,
             "Save brain state to file")
        
        .def("load_state", &nlm::PythonBrainWrapper::loadState,
             "Load brain state from file")
        
        .def("reset", &nlm::PythonBrainWrapper::reset,
             "Reset brain to initial state")
        
        .def("get_brain_age", &nlm::PythonBrainWrapper::getBrainAge,
             "Get estimated brain age in seconds")
        
        .def("is_still_learning", &nlm::PythonBrainWrapper::isStillLearning,
             "Check if brain is still learning (heuristic)");
    
    // === AGENT API ===
    
    py::class_<nlm::PythonAgentWrapper>(m, "Agent")
        .def("initialize", &nlm::PythonAgentWrapper::initialize,
             "Initialize agent with world")
        
        .def("process_sensory_input", &nlm::PythonAgentWrapper::processSensoryInput,
             "Process sensory input from world")
        
        .def("decode_motor_command", &nlm::PythonAgentWrapper::decodeMotorCommand,
             "Get motor command as string")
        
        .def("get_agent_statistics", &nlm::PythonAgentWrapper::getAgentStatistics,
             "Get agent statistics as dictionary")
        
        .def("enable_reward_modulation", &nlm::PythonAgentWrapper::enableRewardModulation,
             "Enable/disable reward modulation")
        
        .def("enable_structural_plasticity", &nlm::PythonAgentWrapper::enableStructuralPlasticity,
             "Enable/disable structural plasticity")
        
        .def("enable_development", &nlm::PythonAgentWrapper::enableDevelopment,
             "Enable/disable development")
        
        .def("enable_curiosity", &nlm::PythonAgentWrapper::enableCuriosity,
             "Enable/disable curiosity")
        
        .def("get_curiosity_level", &nlm::PythonAgentWrapper::getCuriosityLevel,
             "Get curiosity level (0.0 to 1.0)")
        
        .def("get_novelty_level", &nlm::PythonAgentWrapper::getNoveltyLevel,
             "Get novelty level (0.0 to 1.0)")
        
        .def("apply_reward_modulation", &nlm::PythonAgentWrapper::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward") = 0.0f,
             "Apply reward modulation with predicted reward")
        
        .def("update_development", &nlm::PythonAgentWrapper::updateDevelopment,
             "Update agent development");
    
    // === WORLD API ===
    
    py::class_<nlm::PythonWorldWrapper>(m, "World")
        .def("configure_world", &nlm::PythonWorldWrapper::configureWorld,
             py::arg("width"), py::arg("height"),
             py::arg("vision_width") = nlm::Constants::DEFAULT_VISION_WIDTH,
             py::arg("vision_height") = nlm::Constants::DEFAULT_VISION_HEIGHT,
             "Configure world dimensions")
        
        .def("set_agent_start", &nlm::PythonWorldWrapper::setAgentStart,
             "Set agent's starting position")
        
        .def("reset_world", &nlm::PythonWorldWrapper::resetWorld,
             "Reset world to initial state")
        
        .def("update_world", &nlm::PythonWorldWrapper::updateWorld,
             "Update world state")
        
        .def("get_sensory_percept", &nlm::PythonWorldWrapper::getSensoryPercept,
             "Get sensory percept as Python dictionary")
        
        .def("apply_motor_command", &nlm::PythonWorldWrapper::applyMotorCommand,
             "Apply motor command to world")
        
        .def("get_world_statistics", &nlm::PythonWorldWrapper::getWorldStatistics,
             "Get world statistics as dictionary")
        
        .def("set_random_seed", &nlm::PythonWorldWrapper::setRandomSeed,
             "Set random seed for reproducible results");
    
    // === CONVENIENCE FUNCTIONS ===
    
    m.def("create_enhanced_brain", &nlm::createEnhancedBrainWrapper,
          py::arg("neuron_count") = nlm::Constants::BRAIN_DEFAULT_NEURON_COUNT,
          py::arg("connection_probability") = nlm::Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
          py::arg("simulation_timestep") = nlm::Constants::DEFAULT_TIMESTEP,
          py::arg("random_seed") = nlm::Constants::SIMULATION_DEFAULT_RANDOM_SEED,
          py::arg("preset") = "default",
          "Create a brain with enhanced wrapper");
    
    m.def("create_enhanced_agent", &nlm::createEnhancedAgentWrapper,
          "Create an agent with enhanced wrapper");
    
    m.def("create_enhanced_world", &nlm::createEnhancedWorldWrapper,
          "Create a world with enhanced wrapper");
    
    // Basic convenience functions (backwards compatibility)
    m.def("create_brain", &nlm::createBrain,
          py::arg("neuron_count") = nlm::Constants::BRAIN_DEFAULT_NEURON_COUNT,
          py::arg("connection_probability") = nlm::Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
          py::arg("simulation_timestep") = nlm::Constants::DEFAULT_TIMESTEP,
          py::arg("random_seed") = nlm::Constants::SIMULATION_DEFAULT_RANDOM_SEED,
          "Create a brain (backwards compatibility)");
    
    m.def("create_world", &nlm::createWorld,
          "Create a world (backwards compatibility)");
    
    m.def("create_agent", &nlm::createAgent,
          "Create an agent (backwards compatibility)");
    
    // Preset application convenience function
    m.def("apply_preset", &nlm::applyPreset,
          "Apply a preset configuration to an existing config");
    
    // Validation utility
    m.def("validate_config", &nlm::validateConfig,
          "Validate configuration and return errors if any");
    
    // Constants access
    m.attr("DEFAULT_TIMESTEP") = nlm::Constants::DEFAULT_TIMESTEP;
    m.attr("BRAIN_V_THRESH") = nlm::Constants::BRAIN_V_THRESH;
    m.attr("BRAIN_V_REST") = nlm::Constants::BRAIN_V_REST;
    m.attr("BRAIN_V_RESET") = nlm::Constants::BRAIN_V_RESET;
    m.attr("BRAIN_TAU_MEM") = nlm::Constants::BRAIN_TAU_MEM;
    m.attr("BRAIN_TAU_REF") = nlm::Constants::BRAIN_TAU_REF;
    m.attr("BRAIN_INITIAL_WEIGHT_MEAN") = nlm::Constants::DEFAULT_INITIAL_WEIGHT_MEAN;
    m.attr("BRAIN_INITIAL_WEIGHT_STD") = nlm::Constants::DEFAULT_INITIAL_WEIGHT_STD;
    
    // Utility functions
    m.def("get_version", []() { return "NLM Enhanced Python API v1.0"; },
          "Get NLM version information");
    
    m.def("get_available_presets", []() { return nlm::getAvailablePresets(); },
          "Get list of available configuration presets");
    
    m.def("benchmark_constants", []() {
        py::dict bench;
        bench["constants_loaded"] = true;
        bench["memory_usage_kb"] = sizeof(nlm::Constants) / 1024;
        return bench;
    }, "Benchmark constants loading");
    
    // Example scenarios
    m.def("create_learning_scenario", []() {
        auto config = nlm::createDefaultConfig();
        nlm::applyPreset(config, "learning");
        config->set(nlm::ConfigDefaults::BRAIN_NEURON_COUNT, 2000, nlm::ConfigSource::Runtime);
        auto brain = std::make_shared<nlm::Brain>(config);
        return nlm::PythonBrainWrapper(brain);
    }, "Create a learning scenario with enhanced API");
    
    m.def("create_stable_scenario", []() {
        auto config = nlm::createDefaultConfig();
        nlm::applyPreset(config, "stable");
        config->set(nlm::ConfigDefaults::BRAIN_NEURON_COUNT, 500, nlm::ConfigSource::Runtime);
        auto brain = std::make_shared<nlm::Brain>(config);
        return nlm::PythonBrainWrapper(brain);
    }, "Create a stable scenario with enhanced API");
    
    m.def("create_fast_scenario", []() {
        auto config = nlm::createDefaultConfig();
        nlm::applyPreset(config, "fast");
        config->set(nlm::ConfigDefaults::BRAIN_NEURON_COUNT, 100, nlm::ConfigSource::Runtime);
        auto brain = std::make_shared<nlm::Brain>(config);
        return nlm::PythonBrainWrapper(brain);
    }, "Create a fast scenario with enhanced API");
    
    // Error types
    py::register_exception<std::runtime_error>(m, "NLMError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Module documentation
    py::add_module_doc(m, R"pbdoc(
        Enhanced NLM Python API
        ========================
        
        A modern, Pythonic interface to the NLM (Neural Learning Machine) neural simulation framework.
        
        This module provides enhanced APIs for easier and more intuitive usage of NLM,
        including configuration factories, wrapper classes, and preset scenarios.
        
        Key Features:
        - Simplified configuration management with factory functions
        - Enhanced wrapper classes for brain, agent, and world
        - Preset scenarios for common use cases
        - Comprehensive error handling and validation
        - Pythonic API design with fluent builders
        
        Example Usage:
        
        ```python
        import nlm
        
        # Create a brain with preset configuration
        brain = nlm.create_enhanced_brain(preset="learning")
        brain.initialize()
        
        # Run simulation
        brain.run_simulation(1000)
        
        # Get statistics
        stats = brain.get_statistics()
        print(f"Firing neurons: {stats['firing_neurons']}")
        
        # Create world and agent
        world = nlm.create_enhanced_world()
        agent = nlm.create_enhanced_agent(brain)
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Run interaction loop
        for step in range(100):
            world.update_world(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
        ```
    )pbdoc");
}

} // namespace nlm