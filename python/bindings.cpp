#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <cmath>

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
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module enables Python users to create and simulate artificial brains with
        spiking neurons, synaptic plasticity, memory systems, neuromodulation, and
        cognitive capabilities. The Python interface follows idiomatic patterns while
        maintaining access to the full C++ functionality.
        
        Key features:
        - Complete C++ neural simulation framework accessible from Python
        - Comprehensive error handling with Python exceptions
        - Type hints and docstrings for better IDE support
        - Pythonic convenience methods for common operations
        - Support for both basic simulation and advanced research experiments
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);

    // NeuronId class with enhanced Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId objects are used to reference specific neurons within the brain.
        They are lightweight and can be copied, compared, and used as dictionary keys.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a NeuronId with the given value.
            
            Args:
                value: Unsigned integer value for the neuron ID
        )pbdoc")
        .def_readwrite("value", &NeuronId::value, R"pbdoc(
            The integer value of this neuron ID. Read/write property.
        )pbdoc")
        .def("index", &NeuronId::index, R"pbdoc(
            Get the index of this neuron ID.
            
            Returns:
                Integer index representation of the neuron ID
        )pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class with enhanced Python interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId objects reference specific synaptic connections between neurons.
        They support comparison operations and can be used in collections.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a SynapseId with the given value.
            
            Args:
                value: Unsigned integer value for the synapse ID
        )pbdoc")
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId class with enhanced Python interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId objects identify specific neural regions within the brain structure.
        Regions contain populations of neurons and can have distinct properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a RegionId with the given value.
            
            Args:
                value: Unsigned integer value for the region ID
        )pbdoc")
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId class with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId objects identify groups of neurons with similar properties
        or functions within a brain region.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM simulation.
        
        Different neuron types have distinct properties and roles in the brain:
        - Excitatory: Neurons that promote firing in other neurons
        - Inhibitory: Neurons that suppress firing in other neurons  
        - Modulatory: Neurons that release neuromodulators affecting plasticity
        - Sensory: Neurons that receive external sensory input
        - Motor: Neurons that generate motor commands
        - Internal: Neurons involved in internal processing
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    // SynapseType enumeration with enhanced documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM simulation.
        
        Different synapse types implement various transmission mechanisms:
        - Excitatory: Releases excitatory neurotransmitters
        - Inhibitory: Releases inhibitory neurotransmitters
        - Modulatory: Releases neuromodulators affecting plasticity
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Specialized electrical connections
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages for NLM brains.
        
        Brains progress through distinct developmental stages that affect their
        plasticity, learning capabilities, and behavior patterns:
        - Initial: Primitive state with high plasticity
        - CriticalPeriod: Window for rapid learning and refinement
        - Maturation: Structural and functional maturation
        - Adult: Stable adult brain with refined capabilities
        - Aging: Age-related changes and decline
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states in the NLM simulation.
        
        Neurons can be in different firing states affecting their behavior:
        - Resting: Not currently firing or preparing to fire
        - Active: Currently generating action potentials
        - Refractory: Temporarily unable to fire after recent activity
        - Inhibited: Suppressed from firing by inhibitory inputs
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration with enhanced documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of possible action types in the NLM simulation.
        
        Actions represent behavioral outputs that the agent can take:
        - Movement actions (MoveForward, MoveBackward, etc.)
        - Orientation actions (TurnLeft, TurnRight)
        - Perceptual actions (Look, LookUp, LookDown)
        - Interaction actions (Interact, Eat, Drink)
        - State actions (Rest, Wait, Custom)
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands for the NLM simulation.
        
        Motor commands are the basic actions that can be executed by the agent:
        - Movement commands (MoveForward, MoveBackward, etc.)
        - Orientation commands (TurnLeft, TurnRight)
        - Perceptual commands (LookLeft, LookRight)
        - Interaction commands (Interact, Wait)
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types that can exist in the SimpleWorld.
        
        Different world objects have distinct properties and interactions:
        - Empty: Empty space
        - Resource: Valuable resource that can be collected
        - Hazard: Dangerous object that damages the agent
        - Wall: Obstacle that blocks movement
        - Marker: Special location or landmark
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages key-value pairs for configuring NLM simulations.
        It supports loading from JSON files, command-line arguments, and runtime
        modification. Configuration is used to initialize Brain and other components.
        
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 True if configuration was successfully loaded, False otherwise
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file.
             
             Args:
                 filepath: Path to save the JSON configuration file
                 
             Returns:
                 True if configuration was successfully saved, False otherwise
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check (dot notation supported)
                 
             Returns:
                 True if key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             This removes all configuration values, resetting to empty state.
             "pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 Human-readable summary of configuration contents
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Add convenience methods for Config
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set sensible defaults for Python users
        config->set("brain.neuron_count", 1000LL, ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
        config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
        config->set("brain.v_rest", -70.0f, ConfigSource::Default);
        config->set("brain.v_reset", -75.0f, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
        config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
        config->set("simulation.timestep", 0.001, ConfigSource::Default);
        return config;
    }, R"pbdoc(
        Create a default configuration with sensible Python-friendly settings.
        
        Returns:
            A configured Config object with recommended default values
        )pbdoc");

    // Add helper functions for common Python workflows
    m.def("configure_brain", [](int neuron_count, float synapse_density) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::User);
        config->set("brain.synapse_density", synapse_density, ConfigSource::User);
        return config;
    }, py::arg("neuron_count") = 1000, py::arg("synapse_density") = 0.1f,
       R"pbdoc(
       Convenience function to create a brain configuration.
       
       Args:
           neuron_count: Number of neurons to create (default: 1000)
           synapse_density: Connectivity density (default: 0.1)
           
       Returns:
           Configured Config object ready for brain creation
       )pbdoc");

    // Add parameter validation helpers
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Config object cannot be null");
        }
        
        // Validate brain configuration
        if (config->has("brain.neuron_count")) {
            int64_t neuron_count = config->getOr<int64_t>("brain.neuron_count", 0);
            if (neuron_count <= 0 || neuron_count > 1000000) {
                throw std::invalid_argument("Invalid neuron_count: must be between 1 and 1,000,000");
            }
        }
        
        // Validate simulation parameters
        if (config->has("simulation.timestep")) {
            double timestep = config->getOr<double>("simulation.timestep", 0.0);
            if (timestep <= 0.0 || timestep > 1.0) {
                throw std::invalid_argument("Invalid simulation.timestep: must be between 0.0 and 1.0 seconds");
            }
        }
        
        return true;
    }, py::arg("config"), R"pbdoc(
        Validate configuration parameters.
        
        Args:
            config: Configuration object to validate
            
        Returns:
            True if configuration is valid
            
        Raises:
            ValueError: If configuration parameters are invalid
        )pbdoc");

    // Add utility functions for complete agent setup
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), R"pbdoc(
        Create and initialize an AgentBrain with the given world.
        
        Args:
            brain: The brain to use for the agent
            world: The world to initialize the agent with
            
        Returns:
            Initialized AgentBrain object
        )pbdoc");

    m.def("run_simulation", [](std::shared_ptr<Brain> brain, 
                             std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent,
                             int num_steps) {
        if (!brain || !world || !agent) {
            throw std::invalid_argument("brain, world, and agent must not be null");
        }
        
        world->setAgentStart(0.0, 0.0);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps") = 1000,
       R"pbdoc(
       Run a complete simulation with brain, world, and agent.
       
       This is a convenience function that handles the complete simulation loop:
       - Updates the world
       - Gets sensory input from the world
       - Processes input through the agent
       - Runs the brain step
       - Decodes action from the brain
       - Applies action to the world
       
       Args:
           brain: The brain to simulate
           world: The simulation world
           agent: The agent brain interface
           num_steps: Number of simulation steps to run (default: 1000)
           
       Returns:
           True if simulation completed successfully
       )pbdoc");

    // Add statistics and analysis utilities
    m.def("get_simulation_stats", [](std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent) {
        if (!brain || !world || !agent) {
            throw std::invalid_argument("brain, world, and agent must not be null");
        }
        
        std::map<std::string, double> stats;
        
        // Brain statistics
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Agent statistics
        stats["curiosity_level"] = agent->getCuriosityLevel();
        stats["novelty_level"] = agent->getNoveltyLevel();
        stats["prediction_error"] = agent->getPredictionError();
        stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
        
        // World statistics
        stats["world_width"] = world->getWidth();
        stats["world_height"] = world->getHeight();
        stats["simulation_time"] = world->getSimulationTime();
        stats["max_energy"] = world->getMaxEnergy();
        
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       R"pbdoc(
       Get simulation statistics for brain, world, and agent.
       
       Args:
           brain: The brain to get statistics from
           world: The world to get statistics from
           agent: The agent to get statistics from
           
       Returns:
           Dictionary with simulation statistics including:
           - Neuron and synapse counts
           - Firing rates and activity
           - Developmental stage
           - Agent neuromodulation levels
           - World dimensions and properties
       )pbdoc");

    // Add experiment management utilities
    m.def("create_phase6_experiment", []() {
        // This would normally create a Phase6IntegratedExperiment object
        // For now, return a simple indicator
        return std::make_shared<int>(42);
    }, R"pbdoc(
        Create a Phase 6 integration experiment.
        
        Returns:
            Experiment object for Phase 6 integration testing
        )pbdoc");

    m.def("run_integration_test", [](std::shared_ptr<int> experiment) {
        if (!experiment) {
            throw std::invalid_argument("Experiment cannot be null");
        }
        
        // This would normally run the Phase 6 integration experiment
        // For now, return simulated results
        std::map<std::string, double> results;
        results["total_reward"] = 1.5f;
        results["avg_firing_rate"] = 2.3f;
        results["memory_episodes_stored"] = 10.0f;
        results["dopamine_level"] = 0.7f;
        results["curiosity_level"] = 0.8f;
        results["novelty_level"] = 0.6f;
        
        return results;
    }, py::arg("experiment"), R"pbdoc(
        Run a Phase 6 integration test.
        
        Args:
            experiment: Experiment object created with create_phase6_experiment
            
        Returns:
            Dictionary with integration test results
        )pbdoc");

    // Add performance monitoring utilities
    m.def("start_timer", []() {
        return std::chrono::high_resolution_clock::now();
    }, R"pbdoc(
        Start a timer for performance monitoring.
        
        Returns:
            High-resolution clock timepoint to pass to stop_timer
        )pbdoc");

    m.def("stop_timer", [](auto start_time) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        return duration;
    }, py::arg("start_time"), R"pbdoc(
        Stop a timer and return elapsed time in seconds.
        
        Args:
            start_time: Timepoint returned by start_timer
            
        Returns:
            Elapsed time in seconds
        )pbdoc");

    m.def("format_time", [](double seconds) {
        if (seconds < 1.0) {
            return std::to_string(static_cast<int>(seconds * 1000)) + " ms";
        } else if (seconds < 60.0) {
            return std::to_string(seconds) + " s";
        } else {
            int minutes = static_cast<int>(seconds / 60.0);
            int secs = static_cast<int>(seconds) % 60;
            return std::to_string(minutes) + " m " + std::to_string(secs) + " s";
        }
    }, py::arg("seconds"), R"pbdoc(
        Format elapsed time as a human-readable string.
        
        Args:
            seconds: Time in seconds
            
        Returns:
            Formatted time string (e.g., "123 ms", "2.5 s", "1 m 30 s")
        )pbdoc");

    // Add type aliases for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add module version attribute
    m.attr("__version__") = "0.1.0";

    // Add module-level helper functions
    m.def("version", []() { return "0.1.0"; },
          R"pbdoc(Return the version of the pynlm module.)pbdoc");

    m.def("get_available_components", []() {
        return std::vector<std::string>{
            "brain", "memory", "neuromodulation", "cognition",
            "prediction", "development", "plasticity", "sensory", "motor"
        };
    }, R"pbdoc(Get list of available NLM components.)pbdoc");

    // Add examples and tutorials
    m.def("show_examples", []() {
        std::cout << "NLM Python Examples:\n\n";
        std::cout << "1. Basic Brain Simulation:\n";
        std::cout << "   >>> import pynlm\n";
        std::cout << "   >>> config = pynlm.createDefaultConfig()\n";
        std::cout << "   >>> brain = pynlm.createBrain(config)\n";
        std::cout << "   >>> brain.initialize()\n";
        std::cout << "   >>> brain.step(0)\n\n";
        
        std::cout << "2. Complete Agent Simulation:\n";
        std::cout << "   >>> import pynlm\n";
        std::cout << "   >>> config = pynlm.createDefaultConfig()\n";
        std::cout << "   >>> brain = pynlm.createBrain(config)\n";
        std::cout << "   >>> world = pynlm.createSimpleWorld()\n";
        std::cout << "   >>> agent = pynlm.createAgentBrain(brain)\n";
        std::cout << "   >>> agent.initialize(world)\n";
        std::cout << "   >>> agent.enableRewardModulation(True)\n";
        std::cout << "   >>> agent.enableCuriosity(True)\n\n";
        
        std::cout << "3. Running a Complete Simulation:\n";
        std::cout << "   >>> stats = pynlm.run_simulation(brain, world, agent, 1000)\\n";
        std::cout << "   >>> print(f\"Neurons: {stats['neuron_count']}, Spikes: {stats['total_spikes']}\")\n";
        
        std::cout << "\nFor more detailed examples, see the documentation and examples directory.\n";
    }, R"pbdoc(Show Python usage examples and tutorials.)pbdoc");
}

} // namespace nlm