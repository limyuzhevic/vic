# nlm Python package - core bindings

"""
Core Python bindings for the NLM (Neural Learning Machine) C++ framework.

This module provides the actual Python bindings through pybind11, exposing
the core NLM C++ classes and functionality to Python. This is the bridge that
enables the high-level Python API in __init__.py to work with the underlying
C++ implementation.

The module follows the same patterns as the easy_usage.md examples, providing
familiar Pythonic interfaces while maintaining access to the powerful C++ core.
"""

// C++ includes
#include "python/bindings.cpp"

// Python module initialization
PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
    Core NLM Python Bindings

    This module provides the core Python bindings for the NLM (Neural Learning Machine)
    framework. It exposes the complete C++ neural simulation functionality through
    a Pythonic interface, including brains, agents, worlds, sensory systems,
    motor systems, and all the integrated brain systems from Phase 6.

    The bindings follow the patterns shown in the easy_usage.md documentation,
    providing familiar Python APIs that match the examples in the documentation.
    )pbdoc";

    // Include all the original bindings from bindings.cpp
    // This ensures we have all the classes and functions defined in the original
    pybind11_original_bindings();

    // Register additional convenience functions and enhancements
    register_convenience_functions(m);
    register_documentation_helpers(m);
    register_example_helpers(m);

    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";

    // Register module exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
}

// Convenience functions for common operations
void register_convenience_functions(py::module_ &m) {
    // Create simulation setup (matches easy_usage.md Pattern 1)
    m.def("createSimulationSetup", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(20, 20, 8, 8);
        world->reset();
        
        agent->initialize(*world);
        
        return py::make_tuple(config, brain, world, agent);
    }, "Create a complete simulation setup matching easy_usage.md examples");

    // Enable features convenience function (matches easy_usage.md Pattern 3)
    m.def("enableFeatures", [](std::shared_ptr<AgentBrain> agent,
                              bool reward_modulation,
                              bool structural_plasticity,
                              bool development,
                              bool curiosity) {
        agent->enableRewardModulation(reward_modulation);
        agent->enableStructuralPlasticity(structural_plasticity);
        agent->enableDevelopment(development);
        agent->enableCuriosity(curiosity);
        return agent;
    }, "Enable multiple agent features with a single call", 
       py::arg("agent"), py::arg("reward_modulation"), 
       py::arg("structural_plasticity"), py::arg("development"), 
       py::arg("curiosity"));

    // Run simulation loop (matches easy_usage.md Pattern 1)
    m.def("runSimulationLoop", [](std::shared_ptr<Brain> brain,
                                std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent,
                                uint64_t num_steps,
                                bool verbose) {
        for (uint64_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                auto logger = std::make_shared<ConsoleLogger>(LogLevel::Info);
                Logger::getGlobal().addLogger(logger);
                NLM_LOG_INFO("Step " + std::to_string(step) + ": "
                            "Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        return py::make_dict("total_spikes", brain->getTotalSpikeCount(),
                            "firing_neurons", brain->getFiringNeuronCount(),
                            "avg_firing_rate", brain->getAverageFiringRate());
    }, "Run complete agent simulation loop matching examples", 
       py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("num_steps"), py::arg("verbose"));

    // Quick start examples (matches hard-to-use one-liners in easy_usage.md)
    m.def("quickStartExamples", []() {
        py::dict() = py::dict(
            "silent_brain", py::make_function([]() {
                auto config = std::make_shared<Config>();
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                for (int i =  دائمasize_t(0) i < 10; ++i) {
                    brain->step(i);
                }
                return py::make_tuple(brain->getTotalNeuronCount(),
                                     brain->getTotalSynapseCount());
            }, "Silent brain test matching easy_usage.md Example 1"),
            
            "simple_world_example", py::make_function([]() {
                auto world = std::make_shared<SimpleWorld>();
                world->configure(10, 10, 8, 8);
                world->reset();
                
                auto config = std::make_shared<Config>();
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                auto agent = std::make_shared<AgentBrain>(brain);
                agent->initialize(*world);
                
                return py::make_tuple(config, brain, world, agent);
            }, "Simple world example matching easy_usage.md Example 2"),
            
            "complete_agent_example", py::make_function([]() {
                auto config = std::make_shared<Config>();
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                auto world = std::make_shared<SimpleWorld>();
                world->configure(15, 15, 8, 8);
                world->reset();
                
                auto agent = std::make_shared<AgentBrain>(brain);
                agent->initialize(*world);
                
                agent->enableRewardModulation(True);
                agent->enableCuriosity(True);
                
                for (int step = 0; step < 50; ++step) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    brain->step(step);
                }
                
                return py::make_dict("neurons", brain->getTotalNeuronCount(),
                                    "firing", brain->getFiringNeuronCount(),
                                    "rate", brain->getAverageFiringRate());
            }, "Complete agent example matching easy_usage.md Example 3")
        );
    }, "Get quick start examples that match easy_usage.md patterns");

    // Diagnostic and logging helpers
    m.def("diagnoseBrain", [](std::shared_ptr<Brain> brain) {
        std::stringstream output;
        output << "=== NLM Brain Diagnostics ===\n";
        output << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        output << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        output << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        output << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        output << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        output << "Pending spike events: " << brain->getPendingSpikeEventCount() << "\n";
        output << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        output << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        
        return output.str();
    }, "Generate diagnostic report for brain matching easy_usage.md troubleshooting");

    // Quick stat helpers (matches easy_usage.md Pattern 2)
    m.def("getQuickStats", [](std::shared_ptr<Brain> brain) {
        return py::make_dict(
            "neurons_firing", brain->getFiringNeuronCount(),
            "total_spikes", brain->getTotalSpikeCount(),
            "avg_firing_rate", brain->getAverageFiringRate(),
            "development_stage", static_cast<int>(brain->getDevelopmentalStage()),
            "learning_enabled", true
        );
    }, "Get quick statistics matching easy_usage.md Pattern 2");

    // Backward compatibility helpers (for easy_usage.md compatibility)
    m.def("getFiringNeuronCount", [](std::shared_ptr<Brain> brain) {
        return brain->getFiringNeuronCount();
    }, "Get firing neuron count for easy_usage.md compatibility");

    m.def("getTotalSpikeCount", [](std::shared_ptr<Brain> brain) {
        return brain->getTotalSpikeCount();
    }, "Get total spike count for easy_usage.md compatibility");

    m.def("getAverageFiringRate", [](std::shared_ptr<Brain> brain) {
        return brain->getAverageFiringRate();
    }, "Get average firing rate for easy_usage.md compatibility");

    m.def("getDevelopmentalStage", [](std::shared_ptr<Brain> brain) {
        return static_cast<int>(brain->getDevelopmentalStage());
    }, "Get developmental stage for easy_usage.md compatibility");
}

