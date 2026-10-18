// Enhanced Python bindings implementation
#include "EnhancedBindings.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <algorithm>
#include <memory>

namespace py = pybind11;
namespace nlm {

// Helper functions for enhanced bindings
static std::shared_ptr<Config> createConfigFromEnhanced(const EnhancedConfig& enhanced) {
    return enhanced.getConfig();
}

static std::shared_ptr<Brain> createBrainFromEnhanced(const EnhancedConfig& enhanced) {
    return std::make_shared<Brain>(enhanced.getConfig());
}

static std::shared_ptr<SimpleWorld> createWorldFromEnhanced() {
    return std::make_shared<SimpleWorld>();
}

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides high-level interfaces for creating, configuring, and running
        sophisticated neural simulations with advanced features.
        
        Key Features:
        - Enhanced configuration management with validation
        - High-level convenience methods for common operations
        - Advanced agent control with curiosity-driven exploration
        - Comprehensive monitoring and statistics
        - Simplified world interaction with object management
        - Multiple configuration presets (basic, advanced, minimal)
    )pbdoc";
    
    // Enhanced Config class
    py::class_<EnhancedConfig>(m, "EnhancedConfig")
        .def(py::init<>())
        .def_static("createBasicConfig", &EnhancedConfig::createBasicConfig,
                   "Create a basic configuration optimized for simple simulations")
        .def_static("createAdvancedConfig", &EnhancedConfig::createAdvancedConfig,
                   "Create an advanced configuration with all features enabled")
        .def_static("createMinimalConfig", &EnhancedConfig::createMinimalConfig,
                   "Create a minimal configuration with only essential systems")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"),
             "Set a string configuration parameter")
        .def("set", py::overload_cast<const std::string&, int64_t>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"),
             "Set an integer configuration parameter")
        .def("set", py::overload_cast<const std::string&, double>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"),
             "Set a double configuration parameter")
        .def("set", py::overload_cast<const std::string&, float>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"),
             "Set a float configuration parameter")
        .def("set", py::overload_cast<const std::string&, bool>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"),
             "Set a boolean configuration parameter")
        .def("has", &EnhancedConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &EnhancedConfig::getKeys,
             "Get all configuration keys")
        .def("getString", &EnhancedConfig::getString, py::arg("key"), py::arg("default_value") = "",
             "Get a string configuration parameter")
        .def("getInt", &EnhancedConfig::getInt, py::arg("key"), py::arg("default_value") = 0,
             "Get an integer configuration parameter")
        .def("getDouble", &EnhancedConfig::getDouble, py::arg("key"), py::arg("default_value") = 0.0,
             "Get a double configuration parameter")
        .def("getFloat", &EnhancedConfig::getFloat, py::arg("key"), py::arg("default_value") = 0.0f,
             "Get a float configuration parameter")
        .def("getBool", &EnhancedConfig::getBool, py::arg("key"), py::arg("default_value") = false,
             "Get a boolean configuration parameter")
        .def("loadFromFile", &EnhancedConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a file")
        .def("saveToFile", &EnhancedConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("summary", &EnhancedConfig::summary,
             "Get a summary of the configuration")
        
        // Convenience methods
        .def("enableAllSystems", &EnhancedConfig::enableAllSystems,
             "Enable all NLM subsystems (learning, development, neuromodulation, prediction, cognition)")
        .def("disableAllSystems", &EnhancedConfig::disableAllSystems,
             "Disable all NLM subsystems")
        .def("enableLearning", &EnhancedConfig::enableLearning, py::arg("enable") = true,
             "Enable or disable learning subsystems")
        .def("enableDevelopment", &EnhancedConfig::enableDevelopment, py::arg("enable") = true,
             "Enable or disable developmental processes")
        .def("enableNeuromodulation", &EnhancedConfig::enableNeuromodulation, py::arg("enable") = true,
             "Enable or disable neuromodulation systems")
        .def("enablePrediction", &EnhancedConfig::enablePrediction, py::arg("enable") = true,
             "Enable or disable prediction systems")
        .def("enableCognition", &EnhancedConfig::enableCognition, py::arg("enable") = true,
             "Enable or disable cognitive systems")
        
        // Configuration builders
        .def("configureLearning", [](EnhancedConfig& self, bool enable, float rate) {
            return self.enableLearning(enable).set("learning.rate", rate);
        }, py::arg("enable") = true, py::arg("rate") = 0.001f,
             "Configure learning subsystem")
        .def("configureDevelopment", [](EnhancedConfig& self, bool enable, float plasticity) {
            return self.enableDevelopment(enable).set("development.plasticity", plasticity);
        }, py::arg("enable") = true, py::arg("plasticity") = 1.0f,
             "Configure development subsystem")
        .def("configureNeuromodulation", [](EnhancedConfig& self, bool enable, float curiosity) {
            return self.enableNeuromodulation(enable).set("neuromodulation.curiosity", curiosity);
        }, py::arg("enable") = true, py::arg("curiosity") = 1.0f,
             "Configure neuromodulation subsystem")
        .def("configureMemory", [](EnhancedConfig& self, size_t workingCapacity, size_t episodicMax) {
            return self.set("memory.working.capacity", workingCapacity)
                     .set("memory.episodic.max_episodes", episodicMax);
        }, py::arg("working_capacity"), py::arg("episodic_max"),
             "Configure memory systems")
        ;
    
    // Enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &EnhancedBrain::initialize,
             "Initialize the enhanced brain with configuration")
        .def("step", &EnhancedBrain::step, py::arg("steps") = 1,
             "Advance the brain simulation by specified number of steps")
        
        // Statistics
        .def("getStats", &EnhancedBrain::getStats,
             "Get comprehensive statistics about the brain")
        .def_readonly("stats", &EnhancedBrain::getStats, "Brain statistics (read-only)")
        
        // Learning and development controls
        .def("enableLearning", &EnhancedBrain::enableLearning, py::arg("enable") = true,
             "Enable or disable learning subsystems")
        .def("enableDevelopment", &EnhancedBrain::enableDevelopment, py::arg("enable") = true,
             "Enable or disable developmental processes")
        .def("enableNeuromodulation", &EnhancedBrain::enableNeuromodulation, py::arg("enable") = true,
             "Enable or disable neuromodulation systems")
        .def("setCuriosity", &EnhancedBrain::setCuriosity, py::arg("level"),
             "Set curiosity level for exploration behavior (0.0-1.0)")
        .def("setLearningRate", &EnhancedBrain::setLearningRate, py::arg("rate"),
             "Set learning rate for plasticity rules (0.0-1.0)")
        
        // Checkpoint and persistence
        .def("save", &EnhancedBrain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        .def("load", &EnhancedBrain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Experiment control
        .def("runExperiment", &EnhancedBrain::runExperiment, py::arg("type"), py::arg("steps"),
             "Run a predefined experiment of the specified type for given steps")
        
        // Underlying brain access (advanced)
        .def("getBrain", &EnhancedBrain::getBrain,
             "Get the underlying C++ Brain object (advanced access)")
        ;
    
    // Enhanced SimpleWorld class
    py::class_<EnhancedSimpleWorld>(m, "EnhancedSimpleWorld")
        .def(py::init<>())
        
        // Configuration
        .def("configure", py::overload_cast<size_t, size_t, size_t, size_t>(&EnhancedSimpleWorld::configure),
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("configure", py::overload_cast<size_t, size_t, size_t, size_t, size_t, float, float>(&EnhancedSimpleWorld::configure),
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             py::arg("max_objects"), py::arg("energy_decay_rate") = 0.01f, py::arg("max_energy") = 100.0f,
             "Configure world with advanced options")
        
        // Environment setup
        .def("reset", &EnhancedSimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &EnhancedSimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f,
             "Set agent starting position and orientation")
        .def("setRandomSeed", &EnhancedSimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        
        // World update
        .def("update", &EnhancedSimpleWorld::update, py::arg("timestep"), py::arg("apply_physics") = true,
             "Update world simulation (optionally apply physics)")
        
        // Agent control
        .def("applyMotorCommand", &EnhancedSimpleWorld::applyMotorCommand, py::arg("command"), py::arg("time"),
             "Apply motor command to world")
        
        // Environment objects
        .def("addResource", &EnhancedSimpleWorld::addResource, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a resource object to the world")
        .def("addHazard", &EnhancedSimpleWorld::addHazard, py::arg("x"), py::arg("y"), py::arg("severity") = 1.0f,
             "Add a hazard object to the world")
        
        // World query
        .def("getVision", &EnhancedSimpleWorld::getVision,
             "Get current vision percept as vector of floats")
        .def("getTouch", &EnhancedSimpleWorld::getTouch,
             "Get current touch percept as vector of floats")
        .def("getAgentBody", py::overload_cast<const AgentBody&>(&EnhancedSimpleWorld::getAgentBody),
             "Get agent body state (read-only)")
        .def("getAgentBody", py::overload_cast<AgentBody&>(&EnhancedSimpleWorld::getAgentBody),
             "Get agent body state (writable)")
        .def("getSimulationTime", &EnhancedSimpleWorld::getSimulationTime,
             "Get current simulation time")
        
        // Enhanced agent brain interface
        .def("createAgentBrain", &EnhancedSimpleWorld::createAgentBrain,
             "Create an enhanced agent brain interface")
        ;
    
    // Enhanced AgentBrain class (nested in EnhancedSimpleWorld)
    // This is exposed through EnhancedSimpleWorld::createAgentBrain
    
    // Convenience functions
    m.def("createEnhancedConfig", &createEnhancedConfig, py::arg("type") = "basic",
          "Create an enhanced configuration of the specified type (basic, advanced, minimal)")
    ;
    
    // Submodules
    py::module_ experiment = m.def_submodule("experiment", "NLM experiment utilities");
    experiment.def("runBasic", [](const EnhancedConfig& config, size_t steps) {
        auto brain = createBrainFromEnhanced(config);
        brain->initialize();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
    }, py::arg("config"), py::arg("steps"),
       "Run a basic simulation experiment");
    
    experiment.def("runLearning", [](const EnhancedConfig& config, size_t steps) {
        auto brain = createBrainFromEnhanced(config);
        brain->initialize();
        
        // Enhanced learning experiment
        for (size_t i = 0; i < steps; ++i) {
            // Apply learning stimuli
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(i, i * 0.001);
        }
    }, py::arg("config"), py::arg("steps"),
       "Run a learning experiment with plasticity");
    
    experiment.def("runDevelopment", [](const EnhancedConfig& config, size_t steps) {
        auto brain = createBrainFromEnhanced(config);
        brain->initialize();
        
        // Development simulation
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
            brain->develop();
        }
    }, py::arg("config"), py::arg("steps"),
       "Run a developmental simulation experiment");
    
    // Version information
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with high-level interfaces";
    
    // Error types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

} // namespace nlm
