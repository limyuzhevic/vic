#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a comprehensive Python interface to the NLM neural simulation
        framework, enabling intuitive brain simulation with support for learning, memory,
        and development systems.
        
        Key Features:
        - Type conversion utilities (to_float, to_int)
        - Convenience factory methods
        - Context managers for brain lifecycle
        - Simulation convenience functions
        - Enhanced property accessors
        - World object creation utilities
        - Configuration management tools
    )pbdoc";

    m.attr("__version__") = "1.0.0";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Type conversion utilities for Python-friendly type handling
    m.def("to_float", [](const py::object& obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<float>();
        } else if (py::isinstance<py::float_>(obj)) {
            return obj.cast<float>();
        }
        throw py::type_error("Object cannot be converted to float");
    }, py::arg("obj"), "Convert Python int/float to C++ float");

    m.def("to_int", [](const py::object& obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<int64_t>();
        } else if (py::isinstance<py::float_>(obj)) {
            return static_cast<int64_t>(obj.cast<double>());
        }
        throw py::type_error("Object cannot be converted to int");
    }, py::arg("obj"), "Convert Python int/float to C++ int");

    m.def("round_float", [](float value, int decimals) {
        return std::round(value * std::pow(10.0f, decimals)) / std::pow(10.0f, decimals);
    }, py::arg("value"), py::arg("decimals") = 2, "Round float to specified number of decimal places");

    // Convenience factory methods for common use cases
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("create_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Configuration convenience methods
    m.def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_config_from_file", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Create and load configuration from file");

    m.def("create_config_from_args", [](int argc, char** argv) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromArgs(argc, argv)) {
            throw std::runtime_error("Failed to load configuration from command line arguments");
        }
        return config;
    }, py::arg("argc"), py::arg("argv"), "Create and load configuration from command line arguments");

    // Simulation convenience functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int num_steps) {
        brain->initialize();
        for (int i = 0; i < num_steps; ++i) {
            brain->step(i);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("num_steps"), "Run a complete simulation and return spike count");

    m.def("run_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                          std::shared_ptr<AgentBrain> agent, int num_steps) {
        // Initialize agent with world
        agent->initialize(*world);
        
        // Enable default subsystems for learning
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        for (int i = 0; i < num_steps; ++i) {
            // Update world
            world->update(0.1);
            
            // Get sensory input from world
            const auto& percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(i);
            
            // Decode and apply motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, world->getSimulationTime());
            
            // Update development system
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel(),
            agent->getNeuromodulationLevel(),
            brain->getDevelopmentalStage()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), "Run a complete agent episode and return results");

    // Context manager for brain lifecycle
    class BrainContextManager {
    public:
        BrainContextManager(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {}
        
        std::shared_ptr<Brain> enter() {
            if (brain_) {
                brain_->initialize();
            }
            return brain_;
        }
        
        void exit(const py::object& exc_type, const py::object& exc_value, const py::object& traceback) {
            // Context manager exit - cleanup if needed
            if (brain_) {
                brain_->reset();
            }
        }
        
        std::shared_ptr<Brain> get_brain() const {
            return brain_;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };

    // Context manager helper function
    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return BrainContextManager(brain);
    }, py::arg("brain"), "Create a context manager for brain lifecycle management");

    // World configuration helper
    m.def("configure_world", [](std::shared_ptr<SimpleWorld> world, 
                               float width, float height,
                               size_t vision_width, size_t vision_height) {
        world->configure(width, height, vision_width, vision_height);
        return world;
    }, py::arg("world"), py::arg("width"), py::arg("height"), 
       py::arg("vision_width"), py::arg("vision_height"), "Configure world parameters");

    // Legacy function names for backward compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Development system controls
    m.def("enable_development", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableDevelopment(enable);
    }, py::arg("agent"), py::arg("enable"), "Enable/disable development system");

    m.def("enable_curiosity", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableCuriosity(enable);
    }, py::arg("agent"), py::arg("enable"), "Enable/disable curiosity system");

    m.def("enable_reward_modulation", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableRewardModulation(enable);
    }, py::arg("agent"), py::arg("enable"), "Enable/disable reward modulation");

    m.def("enable_structural_plasticity", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableStructuralPlasticity(enable);
    }, py::arg("agent"), py::arg("enable"), "Enable/disable structural plasticity");

    // Simulation statistics helper
    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        return std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getActiveNeuronCount(),
            brain->getFiringNeuronCount(),
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            brain->getExcitationInhibitionRatio(),
            brain->getDevelopmentalStage()
        );
    }, py::arg("brain"), "Get comprehensive brain statistics as a tuple");

    // Legacy function names (maintaining backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("PI") = 3.14159265358979323846;
    m.attr("E") = 2.71828182845904523536;

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Default configuration values
    m.def("get_default_config", []() {
        auto config = std::make_shared<Config>();
        // Set some reasonable defaults based on typical brain parameters
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.tau_mem", 20.0);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
        return config;
    }, "Get a configuration with recommended default values");

    // World object factory methods
    m.def("create_resource", [](float x, float y, float value) {
        WorldObject obj(x, y, WorldObjectType::Resource, value);
        return obj;
    }, py::arg("x"), py::arg("y"), py::arg("value"), "Create a resource object");

    m.def("create_hazard", [](float x, float y, float value) {
        WorldObject obj(x, y, WorldObjectType::Hazard, value);
        return obj;
    }, py::arg("x"), py::arg("y"), py::arg("value"), "Create a hazard object");

    m.def("create_wall", [](float x, float y, float radius) {
        WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
        return obj;
    }, py::arg("x"), py::arg("y"), py::arg("radius"), "Create a wall object");

    m.def("create_marker", [](float x, float y, float value) {
        WorldObject obj(x, y, WorldObjectType::Marker, value);
        return obj;
    }, py::arg("x"), py::arg("y"), py::arg("value"), "Create a marker object");
}

} // namespace nlm
