#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <random>

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

// Custom exception classes for better error handling
class NLMConfigError : public std::runtime_error {
public:
    explicit NLMConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMBrainError : public std::runtime_error {
public:
    explicit NLMBrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMInputError : public std::runtime_error {
public:
    explicit NLMInputError(const std::string& msg) : std::runtime_error(msg) {}
};

// Convenience factory functions and advanced APIs
namespace factory {
    // Create a complete agent setup with common configuration
    std::tuple<std::shared_ptr<Config>, std::shared_ptr<Brain>, 
               std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>
    createCompleteAgent(const std::string& configFile = "") {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            if (!config->loadFromFile(configFile)) {
                throw NLMConfigError("Failed to load config file: " + configFile);
            }
        }
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Standard configuration for complete agents
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.v_thresh", -50.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.tau_mem", 20.0f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(config, brain, agent, world);
    }
    
    // Create a learning agent with reinforcement learning capabilities
    std::tuple<std::shared_ptr<Config>, std::shared_ptr<Brain>, 
               std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>
    createLearningAgent(size_t epochs = 1000, float learningRate = 0.01f) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure for learning
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("plasticity.stdp.learning_rate", learningRate);
        config->set("plasticity.hebbian.learning_rate", learningRate);
        config->set("neuromod.curiosity.strength", 0.5f);
        
        world->configure(15, 15, 6, 6);
        world->setAgentStart(7.5, 7.5);
        
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(config, brain, agent, world);
    }
    
    // Create a perception-only agent (for testing sensory processing)
    std::tuple<std::shared_ptr<Config>, std::shared_ptr<Brain>, 
               std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>
    createPerceptionAgent() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure for perception focus
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05f);
        config->set("brain.v_thresh", -40.0f);  // Higher threshold for less firing
        
        world->configure(10, 10, 4, 4);
        world->setAgentStart(5.0, 5.0);
        
        agent->initialize(*world);
        
        // Disable learning features for pure perception
        agent->enableRewardModulation(false);
        agent->enableStructuralPlasticity(false);
        agent->enableDevelopment(false);
        agent->enableCuriosity(false);
        
        return std::make_tuple(config, brain, agent, world);
    }
}

// Utility functions for data analysis and visualization
namespace utils {
    // Analyze brain statistics and return a summary
    std::string analyzeBrain(const Brain& brain) {
        std::stringstream ss;
        ss << "=== Brain Analysis ===\n";
        ss << "Neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "Synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
        ss << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        ss << "Total Spikes: " << brain.getTotalSpikeCount() << "\n";
        ss << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
        ss << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        return ss.str();
    }
    
    // Run a simulation episode and collect metrics
    struct EpisodeMetrics {
        double totalReward;
        size_t totalSteps;
        size_t finalFiringRate;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        DevelopmentalStage developmentalStage;
        
        EpisodeMetrics() : totalReward(0.0), totalSteps(0), finalFiringRate(0), 
                         curiosityLevel(0.0f), noveltyLevel(0.0f), dopamineLevel(0.0f),
                         developmentalStage(DevelopmentalStage::Initial) {}
    };
    