// Documentation and helper functions
void register_documentation_helpers(py::module_ &m) {
    // Get comprehensive documentation
    m.def("getDocumentation", []() {
        py::dict() = py::dict(
            "version", "0.1.0",
            "description", "NLM Python bindings for brain simulation",
            "author", "NLM Authors",
            "features", py::list({
                "Real LIF neuron dynamics",
                "Event-driven spike propagation", 
                "STDP and Hebbian plasticity",
                "Structural plasticity",
                "Memory systems (working, episodic, associative)",
                "Neuromodulation (dopamine, curiosity, novelty)",
                "Prediction and planning",
                "Concept formation and self-model",
                "Social learning",
                "Development stages",
                "Checkpoint save/load",
                "Replay and consolidation",
            }),
            "examples", py::list({
                "minimal_brain", "complete_agent", "learning_simulation",
                "benchmark", "memory_test", "attention_demo",
            }),
            "quick_reference", py::make_function([]() {
                return """Brain:
  - brain.initialize() : Initialize brain
  - brain.step(step) : Run simulation step
  - brain.reset() : Reset state
  - brain.save("file") : Save checkpoint
  - brain.load("file") : Load checkpoint
  - brain.getTotalNeuronCount() : Get neuron count
  - brain.getFiringNeuronCount() : Get firing neurons
  - brain.getAverageFiringRate() : Get firing rate
  - brain.getDevelopmentalStage() : Get developmental stage

AgentBrain:
  - agent.initialize(world) : Initialize with world
  - agent.processSensoryInput(percept) : Process input
 MuonCommand : AgentMotorCommand()
  - agent.decodeMotorCommand() : Get action
  - agent.applyRewardModulation(reward, predicted) : Apply learning
  - agent.enableRewardModulation(True/False) : Enable features
  - agent.getCuriosityLevel() : Get curiosity
  - agent.getNoveltyLevel() : Get novelty

SimpleWorld:
  - world.configure(w, h, vw, vh) : Configure dimensions
  - world.reset() : Reset world
  - world.update(timestep) : Update world
  - world.getSensoryPercept() : Get perception
  - world.applyMotorCommand(cmd, time) : Apply action
""";
            })
        );
    }, "Get comprehensive documentation for the module");

    // Get example scripts
    m.def("getExamples", []() {
        return py::list({
            "minimal_brain": "Minimal brain test with one-liner (like easy_usage.md Example 1)",
            "simple_world": "Brain in simple world (like easy_usage.md Example 2)",
            "complete_agent": "Complete agent with learning (like easy_usage.md Example 3)",
            "run_simulation": "Complete simulation loop (like easy_usage.md Pattern 1)",
            "get_stats": "Get brain statistics (like easy_usage.md Pattern 2)",
            "enable_features": "Enable agent features (like easy_usage.md Pattern 3)",
        });
    }, "Get available example script names");
}

// Helper functions for creating common scenarios
void register_example_helpers(py::module_ &m) {
    // Create silent brain (matches easy_usage.md Example 1)
    m.def("createSilentBrain", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
        return brain;
    }, "Create silent brain for testing (matches easy_usage.md Example 1)");

    // Create brain-world-agent setup (matches easy_usage.md Example 2)
    m.def("createBrainWorldAgent", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return py::make_tuple(config, brain, world, agent);
    }, "Create brain-world-agent setup (matches easy_usage.md Example 2)");

    // Create complete agent example (matches easy_usage.md Example 3)
    m.def("createCompleteAgentExample", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(True);
        agent->enableCuriosity(True);
        
        for (int step = 0; step < 50; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
        }
        
        return py::make_dict(
            "neurons", brain->getTotalNeuronCount(),
            "firing", brain->getFiringNeuronCount(),
            "rate", brain->getAverageFiringRate()
        );
    }, "Create complete agent example (matches easy_usage.md Example 3)");
}

// Helper to access original pybind11 bindings
void pybind11_original_bindings() {
    // This function exists to ensure all the original pybind11 bindings from
    // the main bindings.cpp file are available. In a real implementation, this
    // would be called during module initialization.
    
    // The actual binding code is in the main bindings.cpp file, which is
    // included at the top of this file. The pybind11_add_module() call in
    // python/CMakeLists.txt will automatically build all the original bindings.
    
    // This empty implementation is here to satisfy the compiler and provide
    // a clear separation between original and enhanced bindings.
    
    // Note: In a real implementation, this would call the actual pybind11
    // binding functions to register all the original classes and methods.
    
    // For now, we rely on the python/CMakeLists.txt to build the original
    // bindings.cpp file, which contains all the essential NLM functionality.
}
