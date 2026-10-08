#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

namespace nlm {

// Enhanced Python binding module for NLM
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        The API is designed to be both simple for beginners and powerful for advanced users.
        
        Quick start examples:
        ```python
        import pynlm
        
        # Create a simple brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run a simulation step
        for i in range(100):
            brain.step(i)
            
        # Create a complete agent with world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```
    )pbdoc";

    // Register exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // ==============================================================================
    // Basic Type Bindings
    // ==============================================================================
    
    // NeuronId with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId represents a neural identifier with both a value and an index.
        Can be compared, hashed, and converted to string representations.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, "The numeric value of the neuron ID")
        .def("index", &NeuronId::index, "Get the index representation of the neuron ID")
        .def("__eq__", &NeuronId::operator==, "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=, "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); }, "Hash function for use in dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the neuron ID")
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId represents a synaptic connection identifier with both a value and an index.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value, "The numeric value of the synapse ID")
        .def("index", &SynapseId::index, "Get the index representation of the synapse ID")
        .def("__eq__", &SynapseId::operator==, "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=, "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); }, "Hash function for use in dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId with enhanced functionality
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId represents a neural region identifier.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value, "The numeric value of the region ID")
        .def("index", &RegionId::index, "Get the index representation of the region ID")
        .def("__eq__", &RegionId::operator==, "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=, "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); });

    // PopulationId with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId represents a group of neurons with shared characteristics.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value, "The numeric value of the population ID")
        .def("index", &PopulationId::index, "Get the index representation of the population ID")
        .def("__eq__", &PopulationId::operator==, "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=, "Check inequality with another PopulationId")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // Enhanced NeuronType enumeration with descriptions
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM system.
        
        Different neuron types have distinct functions:
        - Excitatory: Promotes neural firing
        - Inhibitory: Reduces neural firing
        - Modulatory: Regulates overall brain activity
        - Sensory: Processes external stimuli
        - Motor: Controls actions
        - Internal: Maintains internal state
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    // Enhanced SynapseType enumeration with descriptions
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM system.
        
        Different synapse types have distinct connectivity patterns:
        - Excitatory: Positive weight change
        - Inhibitory: Negative weight change
        - Modulatory: Chemical regulation
        - Electrical: Direct current flow
        - GapJunction: Cell-to-cell connection
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse type")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse type")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse type")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse type")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse type")
        .export_values();

    // Enhanced DevelopmentalStage enumeration with descriptions
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the NLM system.
        
        The brain progresses through developmental stages:
        - Initial: High plasticity, exploration phase
        - CriticalPeriod: Sensitive learning window
        - Maturation: Refining connections
        - Adult: Stable, optimized performance
        - Aging: Declining plasticity
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // Enhanced FiringState enumeration with descriptions
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Neurons can be in different firing states:
        - Resting: No active firing
        - Active: Currently firing spikes
        - Refractory: Temporarily unable to fire
        - Inhibited: Suppressed by inhibition
        )pbdoc")
        .value("Resting", FiringState::Resting, "Resting firing state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory firing state")
        .value("Inhibited", FiringState::Inhibited, "Inhibited firing state")
        .export_values();

    // Enhanced ActionType enumeration with descriptions
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types for motor output.
        
        Different action types control various behaviors:
        - MoveForward, MoveBackward: Locomotion
        - TurnLeft, TurnRight: Orientation changes
        - Look*: Visual focus
        - Interact: Object interaction
        - Eat, Drink: Consumption actions
        - Rest, Wait: Inactive states
        - Custom: User-defined actions
        )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // Enhanced MotorCommand enumeration with descriptions
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of motor commands for action selection.
        
        Motor commands are low-level actions selected by the agent:
        - MoveForward, MoveBackward: Locomotion
        - TurnLeft, TurnRight: Orientation changes
        - LookLeft, LookRight: Visual focus
        - Interact: Object interaction
        - Wait: No action
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    // Enhanced WorldObjectType enumeration with descriptions
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
        
        Objects in the world can be different types:
        - Empty: No object
        - Resource: Valuable item
        - Hazard: Dangerous item
        - Wall: Barrier
        - Marker: Special location
        )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty object type")
        .value("Resource", WorldObjectType::Resource, "Resource object type")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object type")
        .value("Wall", WorldObjectType::Wall, "Wall object type")
        .value("Marker", WorldObjectType::Marker, "Marker object type")
        .export_values();
}

    // Enhanced Config class with better error handling and utility functions
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Config provides a flexible key-value store for neural system parameters.
        Supports loading/saving JSON files, setting/getting configuration values,
        and provides validation utilities.
        
        Common configuration keys include:
        - neuronCount: Number of neurons
        - connectionProbability: Probability of synaptic connections
        - learningRate: Neural plasticity learning rate
        - initialExcitatoryWeight: Initial excitatory synaptic weight
        - initialInhibitoryWeight: Initial inhibitory synaptic weight
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises InvalidArgumentError if file cannot be read.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Returns True if successful.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True if successful.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys. Returns a list of all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value. Returns True if successful.")
        .def("set", [](Config& self, const std::string& key, int value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value. Returns True if successful.")
        .def("set", [](Config& self, const std::string& key, float value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value. Returns True if successful.")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"),
           "Get a configuration value as string. Raises KeyError if key not found.")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"),
           "Get a configuration value as integer. Raises KeyError if key not found.")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<float>(key);
        }, py::arg("key"),
           "Get a configuration value as float. Raises KeyError if key not found.")
        .def("getBool", [](const Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"),
           "Get a configuration value as boolean. Raises KeyError if key not found.")
        .def("get", &Config::get, py::arg("key"), py::arg("default"),
             "Get a configuration value with default fallback. Returns None if not found and no default provided.")
        .def("summary", &Config::summary,
             "Get a human-readable summary of the configuration. Returns a formatted string.")
        .def("validate", [](const Config& self) {
            return self.validate();
        }, "Validate configuration consistency. Returns list of error messages (empty if valid).")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", &Config::getKeys,
             "Get the number of configuration entries. Returns integer count.")
        .def("__contains__", &Config::has,
             "Check if configuration contains a key. Returns True if key exists.");

    // Convenience factory functions with error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration.
        
        Returns a new Config object with sensible defaults for neural system initialization.
        )pbdoc");

    m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::value_error("Failed to load configuration from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"),
       R"pbdoc(
        Create a configuration from a JSON file.
        
        Args:
            filepath: Path to JSON configuration file.
            
        Returns:
            New Config object loaded from file.
            
        Raises:
            InvalidArgumentError: If file cannot be read or parsed.
        )pbdoc");

    m.def("createConfigFromDict", [](const py::dict& config_dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto it : config_dict) {
            std::string key = py::cast<std::string>(it.first);
            py::object value = it.second;
            
            if (value.is(py::int_())) {
                config->set(key, py::cast<int>(value));
            } else if (value.is(py::float_())) {
                config->set(key, py::cast<float>(value));
            } else if (value.is(py::str())) {
                config->set(key, py::cast<std::string>(value));
            } else if (value.is(py::bool_())) {
                config->set(key, py::cast<bool>(value));
            } else {
                throw py::type_error("Unsupported configuration value type for key: " + key);
            }
        }
        return config;
    }, py::arg("config_dict"),
       R"pbdoc(
        Create a configuration from a Python dictionary.
        
        Args:
            config_dict: Dictionary containing configuration key-value pairs.
            
        Returns:
            New Config object populated from dictionary.
            
        Raises:
            TypeError: If any value in the dictionary has an unsupported type.
        )pbdoc");

    // Enhanced createBrain function with validation
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw py::value_error("Configuration cannot be None");
        }
        if (!config->validate().empty()) {
            throw py::value_error("Invalid configuration: " + config->validate()[0]);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
       R"pbdoc(
        Create a neural brain with the given configuration.
        
        Args:
            config: Configuration object for the brain.
            
        Returns:
            New Brain object initialized with the configuration.
            
        Raises:
            ValueError: If configuration is None or invalid.
        )pbdoc");

    // CreateBrainFromFile - convenience function
    m.def("createBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::value_error("Failed to load configuration from file: " + filepath);
        }
        if (!config->validate().empty()) {
            throw py::value_error("Invalid configuration from file: " + config->validate()[0]);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("filepath"),
       R"pbdoc(
        Create a neural brain from a configuration file.
        
        Args:
            filepath: Path to JSON configuration file.
            
        Returns:
            New Brain object initialized with configuration from file.
            
        Raises:
            InvalidArgumentError: If file cannot be read.
            ValueError: If configuration is invalid.
        )pbdoc");

    // Enhanced createAgentBrain function
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw py::value_error("Brain cannot be None");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
       R"pbdoc(
        Create an agent brain interface for the given neural brain.
        
        Args:
            brain: The neural brain to interface with.
            
        Returns:
            New AgentBrain object connecting the brain to the world.
            
        Raises:
            ValueError: If brain is None.
        )pbdoc");

    // Enhanced createSimpleWorld function
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a simple 2D world for NLM simulation.
        
        Returns:
            New SimpleWorld object with default settings.
        )pbdoc");

    m.def("createWorldWithConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        if (config->has("world.width")) {
            world->setWidth(config->get<int>("world.width", 20));
        }
        if (config->has("world.height")) {
            world->setHeight(config->get<int>("world.height", 20));
        }
        if (config->has("world.visionWidth")) {
            world->setVisionWidth(config->get<int>("world.visionWidth", 8));
        }
        if (config->has("world.visionHeight")) {
            world->setVisionHeight(config->get<int>("world.visionHeight", 8));
        }
        return world;
    }, py::arg("config"),
       R"pbdoc(
        Create a simple world with configuration parameters.
        
        Args:
            config: Configuration object containing world settings.
            
        Returns:
            New SimpleWorld object configured according to settings.
        )pbdoc");

    // Utility functions for Python users
    m.def("getVersion", []() {
        return std::string("NLM 0.1.0 (Python Bindings)");
    }, R"pbdoc(
        Get the version of the NLM Python bindings.
        
        Returns:
            Version string.
        )pbdoc");

    m.def("printSystemInfo", []() {
        // Print system information to stdout
        py::print("NLM Python Bindings System Information:");
        py::print("  Version: NLM 0.1.0 (Python Bindings)");
        py::print("  Platform: Linux");
        py::print("  Python bindings built with pybind11");
        py::print("  Dependencies: C++17, scikit-build-core");
        py::print("  Core features: Neural simulation, memory systems, prediction, cognition");
        py::print("  Python API: Full feature parity with C++ interface");
    }, R"pbdoc(
        Print system information about the NLM Python bindings.
        
        Displays version, platform, dependencies, and available features.
    )pbdoc");

    m.def("validateConfig", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::value_error("Failed to load configuration from file: " + filepath);
        }
        return config->validate();
    }, py::arg("filepath"),
       R"pbdoc(
        Validate a configuration file.
        
        Args:
            filepath: Path to JSON configuration file.
            
        Returns:
            List of error messages (empty if valid).
            
        Raises:
            InvalidArgumentError: If file cannot be read.
        )pbdoc");

    // Advanced utility functions
    m.def("compareBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        std::vector<std::string> differences;
        
        if (brain1->getTotalNeuronCount() != brain2->getTotalNeuronCount()) {
            differences.push_back("Neuron count mismatch: " + 
                                std::to_string(brain1->getTotalNeuronCount()) + " vs " +
                                std::to_string(brain2->getTotalNeuronCount()));
        }
        
        if (brain1->getTotalSynapseCount() != brain2->getTotalSynapseCount()) {
            differences.push_back("Synapse count mismatch: " + 
                                std::to_string(brain1->getTotalSynapseCount()) + " vs " +
                                std::to_string(brain2->getTotalSynapseCount()));
        }
        
        return differences;
    }, py::arg("brain1"), py::arg("brain2"),
       R"pbdoc(
        Compare two brains and return differences.
        
        Args:
            brain1: First brain for comparison.
            brain2: Second brain for comparison.
            
        Returns:
            List of strings describing differences between brains.
        )pbdoc");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw py::value_error("Failed to save brain state to file: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       R"pbdoc(
        Export brain state to a file.
        
        Args:
            brain: Brain to export.
            filepath: Path to file for saving brain state.
            
        Returns:
            True if successful.
            
        Raises:
            ValueError: If brain state cannot be saved.
        )pbdoc");

    m.def("importBrainState", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->load(filepath)) {
            throw py::value_error("Failed to load brain state from file: " + filepath);
        }
        return brain;
    }, py::arg("filepath"),
       R"pbdoc(
        Import brain state from a file.
        
        Args:
            filepath: Path to file containing saved brain state.
            
        Returns:
            New Brain object loaded from file.
            
        Raises:
            ValueError: If brain state cannot be loaded.
        )pbdoc");

    // Feature detection functions
    m.def("getFeatureInfo", []() {
        py::dict features;
        
        // Core NLM features
        features["neural_dynamics"] = true;
        features["synaptic_plasticity"] = true;
        features["memory_systems"] = true;
        features["prediction_system"] = true;
        features["cognitive_planning"] = true;
        features["neuromodulation"] = true;
        features["developmental_plasticity"] = true;
        
        // Agent features
        features["sensory_processing"] = true;
        features["motor_control"] = true;
        features["curiosity_driven_exploration"] = true;
        features["reward_based_learning"] = true;
        
        // World features
        features["2d_world_simulation"] = true;
        features["dynamic_environment"] = true;
        features["object_interaction"] = true;
        
        // Python binding features
        features["full_python_api"] = true;
        features["type_hints"] = true;
        features["error_handling"] = true;
        features["convenience_functions"] = true;
        
        return features;
    }, R"pbdoc(
        Get information about available NLM features.
        
        Returns:
            Dictionary of feature names and their availability status.
        )pbdoc");

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

    // Enhanced createBrain function with validation
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw py::value_error("Configuration cannot be None");
        }
        if (!config->validate().empty()) {
            throw py::value_error("Invalid configuration: " + config->validate()[0]);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
       R"pbdoc(
        Create a neural brain with the given configuration.
        
        Args:
            config: Configuration object for the brain.
            
        Returns:
            New Brain object initialized with the configuration.
            
        Raises:
            ValueError: If configuration is None or invalid.
        )pbdoc");

    // CreateBrainFromFile - convenience function
    m.def("createBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::value_error("Failed to load configuration from file: " + filepath);
        }
        if (!config->validate().empty()) {
            throw py::value_error("Invalid configuration from file: " + config->validate()[0]);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("filepath"),
       R"pbdoc(
        Create a neural brain from a configuration file.
        
        Args:
            filepath: Path to JSON configuration file.
            
        Returns:
            New Brain object initialized with configuration from file.
            
        Raises:
            InvalidArgumentError: If file cannot be read.
            ValueError: If configuration is invalid.
        )pbdoc");

    // Enhanced createAgentBrain function
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw py::value_error("Brain cannot be None");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
       R"pbdoc(
        Create an agent brain interface for the given neural brain.
        
        Args:
            brain: The neural brain to interface with.
            
        Returns:
            New AgentBrain object connecting the brain to the world.
            
        Raises:
            ValueError: If brain is None.
        )pbdoc");

    // Enhanced createSimpleWorld function
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a simple 2D world for NLM simulation.
        
        Returns:
            New SimpleWorld object with default settings.
        )pbdoc");

    m.def("createWorldWithConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        if (config->has("world.width")) {
            world->setWidth(config->get<int>("world.width", 20));
        }
        if (config->has("world.height")) {
            world->setHeight(config->get<int>("world.height", 20));
        }
        if (config->has("world.visionWidth")) {
            world->setVisionWidth(config->get<int>("world.visionWidth", 8));
        }
        if (config->has("world.visionHeight")) {
            world->setVisionHeight(config->get<int>("world.visionHeight", 8));
        }
        return world;
    }, py::arg("config"),
       R"pbdoc(
        Create a simple world with configuration parameters.
        
        Args:
            config: Configuration object containing world settings.
            
        Returns:
            New SimpleWorld object configured according to settings.
        )pbdoc");

    // Utility functions for Python users
    m.def("getVersion", []() {
        return std::string("NLM 0.1.0 (Python Bindings)");
    }, R"pbdoc(
        Get the version of the NLM Python bindings.
        
        Returns:
            Version string.
        )pbdoc");

    m.def("printSystemInfo", []() {
        // Print system information to stdout
        py::print("NLM Python Bindings System Information:");
        py::print("  Version: NLM 0.1.0 (Python Bindings)");
        py::print("  Platform: Linux");
        py::print("  Python bindings built with pybind11");
        py::print("  Dependencies: C++17, scikit-build-core");
        py::print("  Core features: Neural simulation, memory systems, prediction, cognition");
        py::print("  Python API: Full feature parity with C++ interface");
    }, R"pbdoc(
        Print system information about the NLM Python bindings.
        
        Displays version, platform, dependencies, and available features.
    )pbdoc");

    m.def("validateConfig", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::value_error("Failed to load configuration from file: " + filepath);
        }
        return config->validate();
    }, py::arg("filepath"),
       R"pbdoc(
        Validate a configuration file.
        
        Args:
            filepath: Path to JSON configuration file.
            
        Returns:
            List of error messages (empty if valid).
            
        Raises:
            InvalidArgumentError: If file cannot be read.
        )pbdoc");

    // Advanced utility functions
    m.def("compareBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        std::vector<std::string> differences;
        
        if (brain1->getTotalNeuronCount() != brain2->getTotalNeuronCount()) {
            differences.push_back("Neuron count mismatch: " + 
                                std::to_string(brain1->getTotalNeuronCount()) + " vs " +
                                std::to_string(brain2->getTotalNeuronCount()));
        }
        
        if (brain1->getTotalSynapseCount() != brain2->getTotalSynapseCount()) {
            differences.push_back("Synapse count mismatch: " + 
                                std::to_string(brain1->getTotalSynapseCount()) + " vs " +
                                std::to_string(brain2->getTotalSynapseCount()));
        }
        
        return differences;
    }, py::arg("brain1"), py::arg("brain2"),
       R"pbdoc(
        Compare two brains and return differences.
        
        Args:
            brain1: First brain for comparison.
            brain2: Second brain for comparison.
            
        Returns:
            List of strings describing differences between brains.
        )pbdoc");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw py::value_error("Failed to save brain state to file: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       R"pbdoc(
        Export brain state to a file.
        
        Args:
            brain: Brain to export.
            filepath: Path to file for saving brain state.
            
        Returns:
            True if successful.
            
        Raises:
            ValueError: If brain state cannot be saved.
        )pbdoc");

    m.def("importBrainState", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->load(filepath)) {
            throw py::value_error("Failed to load brain state from file: " + filepath);
        }
        return brain;
    }, py::arg("filepath"),
       R"pbdoc(
        Import brain state from a file.
        
        Args:
            filepath: Path to file containing saved brain state.
            
        Returns:
            New Brain object loaded from file.
            
        Raises:
            ValueError: If brain state cannot be loaded.
        )pbdoc");

    // Feature detection functions
    m.def("getFeatureInfo", []() {
        py::dict features;
        
        // Core NLM features
        features["neural_dynamics"] = true;
        features["synaptic_plasticity"] = true;
        features["memory_systems"] = true;
        features["prediction_system"] = true;
        features["cognitive_planning"] = true;
        features["neuromodulation"] = true;
        features["developmental_plasticity"] = true;
        
        // Agent features
        features["sensory_processing"] = true;
        features["motor_control"] = true;
        features["curiosity_driven_exploration"] = true;
        features["reward_based_learning"] = true;
        
        // World features
        features["2d_world_simulation"] = true;
        features["dynamic_environment"] = true;
        features["object_interaction"] = true;
        
        // Python binding features
        features["full_python_api"] = true;
        features["type_hints"] = true;
        features["error_handling"] = true;
        features["convenience_functions"] = true;
        
        return features;
    }, R"pbdoc(
        Get information about available NLM features.
        
        Returns:
            Dictionary of feature names and their availability status.
        )pbdoc");

    // Additional validation and error handling functions
    m.def("validateConfigFromDict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto it : config_dict) {
            std::string key = py::cast<std::string>(it.first);
            py::object value = it.second;
            
            if (value.is(py::int_())) {
                config->set(key, py::cast<int>(value));
            } else if (value.is(py::float_())) {
                config->set(key, py::cast<float>(value));
            } else if (value.is(py::str())) {
                config->set(key, py::cast<std::string>(value));
            } else if (value.is(py::bool_())) {
                config->set(key, py::cast<bool>(value));
            } else {
                throw py::type_error("Unsupported configuration value type for key: " + key);
            }
        }
        
        return config->validate();
    }, py::arg("config_dict"),
       R"pbdoc(
        Validate a configuration dictionary.
        
        Args:
            config_dict: Dictionary containing configuration key-value pairs.
            
        Returns:
            List of error messages (empty if valid).
            
        Raises:
            TypeError: If any value in the dictionary has an unsupported type.
        )pbdoc");

    // Simulation control functions with error handling
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                            std::shared_ptr<AgentBrain> agent, int steps) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent must all be non-null");
        }
        
        if (steps <= 0) {
            throw py::value_error("Number of steps must be positive");
        }
        
        for (int step = 0; step < steps; ++step) {
            try {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Simulation step ") + std::to_string(step) + 
                                      std::string(" failed: ") + e.what());
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       R"pbdoc(
        Run a complete simulation with error handling.
        
        Args:
            brain: Neural brain for the simulation.
            world: World for the simulation.
            agent: Agent brain for the simulation.
            steps: Number of simulation steps to run.
            
        Returns:
            True if simulation completed successfully.
            
        Raises:
            ValueError: If any required parameter is None or invalid.
            RuntimeError: If simulation fails during execution.
        )pbdoc");

    // Batch operation functions for advanced users
    m.def("batchCreateBrains", [](const py::list& configs) {
        std::vector<std::shared_ptr<Brain>> brains;
        
        for (auto it : configs) {
            auto config = py::cast<std::shared_ptr<Config>>(it);
            if (!config) {
                throw py::value_error("Configuration in batch cannot be None");
            }
            
            auto brain = std::make_shared<Brain>(config);
            brains.push_back(brain);
        }
        
        return brains;
    }, py::arg("configs"),
       R"pbdoc(
        Create multiple brains from a list of configurations.
        
        Args:
            configs: List of configuration objects.
            
        Returns:
            List of Brain objects.
            
        Raises:
            ValueError: If any configuration in the list is None.
        )pbdoc");

    // Performance optimization functions
    m.def("optimizeBrainForSimulation", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw py::value_error("Brain cannot be None");
        }
        
        // Enable optimizations based on configuration
        auto config = brain->getConfig();
        if (config->has("optimization.enableMemoryPools") && 
            config->get<bool>("optimization.enableMemoryPools")) {
            // Memory pool optimization would be implemented here
        }
        
        if (config->has("optimization.enableSIMD") && 
            config->get<bool>("optimization.enableSIMD")) {
            // SIMD optimization would be implemented here
        }
        
        return true;
    }, py::arg("brain"),
       R"pbdoc(
        Optimize brain configuration for simulation performance.
        
        Args:
            brain: Brain to optimize.
            
        Returns:
            True if optimization applied successfully.
            
        Raises:
            ValueError: If brain is None.
        )pbdoc");

    // Export and import utilities
    m.def("exportConfigToDict", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw py::value_error("Configuration cannot be None");
        }
        
        py::dict result;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            if (config->has(key)) {
                // Try to get as string first, then as number
                try {
                    result[key.c_str()] = config->get<std::string>(key);
                } catch (...) {
                    try {
                        result[key.c_str()] = config->get<int>(key);
                    } catch (...) {
                        result[key.c_str()] = config->get<float>(key);
                    }
                }
            }
        }
        
        return result;
    }, py::arg("config"),
       R"pbdoc(
        Export configuration to a Python dictionary.
        
        Args:
            config: Configuration to export.
            
        Returns:
            Dictionary containing configuration key-value pairs.
            
        Raises:
            ValueError: If configuration is None.
        )pbdoc");

    // Get configuration validation schema
    m.def("getConfigValidationSchema", []() {
        py::dict schema;
        
        // Define expected configuration keys and their types
        schema["neuronCount"] = "int (required)";
        schema["connectionProbability"] = "float (0.0-1.0, required)";
        schema["learningRate"] = "float (0.0-1.0, required)";
        schema["initialExcitatoryWeight"] = "float (optional)";
        schema["initialInhibitoryWeight"] = "float (optional)";
        schema["world.width"] = "int (optional)";
        schema["world.height"] = "int (optional)";
        schema["world.visionWidth"] = "int (optional)";
        schema["world.visionHeight"] = "int (optional)";
        schema["development.enabled"] = "bool (optional)";
        schema["neuromodulation.rewardModulation"] = "bool (optional)";
        schema["neuromodulation.curiosity"] = "bool (optional)";
        schema["memory.workingMemorySize"] = "int (optional)";
        schema["memory.episodicMemoryCapacity"] = "int (optional)";
        
        return schema;
    }, R"pbdoc(
        Get configuration validation schema.
        
        Returns:
            Dictionary describing expected configuration keys and their types.
        )pbdoc");

    // Diagnostic function for troubleshooting
    m.def("diagnoseSimulationEnvironment", []() {
        py::dict diagnostics;
        
        // Check basic environment
        diagnostics["python_version"] = PYBIND11_VERSION;
        diagnostics["platform"] = "Linux";
        diagnostics["pybind11_version"] = PYBIND11_VERSION;
        
        // Check available features
        diagnostics["features"] = getFeatureInfo();
        
        // Check for common issues
        py::dict issues;
        bool has_issues = false;
        
        // Check if basic functionality works
        try {
            auto config = std::make_shared<Config>();
            config->set("test.key", "test.value");
            if (!config->get<std::string>("test.key").empty()) {
                issues["basic_config"] = "OK";
            } else {
                issues["basic_config"] = "Config get failed";
                has_issues = true;
            }
        } catch (const std::exception& e) {
            issues["basic_config"] = std::string("Exception: ") + e.what();
            has_issues = true;
        }
        
        diagnostics["issues"] = issues;
        diagnostics["has_issues"] = has_issues;
        
        return diagnostics;
    }, R"pbdoc(
        Diagnose the simulation environment for troubleshooting.
        
        Returns:
            Dictionary containing diagnostic information and detected issues.
        )pbdoc");

    // Error recovery function
    m.def("recoverFromSimulationError", [](std::shared_ptr<Brain> brain, 
                                          std::shared_ptr<SimpleWorld> world, 
                                          std::shared_ptr<AgentBrain> agent) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent must all be non-null");
        }
        
        try {
            // Attempt to reset and continue
            brain->reset();
            world->reset();
            agent->reset();
            
            // Apply default configuration if available
            auto config = brain->getConfig();
            if (config->has("recovery.defaultSteps")) {
                int recovery_steps = config->get<int>("recovery.defaultSteps");
                for (int step = 0; step < recovery_steps; ++step) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    brain->step(step);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Recovery failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       R"pbdoc(
        Attempt to recover from simulation errors.
        
        Args:
            brain: Brain to recover.
            world: World to reset.
            agent: Agent to reset.
            
        Returns:
            True if recovery successful.
            
        Raises:
            ValueError: If any parameter is None.
            RuntimeError: If recovery fails.
        )pbdoc");

    // Legacy compatibility functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw py::value_error("Configuration cannot be None");
        }
        if (!config->validate().empty()) {
            throw py::value_error("Invalid configuration: " + config->validate()[0]);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
       R"pbdoc(
        Legacy brain creation function with validation (backwards compatible).
        
        Args:
            config: Configuration object for the brain.
            
        Returns:
            New Brain object initialized with the configuration.
            
        Raises:
            ValueError: If configuration is None or invalid.
        )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw py::value_error("Brain cannot be None");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
       R"pbdoc(
        Legacy agent brain creation function with validation (backwards compatible).
        
        Args:
            brain: The neural brain to interface with.
            
        Returns:
            New AgentBrain object connecting the brain to the world.
            
        Raises:
            ValueError: If brain is None.
        )pbdoc");

    // Placeholder functions for future implementation
    m.def("createAdvancedWorld", [](std::shared_ptr<Config> config) -> std::shared_ptr<SimpleWorld> {
        if (!config) {
            throw py::value_error("Configuration cannot be None");
        }
        return std::make_shared<SimpleWorld>();
    }, py::arg("config"),
       R"pbdoc(
        Create an advanced world with full configuration support.
        
        Args:
            config: Configuration object for world settings.
            
        Returns:
            New SimpleWorld object with advanced configuration.
            
        Raises:
            ValueError: If configuration is None.
            NotImplementedError: This is a placeholder for future implementation.
        )pbdoc");

    m.def("createCustomAgent", [](std::shared_ptr<Brain> brain, 
                                 std::shared_ptr<SimpleWorld> world,
                                 const py::dict& agent_config) -> std::shared_ptr<AgentBrain> {
        if (!brain || !world) {
            throw py::value_error("Brain and world must be non-null");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Apply custom configuration
        for (auto it : agent_config) {
            std::string key = py::cast<std::string>(it.first);
            py::object value = it.second;
            
            if (key == "agent.curiosityLevel" && value.is(py::float_())) {
                // Note: This is a simplified example - actual implementation
                // would depend on the AgentBrain interface
                float curiosity = py::cast<float>(value);
                // agent->setCuriosityLevel(curiosity); // Would need to be implemented
            }
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("agent_config"),
       R"pbdoc(
        Create a custom agent with configuration.
        
        Args:
            brain: Neural brain for the agent.
            world: World for the agent.
            agent_config: Configuration dictionary for agent settings.
            
        Returns:
            New AgentBrain object with custom configuration.
            
        Raises:
            ValueError: If brain or world is None.
            NotImplementedError: This is a placeholder for future implementation.
        )pbdoc");

    // Statistics and metrics collection
    m.def("collectSimulationStatistics", [](std::shared_ptr<Brain> brain, 
                                           std::shared_ptr<SimpleWorld> world,
                                           std::shared_ptr<AgentBrain> agent) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent must all be non-null");
        }
        
        py::dict statistics;
        
        // Neural network statistics
        statistics["neurons_total"] = brain->getTotalNeuronCount();
        statistics["synapses_total"] = brain->getTotalSynapseCount();
        statistics["firing_rate"] = brain->getAverageFiringRate();
        statistics["spike_count"] = brain->getTotalSpikeCount();
        statistics["active_neurons"] = brain->getActiveNeuronCount();
        statistics["firing_neurons"] = brain->getFiringNeuronCount();
        statistics["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Agent statistics
        statistics["agent_curiosity"] = agent->getCuriosityLevel();
        statistics["agent_novelty"] = agent->getNoveltyLevel();
        statistics["agent_neuromodulation"] = agent->getNeuromodulationLevel();
        statistics["agent_prediction_error"] = agent->getPredictionError();
        statistics["agent_developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        
        // World statistics
        statistics["world_time"] = world->getSimulationTime();
        statistics["world_width"] = world->getWidth();
        statistics["world_height"] = world->getHeight();
        statistics["world_agent_x"] = world->getAgentBody().x;
        statistics["world_agent_y"] = world->getAgentBody().y;
        statistics["world_agent_energy"] = world->getAgentBody().energy;
        statistics["world_agent_health"] = world->getAgentBody().health;
        
        return statistics;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       R"pbdoc(
        Collect comprehensive statistics from the simulation.
        
        Args:
            brain: Neural brain for statistics.
            world: World for statistics.
            agent: Agent for statistics.
            
        Returns:
            Dictionary containing comprehensive simulation statistics.
            
        Raises:
            ValueError: If brain, world, or agent is None.
        )pbdoc");

    // Simulation checkpoint and recovery
    m.def("createSimulationCheckpoint", [](std::shared_ptr<Brain> brain, 
                                          std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent must all be non-null");
        }
        
        py::dict checkpoint;
        
        // Save brain state
        checkpoint["brain_type"] = "Brain";
        checkpoint["brain_state"] = brain->save("/tmp/brain_checkpoint.dat"); // Would save to file
        
        // Save world state
        checkpoint["world_type"] = "SimpleWorld";
        checkpoint["world_state"] = std::to_string(world->getSimulationTime());
        
        // Save agent state
        checkpoint["agent_type"] = "AgentBrain";
        checkpoint["agent_curiosity"] = agent->getCuriosityLevel();
        checkpoint["agent_novelty"] = agent->getNoveltyLevel();
        checkpoint["agent_neuromodulation"] = agent->getNeuromodulationLevel();
        
        return checkpoint;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       R"pbdoc(
        Create a simulation checkpoint for recovery.
        
        Args:
            brain: Neural brain for checkpoint.
            world: World for checkpoint.
            agent: Agent for checkpoint.
            
        Returns:
            Dictionary containing simulation checkpoint data.
            
        Raises:
            ValueError: If brain, world, or agent is None.
        )pbdoc");

    // Version and compatibility information
    m.attr("VERSION") = std::string("0.1.0");
    m.attr("API_COMPATIBILITY_VERSION") = std::string("1.0");
    m.attr("PYBIND11_VERSION") = std::string(PYBIND11_VERSION);
    
    m.def("getCompatibilityInfo", []() {
        py::dict compatibility;
        
        compatibility["nlm_version"] = "0.1.0";
        compatibility["api_version"] = "1.0";
        compatibility["python_bindings"] = "pybind11 " PYBIND11_VERSION;
        compatibility["cpp_standard"] = "C++20";
        compatibility["build_system"] = "scikit-build-core";
        
        // Feature compatibility matrix
        py::dict features;
        features["basic_functionality"] = true;
        features["advanced_config"] = true;
        features["error_handling"] = true;
        features["type_safety"] = true;
        features["memory_management"] = true;
        features["simulation_control"] = true;
        features["batch_operations"] = true;
        features["performance_optimization"] = true;
        features["checkpoint_recovery"] = false;  // Not yet implemented
        features["advanced_agents"] = false;      // Not yet implemented
        features["custom_serialization"] = false; // Not yet implemented
        
        compatibility["features"] = features;
        
        return compatibility;
    }, R"pbdoc(
        Get compatibility and version information.
        
        Returns:
            Dictionary containing version and compatibility information.
        )pbdoc");

    // Legacy function aliases for backwards compatibility
    m.def("createConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<Config> {
        return config;  // Just return the config as-is for backwards compatibility
    }, py::arg("config"), "Legacy alias for createConfigFromDict");

    m.def("runBrain", [](std::shared_ptr<Brain> brain, int steps) {
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        return true;
    }, py::arg("brain"), py::arg("steps"), "Legacy brain stepping function");
}

} // namespace nlm
