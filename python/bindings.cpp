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

// Type aliases for better Python readability
typedef NeuronId neuron_id;
typedef SynapseId synapse_id;
typedef RegionId region_id;
typedef PopulationId population_id;
typedef ActionType action_type;
typedef MotorCommand motor_command;
typedef WorldObjectType world_object_type;
typedef DevelopmentalStage developmental_stage;
typedef FiringState firing_state;
typedef NeuronType neuron_type;
typedef SynapseType synapse_type;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Forward declarations for helper functions
    void register_types(py::module_& m);
    void register_enums(py::module_& m);
    void register_config(py::module_& m);
    void register_sensory(py::module_& m);
    void register_action(py::module_& m);
    void register_world(py::module_& m);
    void register_brain(py::module_& m);
    void register_agent(py::module_& m);
    void register_factory_functions(py::module_& m);

    // Register components in logical order
    register_types(m);
    register_enums(m);
    register_config(m);
    register_sensory(m);
    register_action(m);
    register_world(m);
    register_brain(m);
    register_agent(m);
    register_factory_functions(m);

    // Convenience functions and utilities
    m.def("run_simulation", &run_simulation, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          "Run a complete agent simulation with brain, world, and agent integration.");

    m.def("train_agent", &train_agent, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("episodes"), py::arg("steps_per_episode"),
          "Train an agent using reinforcement learning for a specified number of episodes.");

    m.def("create_agent_config", &create_agent_config, "Create a configuration optimized for agent-based tasks.");

    m.def("create_rendering_config", &create_rendering_config, "Create a configuration optimized for visualization.");

    // Performance monitoring and debugging
    m.def("benchmark_brain", &benchmark_brain, py::arg("brain"), py::arg("steps"), py::arg("warmup"),
          "Benchmark brain performance with specified warmup and test steps.");

    m.def("profile_simulation", &profile_simulation, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          "Profile simulation performance and return timing metrics.");

    // Checkpoint utilities
    m.def("save_checkpoint", &save_checkpoint, py::arg("brain"), py::arg("path"), py::arg("world"),
          "Save complete simulation state (brain + world) to checkpoint file.");

    m.def("load_checkpoint", &load_checkpoint, py::arg("brain"), py::arg("path"), py::arg("world"),
          "Load complete simulation state from checkpoint file.");

    // Advanced exploration and analysis
    m.def("explore_states", &explore_states, py::arg("brain"), py::arg("count"),
          "Explore different neural states and return samples for analysis.");

    m.def("analyze_behavior", &analyze_behavior, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          "Analyze agent behavior patterns and return statistical summaries.");

    // Utility functions
    m.def("get_version", []() { return "NLM Python Bindings v1.0.0"; },
          "Get version information for the Python bindings.");

    m.def("get_default_config", &get_default_config, "Get the complete default configuration dictionary.");

    m.def("validate_config", &validate_config, py::arg("config"),
          "Validate configuration and return list of any errors or warnings.");

    m.def("format_config", &format_config, py::arg("config"), py::arg("indent"),
          "Format configuration dictionary with specified indentation.");

    m.def("deepcopy_brain", &deepcopy_brain, py::arg("brain"),
          "Create a deep copy of a brain object (useful for state restoration).");

    // Context managers for resource management
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(
        Context manager for safe simulation execution.
        Automatically handles initialization, cleanup, and error recovery.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("__enter__", &SimulationContext::enter)
        .def("__exit__", &SimulationContext::exit)
        .def("run", &SimulationContext::run, py::arg("steps"), "Run simulation steps within context")
        .def("get_stats", &SimulationContext::get_stats, "Get statistics about the simulation");

    py::class_<ConfigContext>(m, "ConfigContext", R"pbdoc(
        Context manager for safe configuration modification.
        Automatically reverts changes on exit if requested.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", &ConfigContext::enter)
        .def("__exit__", &ConfigContext::exit)
        .def("set", &ConfigContext::set, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("apply_preset", &ConfigContext::apply_preset, py::arg("preset"), "Apply configuration preset");
}

// Helper function implementations
void register_types(py::module_& m) {
    py::class_<neuron_id>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &neuron_id::value)
        .def("index", &neuron_id::index)
        .def("__eq__", &neuron_id::operator==)
        .def("__ne__", &neuron_id::operator!=)
        .def("__hash__", [](const neuron_id& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const neuron_id& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // Similar for other type aliases...
}

void register_enums(py::module_& m) {
    py::enum_<neuron_type>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Register all other enums...
}

void register_config(py::module_& m) {
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Convenience methods
        .def("get", [](Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), "Get configuration value by key")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("get_int", [](Config& self, const std::string& key, int default_value = 0) {
            return self.get(key, default_value);
        }, py::arg("key"), py::arg("default_value") = 0, "Get integer configuration value")
        .def("get_float", [](Config& self, const std::string& key, float default_value = 0.0f) {
            return self.get(key, default_value);
        }, py::arg("key"), py::arg("default_value") = 0.0f, "Get float configuration value")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value = "") {
            return self.get(key, default_value);
        }, py::arg("key"), py::arg("default_value") = "", "Get string configuration value")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value = false) {
            return self.get(key, default_value);
        }, py::arg("key"), py::arg("default_value") = false, "Get boolean configuration value");
}

// Helper functions implementations
std::shared_ptr<Brain> run_simulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    int steps) {
    if (!brain || !world || !agent) {
        throw std::runtime_error("Invalid arguments: brain, world, and agent must be provided");
    }
    brain->initialize();
    agent->initialize(*world);
    for (int step = 0; step < steps; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    return brain;
}

std::shared_ptr<Brain> train_agent(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    int episodes,
    int steps_per_episode) {
    if (!brain || !world || !agent) {
        throw std::runtime_error("Invalid arguments: brain, world, and agent must be provided");
    }
    brain->initialize();
    agent->initialize(*world);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);

    for (int episode = 0; episode < episodes; ++episode) {
        world->reset();
        agent->reset();
        for (int step = 0; step < steps_per_episode; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            auto reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0);
        }
    }
    return brain;
}

std::shared_ptr<Config> create_agent_config() {
    auto config = std::make_shared<Config>();
    // Set agent-specific defaults
    config->set("brain.neuron_count", 2000);
    config->set("brain.synapse_density", 0.15);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.dopamine.scale", 1.5);
    config->set("neuromod.curiosity.enable", true);
    return config;
}

std::shared_ptr<Config> create_rendering_config() {
    auto config = std::make_shared<Config>();
    // Set rendering/visualization defaults
    config->set("brain.neuron_count", 500);
    config->set("visualization.enabled", true);
    config->set("visualization.render_rate", 30);
    return config;
}

// Additional helper functions would be implemented here...

} // namespace nlm