    EpisodeMetrics runEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                              std::shared_ptr<SimpleWorld> world, size_t maxSteps) {
        EpisodeMetrics metrics;
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (size_t step = 0; step < maxSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from internal signals
            auto internal = percept.getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            metrics.totalReward += reward;
            metrics.totalSteps = step + 1;
            
            if (step % 100 == 0) {
                agent->updateDevelopment(0.1);
            }
        }
        
        metrics.finalFiringRate = brain->getFiringNeuronCount();
        metrics.curiosityLevel = agent->getCuriosityLevel();
        metrics.noveltyLevel = agent->getNoveltyLevel();
        metrics.dopamineLevel = agent->getNeuromodulationLevel();
        metrics.developmentalStage = agent->getDevelopmentalStage();
        
        return metrics;
    }
    
    // Generate random configuration with seed
    std::shared_ptr<Config> createRandomConfig(uint64_t seed = 42) {
        auto config = std::make_shared<Config>();
        std::mt19937 rng(seed);
        std::uniform_int_distribution<uint64_t> neuronDist(500, 5000);
        std::uniform_real_distribution<float> densityDist(0.05f, 0.3f);
        std::uniform_real_distribution<float> floatDist(-80.0f, -20.0f);
        
        config->set("random_seed", seed);
        config->set("brain.neuron_count", neuronDist(rng));
        config->set("brain.synapse_density", densityDist(rng));
        config->set("brain.connection_probability", densityDist(rng) * 0.1f);
        config->set("brain.v_thresh", floatDist(rng));
        config->set("brain.v_rest", -70.0f);
        config->set("brain.v_reset", -75.0f);
        config->set("brain.tau_mem", 10.0f + floatDist(rng) * 30.0f);
        config->set("brain.tau_ref", 1.0f + floatDist(rng));
        
        return config;
    }
    
    // Validate configuration has all required keys
    bool validateConfig(const Config& config) {
        // Check for essential configuration keys
        std::vector<std::string> essentialKeys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.v_thresh",
            "brain.v_rest"
        };
        
        for (const auto& key : essentialKeys) {
            if (!config.has(key)) {
                return false;
            }
        }
        return true;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Complete brain simulation with integrated memory, prediction, and cognition systems
        - Advanced convenience functions for easy setup
        - Comprehensive error handling with specific exception types
        - Utility functions for analysis and simulation management
        - Command-line configuration support
        - Performance-optimized data structures
        
        Quick Start:
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        For advanced usage, see the examples directory.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMConfigError>(m, "ConfigError");
    py::register_exception<NLMBrainError>(m, "BrainError");
    py::register_exception<NLMInputError>(m, "InputError");

    // Core data types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
        
        Types of neurons in the NLM simulation:
        - Excitatory: Promotes neural activity
        - Inhibitory: Reduces neural activity  
        - Modulatory: Affects other neuron types
        - Sensory: Processes external input
        - Motor: Generates actions
        - Internal: Handles internal processes
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
        
        Types of synaptic connections:
        - Excitatory: Increases postsynaptic potential
        - Inhibitory: Decreases postsynaptic potential
        - Modulatory: Affects other processes
        - Electrical: Direct ion flow
        - GapJunction: Electrical coupling
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
        
        Stages of brain development:
        - Initial: Newborn, high plasticity
        - CriticalPeriod: Sensitive learning phase
        - Maturation: Refinement and specialization
        - Adult: Stable, balanced functioning
        - Aging: Declining plasticity
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
        
        States a neuron can be in:
        - Resting: No active potential
        - Active: Generating action potentials
        - Refractory: Cannot fire
        - Inhibited: Suppressed by inhibitory signals
        )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
        
        Types of motor actions:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - MoveLeft: Turn left
        - MoveRight: Turn right
        - TurnLeft: Rotate left
        - TurnRight: Rotate right
        - Look: Look direction
        - LookUp: Look upward
        - LookDown: Look downward
        - Interact: Interact with objects
        - Eat: Consume resources
        - Drink: Hydrate
        - Rest: Stop moving
        - Wait: Hold position
        - Custom: User-defined action
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)
        
        Motor commands for direct world interaction:
        - MoveForward: Move forward one unit
        - MoveBackward: Move backward one unit
        - TurnLeft: Rotate left 90 degrees
        - TurnRight: Rotate right 90 degrees
        - LookLeft: Look left direction
        - LookRight: Look right direction
        - Interact: Interact with current position
        - Wait: No movement
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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
        
        Types of objects in the world:
        - Empty: No object
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Impassable barrier
        - Marker: Location marker
        )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with validation and advanced methods
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Manages brain configuration parameters. Supports JSON/YAML file loading
        and command-line argument parsing.
        
        Key features:
        - Type-safe value storage (int, float, bool, string, vectors)
        - Configuration source tracking (default, file, command-line, runtime)
        - Validation methods
        - Serialization support
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file. Raises ConfigError on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises ConfigError on failure.")
        .def("set", static_cast<void (Config::*)(const std::string&, const ConfigValue&, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value. Value can be int, float, bool, string, or vector.")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set string configuration value.")
        .def("set", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set integer configuration value.")
        .def("set", static_cast<void (Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set float configuration value.")
        .def("set", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set boolean configuration value.")
        .def("get", static_cast<std::optional<int> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get integer value. Returns None if not found or wrong type.")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get float value. Returns None if not found or wrong type.")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get boolean value. Returns None if not found or wrong type.")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get string value. Returns None if not found or wrong type.")
        .def("getOr", static_cast<int (Config::*)(const std::string&, const int&) const>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get integer value or default if not found.")
        .def("getOr", static_cast<double (Config::*)(const std::string&, const double&) const>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get float value or default if not found.")
        .def("getOr", static_cast<bool (Config::*)(const std::string&, const bool&) const>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get boolean value or default if not found.")
        .def("getOr", static_cast<std::string (Config::*)(const std::string&, const std::string&) const>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get string value or default if not found.")
        .def("has", &Config::has, py::arg("key"),
             "Check if configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &utils::validateConfig,
             "Validate that essential configuration keys are present")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Utility functions for advanced users
    m.def("validateConfig", &utils::validateConfig,
          "Validate configuration has all required keys", py::arg("config"));
    
    m.def("createRandomConfig", &utils::createRandomConfig,
          "Create a random configuration with specified seed", py::arg("seed") = 42);
    
    m.def("analyzeBrain", &utils::analyzeBrain,
          "Analyze brain statistics and return formatted summary", py::arg("brain"));
    
    m.def("runEpisode", &utils::runEpisode,
          "Run a complete simulation episode and collect metrics",
          py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("maxSteps"));

    // Enhanced factory functions for easy setup
    m.def("createCompleteAgent", &factory::createCompleteAgent,
          "Create a complete agent with all subsystems configured", py::arg("configFile") = "");
    
    m.def("createLearningAgent", &factory::createLearningAgent,
          "Create a learning agent suitable for reinforcement learning", 
          py::arg("epochs") = 1000, py::arg("learningRate") = 0.01f);
    
    m.def("createPerceptionAgent", &factory::createPerceptionAgent,
          "Create a perception-only agent for testing sensory processing");

    // Original simple factory methods (maintained for backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants for easy access
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and build information
    m.attr("__version__") = "0.1.0";
    m.attr("__build_date__") = __DATE__;
    m.attr("__build_type__") = "Release";
}

} // namespace nlm
