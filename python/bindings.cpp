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
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides access to NLM simulation parameters including neuron properties,
        world settings, memory system parameters, and developmental stages.
        
        Examples:
            >>> config = Config.createDefault()
            >>> config = Config.fromFile("nlm_config.json")
            >>> config = Config()
            >>> config.set("neuron.excitatory_strength", 1.2)
    )pbdoc")
    .def(py::init<>())
    .def(py::init<const std::string&>(), py::arg("filepath"),
         R"pbdoc(
         Create configuration loaded from file.
         
         Args:
             filepath: Path to configuration file
         Returns:
             Config instance with loaded configuration
         )pbdoc")
    .def_static("createDefault", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults for NLM simulation
        config->set("neuron.count", int64_t(500), ConfigSource::Default);
        config->set("neuron.excitatory_strength", 1.0f, ConfigSource::Default);
        config->set("neuron.inhibitory_strength", 1.0f, ConfigSource::Default);
        config->set("neuron.threshold", 0.5f, ConfigSource::Default);
        config->set("neuron.reset_potential", -0.6f, ConfigSource::Default);
        config->set("neuron.membrane_time_constant", 0.02f, ConfigSource::Default);
        config->set("neuron.refractory_period", 2.0f, ConfigSource::Default);
        
        config->set("synapse.max_connections_per_neuron", int64_t(50), ConfigSource::Default);
        config->set("synapse.connection_probability", 0.15f, ConfigSource::Default);
        config->set("synapse.max_weight", 2.0f, ConfigSource::Default);
        config->set("synapse.min_weight", -2.0f, ConfigSource::Default);
        
        config->set("simulation.timestep", 0.001, ConfigSource::Default);
        config->set("simulation.max_steps", int64_t(10000), ConfigSource::Default);
        
        config->set("world.width", int64_t(16), ConfigSource::Default);
        config->set("world.height", int64_t(16), ConfigSource::Default);
        config->set("world.cell_size", 0.5f, ConfigSource::Default);
        
        config->set("random.seed", int64_t(42), ConfigSource::Default);
        
        // Enable all learning systems by default
        config->set("plasticity.stdp_enabled", true, ConfigSource::Default);
        config->set("plasticity.hebbian_enabled", true, ConfigSource::Default);
        config->set("plasticity.structural_enabled", true, ConfigSource::Default);
        config->set("development.enabled", true, ConfigSource::Default);
        config->set("memory.working_memory_enabled", true, ConfigSource::Default);
        config->set("memory.episodic_memory_enabled", true, ConfigSource::Default);
        config->set("memory.associative_memory_enabled", true, ConfigSource::Default);
        
        config->set("neuromodulation.dopamine_enabled", true, ConfigSource::Default);
        config->set("neuromodulation.curiosity_enabled", true, ConfigSource::Default);
        config->set("neuromodulation.novelty_enabled", true, ConfigSource::Default);
        
        config->set("prediction.enabled", true, ConfigSource::Default);
        config->set("cognition.planner_enabled", true, ConfigSource::Default);
        config->set("cognition.concept_formation_enabled", true, ConfigSource::Default);
        config->set("cognition.attention_enabled", true, ConfigSource::Default);
        
        return config;
    }, R"pbdoc(
        Create a default configuration with sensible defaults for NLM simulation.
        
        Returns:
            Config instance with optimized default values
        )pbdoc")
    .def_static("fromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::key_error("Failed to load configuration from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"),
        R"pbdoc(
        Create and load configuration from file.
        
        Args:
            filepath: Path to configuration file
        
        Returns:
            Config instance with loaded configuration
        
        Raises:
            KeyError: If file cannot be loaded
        )pbdoc")
    .def_static("fromDict", [](py::dict dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto item : dict) {
            std::string key = py::str(item.first).cast<std::string>();
            py::object value = item.second;
            
            // Convert Python value to ConfigValue
            try {
                if (value.is(py::int_())) {
                    config->set(key, value.cast<int64_t>());
                } else if (value.is(py::float_())) {
                    config->set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    config->set(key, value.cast<bool>());
                } else {
                    config->set(key, value.cast<std::string>());
                }
            } catch (const std::exception& e) {
                throw py::value_error("Error setting configuration key " + key + ": " + std::string(e.what()));
            }
        }
        return config;
    }, py::arg("dict"),
        R"pbdoc(
        Create configuration from Python dictionary.
        
        Args:
            dict: Dictionary with configuration key-value pairs
        
        Returns:
            Config instance
        
        Examples:
            >>> config = Config.fromDict({
            ...     "neuron.count": 500,
            ...     "world.width": 16,
            ...     "world.height": 16,
            ...     "plasticity.stdp_enabled": True
            ... })
        )pbdoc")
    .def("toDict", [](const Config& self) {
        py::dict result;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<std::string>(key);
            if (value) {
                result[key.c_str()] = value.value();
            } else {
                // Try other types
                auto intVal = self.get<int64_t>(key);
                if (intVal) {
                    result[key.c_str()] = intVal.value();
                } else {
                    auto doubleVal = self.get<double>(key);
                    if (doubleVal) {
                        result[key.c_str()] = doubleVal.value();
                    } else {
                        auto boolVal = self.get<bool>(key);
                        if (boolVal) {
                            result[key.c_str()] = boolVal.value();
                        }
                    }
                }
            }
        }
        return result;
    }, R"pbdoc(
        Convert configuration to Python dictionary.
        
        Returns:
            Dictionary with all configuration key-value pairs
        )pbdoc")
    .def("validate", &validateConfig, py::arg("config"),
         R"pbdoc(
        Validate configuration consistency.
        
        Args:
            config: Configuration to validate
        
        Returns:
            Tuple of (is_valid, errors_list)
            
        Raises:
            ValueError: If configuration is invalid
        )pbdoc")
    .def("merge", &mergeConfig, py::arg("config"), py::arg("other_config"),
         R"pbdoc(
        Merge another configuration into this one.
        
        Args:
            other_config: Configuration to merge (other_config takes precedence)
        )pbdoc")
    .def("clone", []() { return std::make_shared<Config>(*this); },
         "Create a copy of this configuration")
    .def("loadFromFile", [](Config& self, const std::string& filepath) {
        if (filepath.empty()) {
            throw py::value_error("File path cannot be empty");
        }
        return self.loadFromFile(filepath);
    }, py::arg("filepath"),
        R"pbdoc(
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
        
        Returns:
            True if file loaded successfully, False otherwise
        
        Raises:
            ValueError: If filepath is empty
        )pbdoc")
    .def("loadFromArgs", [](Config& self, int argc, char** argv) {
        return self.loadFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"),
        "Load configuration from command line arguments")
    .def("saveToFile", [](Config& self, const std::string& filepath) {
        if (filepath.empty()) {
            throw py::value_error("File path cannot be empty");
        }
        return self.saveToFile(filepath);
    }, py::arg("filepath"),
        R"pbdoc(
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration file
        
        Returns:
            True if file saved successfully, False otherwise
        
        Raises:
            ValueError: If filepath is empty
        )pbdoc")
    .def("has", &Config::has, py::arg("key"),
         "Check if a configuration key exists")
    .def("getKeys", &Config::getKeys,
         "Get all configuration keys")
    .def("clear", &Config::clear,
         "Clear all configuration entries")
    .def("summary", &Config::summary,
         "Get a summary string of the configuration")
    .def("__repr__", [](const Config& cfg) {
        return "<Config: " + cfg.summary() + ">";
    }, "String representation of configuration")
    // Enhanced getters with type conversion
    .def("get", [](const Config& self, const std::string& key, const py::object& defaultValue) {
        if (self.has(key)) {
            // Try to get the appropriate type based on default value type
            if (defaultValue.is(py::int_())) {
                auto val = self.get<int64_t>(key);
                return val.has_value() ? py::cast(val.value()) : defaultValue;
            } else if (defaultValue.is(py::float_())) {
                auto val = self.get<double>(key);
                return val.has_value() ? py::cast(val.value()) : defaultValue;
            } else if (defaultValue.is(py::bool_())) {
                auto val = self.get<bool>(key);
                return val.has_value() ? py::cast(val.value()) : defaultValue;
            } else if (defaultValue.is(py::str())) {
                auto val = self.get<std::string>(key);
                return val.has_value() ? py::cast(val.value()) : defaultValue;
            }
        }
        return defaultValue;
    }, py::arg("key"), py::arg("default") = nullptr,
        R"pbdoc(
        Get configuration value with Pythonic interface.
        
        Args:
            key: Configuration key (dot notation supported)
            default: Default value if key doesn't exist
        
        Returns:
            Configuration value or default
        
        Examples:
            >>> config.get("neuron.count", 500)
            >>> config.get("world.width", 16)
            >>> config.get("plasticity.stdp_enabled", True)
        )pbdoc")
    // Enhanced setters with validation
    .def("set", [](Config& self, const std::string& key, const py::object& value) {
        if (value.is(py::int_())) {
            self.set(key, value.cast<int64_t>());
        } else if (value.is(py::float_())) {
            self.set(key, value.cast<double>());
        } else if (value.is(py::bool_())) {
            self.set(key, value.cast<bool>());
        } else if (value.is(py::str())) {
            self.set(key, value.cast<std::string>());
        } else {
            throw py::type_error("Unsupported value type for configuration key: " + key);
        }
    }, py::arg("key"), py::arg("value"),
        R"pbdoc(
        Set configuration value.
        
        Args:
            key: Configuration key (dot notation supported)
            value: Value to set (int, float, bool, or string)
        
        Examples:
            >>> config.set("neuron.count", 500)
            >>> config.set("world.width", 16)
            >>> config.set("plasticity.stdp_enabled", True)
        )pbdoc")
    // Convenience getters for common NLM parameters
    .def("getNeuronCount", &Config::getOr<int64_t>, py::arg("defaultValue") = 500,
         "Get neuron count with default")
    .def("getWorldSize", [](const Config& self) {
        int64_t width = self.getOr<int64_t>("world.width", 16);
        int64_t height = self.getOr<int64_t>("world.height", 16);
        return std::make_pair(width, height);
    }, "Get world dimensions as (width, height) tuple")
    .def("getSimulationTimestep", &Config::getOr<double>, py::arg("defaultValue") = 0.001,
         "Get simulation timestep with default")
    .def("getMaxSteps", &Config::getOr<int64_t>, py::arg("defaultValue") = 10000,
         "Get maximum simulation steps with default")
    // Brain creation helpers
    .def_static("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"),
        R"pbdoc(
        Create a brain with configuration and auto-initialize.
        
        Args:
            config: Configuration for the brain
        
        Returns:
            Initialized brain instance
        
        Examples:
            >>> config = Config.createDefault()
            >>> brain = Config.createBrain(config)
        )pbdoc")
    .def_static("createBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = Config::fromFile(filepath);
        return Config::createBrain(config);
    }, py::arg("filepath"),
        R"pbdoc(
        Create and initialize a brain from configuration file.
        
        Args:
            filepath: Path to configuration file
        
        Returns:
            Initialized brain instance
        )pbdoc")
    // World creation helpers
    .def_static("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a simple world for NLM simulation.
        
        Returns:
            SimpleWorld instance
        
        Examples:
            >>> world = Config.createSimpleWorld()
            >>> world.configure(16, 16, 8, 8)  # width, height, vision_width, vision_height
        )pbdoc")
    .def_static("createSimpleWorldWithConfig", [](const Config& config) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            config.getOr<int64_t>("world.width", 16),
            config.getOr<int64_t>("world.height", 16),
            config.getOr<int64_t>("world.vision_width", 8),
            config.getOr<int64_t>("world.vision_height", 8)
        );
        return world;
    }, py::arg("config"),
        R"pbdoc(
        Create and configure a simple world from configuration.
        
        Args:
            config: Configuration with world settings
        
        Returns:
            Configured SimpleWorld instance
        )pbdoc")
    // Agent creation helpers
    .def_static("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
        R"pbdoc(
        Create an agent brain interface.
        
        Args:
            brain: The underlying brain instance
        
        Returns:
            AgentBrain instance
        
        Examples:
            >>> brain = Config.createBrain(config)
            >>> agent = Config.createAgentBrain(brain)
        )pbdoc")
    .def_static("createAgentBrainWithWorld", [](std::shared_ptr<Brain> brain, const SimpleWorld& world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        R"pbdoc(
        Create an agent brain with world initialization.
        
        Args:
            brain: The underlying brain instance
            world: World to initialize the agent with
        
        Returns:
            Initialized AgentBrain instance
        )pbdoc")

    // Configuration validation and merging helper functions
    bool validateConfig(const Config& config) {
        bool isValid = true;
        std::vector<std::string> errors;
        
        // Check for required keys
        const std::vector<std::string> requiredKeys = {
            "neuron.count",
            "world.width", 
            "world.height",
            "simulation.timestep"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                isValid = false;
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Validate value ranges
        if (config.has("neuron.count")) {
            auto count = config.get<int64_t>("neuron.count");
            if (count && (*count <= 0 || *count > 1000000)) {
                isValid = false;
                errors.push_back("Invalid neuron count: " + std::to_string(*count));
            }
        }
        
        if (config.has("world.width")) {
            auto width = config.get<int64_t>("world.width");
            if (width && (*width <= 0 || *width > 1000)) {
                isValid = false;
                errors.push_back("Invalid world width: " + std::to_string(*width));
            }
        }
        
        if (config.has("world.height")) {
            auto height = config.get<int64_t>("world.height");
            if (height && (*height <= 0 || *height > 1000)) {
                isValid = false;
                errors.push_back("Invalid world height: " + std::to_string(*height));
            }
        }
        
        if (config.has("simulation.timestep")) {
            auto timestep = config.get<double>("simulation.timestep");
            if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
                isValid = false;
                errors.push_back("Invalid simulation timestep: " + std::to_string(*timestep));
            }
        }
        
        // Log errors for debugging
        for (const auto& error : errors) {
            NLM_LOG_WARN("Config validation error: " + error);
        }
        
        return isValid;
    }
    
    void mergeConfig(Config& config, const Config& other) {
        for (const auto& key : other.getKeys()) {
            if (other.has(key)) {
                auto value = other.get<std::string>(key);
                if (value) {
                    config.set(key, *value);
                } else {
                    auto intVal = other.get<int64_t>(key);
                    if (intVal) {
                        config.set(key, *intVal);
                    } else {
                        auto doubleVal = other.get<double>(key);
                        if (doubleVal) {
                            config.set(key, *doubleVal);
                        } else {
                            auto boolVal = other.get<bool>(key);
                            if (boolVal) {
                                config.set(key, *boolVal);
                            }
                        }
                    }
                }
            }
        }
    }

    // High-level simulation helper functions
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createSimulationSetup(const Config& config) {
        // Create brain
        auto brain = Config::createBrain(config);
        
        // Create world
        auto world = Config::createSimpleWorldWithConfig(config);
        
        // Create agent
        auto agent = Config::createAgentBrainWithWorld(brain, *world);
        
        // Configure agent based on config
        agent->enableRewardModulation(config.getOr<bool>("neuromodulation.reward_enabled", true));
        agent->enableStructuralPlasticity(config.getOr<bool>("neuromodulation.structural_enabled", true));
        agent->enableDevelopment(config.getOr<bool>("neuromodulation.development_enabled", true));
        agent->enableCuriosity(config.getOr<bool>("neuromodulation.curiosity_enabled", true));
        
        return std::make_tuple(brain, world, agent);
    }

    // Module-level convenience functions
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        return config;
    }
    
    std::shared_ptr<Brain> createBrainFromDefaults() {
        auto config = createDefaultConfig();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createReadySimulation() {
        auto config = createDefaultConfig();
        return createSimulationSetup(*config);
    // Module-level convenience functions
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        return config;
    }
    
    std::shared_ptr<Brain> createBrainFromDefaults() {
        auto config = createDefaultConfig();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createReadySimulation() {
        auto config = createDefaultConfig();
        return createSimulationSetup(*config);
    }

    // Now add these as module-level functions
    m.def("createDefaultConfig", []() {
        return createDefaultConfig();
    }, "Create a default configuration with sensible defaults")
    .def("createBrainFromDefaults", []() {
        return createBrainFromDefaults();
    }, "Create a brain with default configuration and auto-initialize")
    .def("createReadySimulation", []() {
        auto result = createReadySimulation();
        auto brain = std::get<0>(result);
        auto world = std::get<1>(result);
        auto agent = std::get<2>(result);
        
        // Return as Python tuple-like object
        return py::make_tuple(brain, world, agent);
    }, "Create a ready-to-use simulation with brain, world, and agent");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
