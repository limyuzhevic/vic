#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

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
        
        This module provides Python bindings for the NLM (Neural Learning Machine) project,
        an experimental artificial developmental brain simulation. The project implements
        spiking neural networks with integrated memory, prediction, cognition, and neuromodulation
        systems. NLM uses Leaky Integrate-and-Fire (LIF) neurons with event-driven spike
        propagation and supports STDP, Hebbian learning, structural plasticity, and development.
        
        ## Quick Examples
        
        ### Basic Brain Simulation
        ```python
        import pynlm
        
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```
        
        ### Complete Agent Simulation
        ```python
        import pynlm
        
        def run_agent_simulation(steps=500):
            # Setup
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            return brain, agent, world
        
        # Run simulation
        brain, agent, world = run_agent_simulation(1000)
        print(f"Simulation complete. Final curiosity: {agent.getCuriosityLevel():.3f}")
        ```
        
        ### Configuration Management
        ```python
        import pynlm
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Set values using different methods
        config.set("brain.neuron_count", 1000)
        config.set("learning.rate", 0.001)
        config.set("plasticity.enabled", True)
        
        # Use convenience methods
        config.setMultiple({
            "brain.neuron_count": 2000,
            "brain.synapse_density": 0.15,
            "neuromod.curiosity.enabled": True
        })
        
        # Get values with type conversion
        neurons = config.get("brain.neuron_count", "int")
        learning_rate = config.get("learning.rate", "float")
        plasticity_enabled = config.get("plasticity.enabled", "bool")
        
        # Validate configuration
        if config.validateKey("brain.neuron_count", "neuron_*"):
            print("Key validation passed")
        
        if config.validateType("brain.neuron_count", "int"):
            print("Type validation passed")
        
        # Save and load configuration
        config.saveToFile("my_config.json")
        config.loadFromFile("my_config.json")
        ```
        
        ### Sensory Input Processing
        ```python
        import pynlm
        
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create different sensory inputs
        vision = pynlm.Vision(8, 8, 3)
        vision.setData([0.1, 0.2, 0.3] * 8)
        
        audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
        audio.setData([0.0, 0.5, 0.0, -0.5] * 256)
        
        # Process sensory input
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        percept.setAudio(audio)
        
        agent.processSensoryInput(percept)
        brain.step(0)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        ```
        
        ### Error Handling
        ```python
        import pynlm
        
        def safe_operation():
            try:
                config = pynlm.createDefaultConfig()
                
                # Safe configuration operations
                if config.validateKey("brain.neuron_count", "neuron_*"):
                    config.set("brain.neuron_count", 1000)
                
                if config.validateType("brain.neuron_count", "int"):
                    neurons = config.get("brain.neuron_count", "int")
                    print(f"Neurons: {neurons}")
                
                # File operations with error handling
                config.saveToFile("config.json")
                
            except Exception as e:
                print(f"Error: {e}")
                return False
            
            return True
        
        # Run safe operation
        if safe_operation():
            print("Operation completed successfully")
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Create custom NLM exceptions
    py::class_<std::runtime_error>(m, "RuntimeError");  // For backward compatibility
    
    // Add NLM-specific exception class
    py::class_<std::runtime_error>(m, "NLMError")
        .def(py::init<const std::string&>());
    
    // Helper function to translate C++ exceptions to Python
    auto translate_exceptions = [](const std::function<void()>& func) {
        try {
            func();
        } catch (const std::runtime_error& e) {
            throw pybind11::python_exception<std::runtime_error>(e);
        } catch (...) {
            throw pybind11::python_exception<std::runtime_error>("Unknown error");
        }
    };

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

        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                bool success = self.loadFromFile(filepath);
                if (!success) {
                    throw py::runtime_error("Failed to load configuration from file: " + filepath);
                }
                return success;
            } catch (const std::exception& e) {
                throw py::runtime_error("Error loading configuration from file " + filepath + ": " + std::string(e.what()));
            }
        }, py::arg("filepath"),
             "Load configuration from a JSON file with error handling")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                bool success = self.loadFromArgs(argc, argv);
                if (!success) {
                    throw py::runtime_error("Failed to load configuration from command line arguments");
                }
                return success;
            } catch (const std::exception& e) {
                throw py::runtime_error("Error loading configuration from command line arguments: " + std::string(e.what()));
            }
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments with error handling")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                bool success = self.saveToFile(filepath);
                if (!success) {
                    throw py::runtime_error("Failed to save configuration to file: " + filepath);
                }
                return success;
            } catch (const std::exception& e) {
                throw py::runtime_error("Error saving configuration to file " + filepath + ": " + std::string(e.what()));
            }
        }, py::arg("filepath"),
             "Save configuration to a JSON file with error handling")
        .def("set", [](Config& self, const std::string& key, const py::object& value, const std::string& sourceHint = "Runtime") {
            // Convert Python value to appropriate C++ type
            Config::ConfigSource source;
            if (sourceHint == "Default") {
                source = Config::ConfigSource::Default;
            } else if (sourceHint == "File") {
                source = Config::ConfigSource::File;
            } else if (sourceHint == "CommandLine") {
                source = Config::ConfigSource::CommandLine;
            } else {
                source = Config::ConfigSource::Runtime;
            }
            
            try {
                if (py::isinstance<py::int_>(value)) {
                    self.set(key, py::int_(value).cast<int>(), source);
                } else if (py::isinstance<py::float_>(value)) {
                    self.set(key, py::float_(value).cast<double>(), source);
                } else if (py::isinstance<py::bool_>(value)) {
                    self.set(key, py::bool_(value).cast<bool>(), source);
                } else if (py::isinstance<py::str>(value)) {
                    self.set(key, py::str(value).cast<std::string>(), source);
                } else if (py::isinstance<py::list>(value)) {
                    py::list list_value = py::list(value);
                    // Try to determine if it's int or double list
                    bool is_int_list = true;
                    std::vector<double> double_vec;
                    
                    for (size_t i = 0; i < list_value.size(); ++i) {
                        try {
                            double val = py::float_(list_value[i]).cast<double>();
                            double_vec.push_back(val);
                            if (val != static_cast<int>(val)) {
                                is_int_list = false;
                            }
                        } catch (...) {
                            is_int_list = false;
                        }
                    }
                    
                    if (is_int_list) {
                        std::vector<int> int_vec;
                        for (double val : double_vec) {
                            int_vec.push_back(static_cast<int>(val));
                        }
                        self.set(key, int_vec, source);
                    } else {
                        self.set(key, double_vec, source);
                    }
                } else {
                    throw py::type_error("Unsupported value type for configuration key: " + key);
                }
                
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error("Error setting configuration key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("value"), py::arg("sourceHint") = "Runtime",
             "Set a configuration value with type conversion and error handling")
        .def("get", [](const Config& self, const std::string& key, const std::string& typeHint = "auto") -> py::object {
            try {
                if (!self.has(key)) {
                    return py::none();
                }
                
                if (typeHint == "int") {
                    auto val = self.get<int>(key);
                    if (val.has_value()) {
                        return py::cast(*val);
                    }
                } else if (typeHint == "double" || typeHint == "float") {
                    auto val = self.get<double>(key);
                    if (val.has_value()) {
                        return py::cast(*val);
                    }
                } else if (typeHint == "bool") {
                    auto val = self.get<bool>(key);
                    if (val.has_value()) {
                        return py::cast(*val);
                    }
                } else if (typeHint == "str" || typeHint == "string") {
                    auto val = self.get<std::string>(key);
                    if (val.has_value()) {
                        return py::cast(*val);
                    }
                } else if (typeHint == "int_list" || typeHint == "int_vector") {
                    auto val = self.get<std::vector<int>>(key);
                    if (val.has_value()) {
                        return py::cast(*val);
                    }
                } else if (typeHint == "float_list" || typeHint == "double_vector") {
                    auto val = self.get<std::vector<double>>(key);
                    if (val.has_value()) {
                        return py::cast(*val);
                    }
                } else if (typeHint == "str_list" || typeHint == "string_vector") {
                    auto val = self.get<std::vector<std::string>>(key);
                    if (val.has_value()) {
                        return py::cast(*val);
                    }
                } else {
                    // Auto-detect type
                    if (auto val = self.get<int>(key)) {
                        return py::cast(*val);
                    } else if (auto val = self.get<double>(key)) {
                        return py::cast(*val);
                    } else if (auto val = self.get<bool>(key)) {
                        return py::cast(*val);
                    } else if (auto val = self.get<std::string>(key)) {
                        return py::cast(*val);
                    }
                }
                
                return py::none();
            } catch (const std::exception& e) {
                throw py::runtime_error("Error getting configuration key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("typeHint") = "auto",
             "Get a configuration value with type conversion and error handling")
        .def("getOr", [](const Config& self, const std::string& key, const py::object& defaultValue, const std::string& typeHint = "auto") -> py::object {
            try {
                if (self.has(key)) {
                    return self.get(key, typeHint);
                }
                return defaultValue;
            } catch (const std::exception& e) {
                throw py::runtime_error("Error getting configuration key '" + key + "' or default value: " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("defaultValue"), py::arg("typeHint") = "auto",
             "Get a configuration value or return a default if key doesn't exist")
        .def("remove", [](Config& self, const std::string& key) {
            try {
                if (!self.has(key)) {
                    throw py::key_error("Configuration key does not exist: " + key);
                }
                self.remove(key);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error("Error removing configuration key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"),
             "Remove a configuration key with error handling")
        .def("clear", [](Config& self) {
            try {
                self.clear();
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error("Error clearing configuration: " + std::string(e.what()));
            }
        },
             "Clear all configuration entries with error handling")
        .def("has", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", [](const Config& self) {
            return self.getKeys();
        },
             "Get all configuration keys")
        .def("summary", [](const Config& self) {
            return self.summary();
        },
             "Get a summary string of the configuration")
        // Convenience methods for multiple operations
        .def("setMultiple", [](Config& self, const std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>& values) {
            for (const auto& pair : values) {
                std::visit([&self, &pair](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, int>)
                        self.set(pair.first, val);
                    else if constexpr (std::is_same_v<T, double>)
                        self.set(pair.first, val);
                    else if constexpr (std::is_same_v<T, bool>)
                        self.set(pair.first, val);
                    else if constexpr (std::is_same_v<T, std::string>)
                        self.set(pair.first, val);
                    else if constexpr (std::is_same_v<T, std::vector<int>>)
                        self.set(pair.first, val);
                    else if constexpr (std::is_same_v<T, std::vector<double>>)
                        self.set(pair.first, val);
                    else if constexpr (std::is_same_v<T, std::vector<std::string>>)
                        self.set(pair.first, val);
                }, pair.second);
            }
        }, py::arg("values"),
           "Set multiple configuration values at once")
        .def("getWithType", [](const Config& self, const std::string& key, const std::string& typeHint) -> py::object {
            if (!self.has(key)) {
                return py::none();
            }
            
            if (typeHint == "int") {
                auto val = self.get<int>(key);
                if (val.has_value()) {
                    return py::cast(*val);
                }
            } else if (typeHint == "double") {
                auto val = self.get<double>(key);
                if (val.has_value()) {
                    return py::cast(*val);
                }
            } else if (typeHint == "bool") {
                auto val = self.get<bool>(key);
                if (val.has_value()) {
                    return py::cast(*val);
                }
            } else if (typeHint == "str" || typeHint == "string") {
                auto val = self.get<std::string>(key);
                if (val.has_value()) {
                    return py::cast(*val);
                }
            } else if (typeHint == "int_list" || typeHint == "int_vector") {
                auto val = self.get<std::vector<int>>(key);
                if (val.has_value()) {
                    return py::cast(*val);
                }
            } else if (typeHint == "float_list" || typeHint == "double_vector") {
                auto val = self.get<std::vector<double>>(key);
                if (val.has_value()) {
                    return py::cast(*val);
                }
            } else if (typeHint == "str_list" || typeHint == "string_vector") {
                auto val = self.get<std::vector<std::string>>(key);
                if (val.has_value()) {
                    return py::cast(*val);
                }
            }
            return py::none();
        }, py::arg("key"), py::arg("typeHint"),
           "Get a configuration value with type hint (returns None if not found or type mismatch)")
        .def("validateKey", [](const Config& self, const std::string& key, const std::string& pattern) {
            if (!self.has(key)) {
                return false;
            }
            
            // Basic validation: check if key contains only valid characters
            // Allow alphanumeric, underscore, dash, and dot
            for (char c : key) {
                if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
                    return false;
                }
            }
            
            // If pattern is provided, perform basic pattern matching
            if (!pattern.empty()) {
                // Simple pattern validation - could be extended
                if (pattern == "neuron_*" && key.find("neuron_") == 0) {
                    return true;
                } else if (pattern == "synapse_*" && key.find("synapse_") == 0) {
                    return true;
                } else if (pattern == "learning_*" && key.find("learning_") == 0) {
                    return true;
                } else if (pattern == "brain_*" && key.find("brain_") == 0) {
                    return true;
                } else if (pattern == "network_*" && key.find("network_") == 0) {
                    return true;
                } else {
                    // For other patterns, just check that key is not empty
                    return !key.empty();
                }
            }
            
            return true;
        }, py::arg("key"), py::arg("pattern"),
           "Validate a configuration key against a pattern")
        .def("validateType", [](const Config& self, const std::string& key, const std::string& expectedType) {
            if (!self.has(key)) {
                return false;
            }
            
            // Try to get the value and check its type
            if (expectedType == "int") {
                auto val = self.get<int>(key);
                return val.has_value();
            } else if (expectedType == "double" || expectedType == "float") {
                auto val = self.get<double>(key);
                return val.has_value();
            } else if (expectedType == "bool") {
                auto val = self.get<bool>(key);
                return val.has_value();
            } else if (expectedType == "str" || expectedType == "string") {
                auto val = self.get<std::string>(key);
                return val.has_value();
            } else if (expectedType == "int_list" || expectedType == "int_vector") {
                auto val = self.get<std::vector<int>>(key);
                return val.has_value();
            } else if (expectedType == "float_list" || expectedType == "double_vector") {
                auto val = self.get<std::vector<double>>(key);
                return val.has_value();
            } else if (expectedType == "str_list" || expectedType == "string_vector") {
                auto val = self.get<std::vector<std::string>>(key);
                return val.has_value();
            }
            return false;
        }, py::arg("key"), py::arg("expectedType"),
           "Validate that a configuration key has an expected type")
        .def("getConfigInfo", [](const Config& self) {
            std::unordered_map<std::string, std::string> info;
            auto keys = self.getKeys();
            
            for (const auto& key : keys) {
                if (self.has(key)) {
                    // Get type hint for the value
                    if (auto val = self.get<int>(key)) {
                        info[key] = "int: " + std::to_string(*val);
                    } else if (auto val = self.get<double>(key)) {
                        info[key] = "float: " + std::to_string(*val);
                    } else if (auto val = self.get<bool>(key)) {
                        info[key] = "bool: " + std::string(*val ? "true" : "false");
                    } else if (auto val = self.get<std::string>(key)) {
                        info[key] = "string: " + *val;
                    } else if (auto val = self.get<std::vector<int>>(key)) {
                        info[key] = "int_list: [" + std::to_string(val->size()) + " elements]";
                    } else if (auto val = self.get<std::vector<double>>(key)) {
                        info[key] = "float_list: [" + std::to_string(val->size()) + " elements]";
                    } else if (auto val = self.get<std::vector<std::string>>(key)) {
                        info[key] = "string_list: [" + std::to_string(val->size()) + " elements]";
                    } else {
                        info[key] = "unknown";
                    }
                }
            }
            
            return info;
        }, "Get configuration keys and their types as a dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Add string representation methods
        .def("__repr__", [](const SensoryInput& input) {
            std::string typeName = "Unknown";
            // Map enum values to strings (simplified)
            typeName = "SensoryInput";
            return "<SensoryInput: " + typeName + " at " + 
                   std::to_string(input.getTimestamp()) + ">";
        })
        .def("__str__", [](const SensoryInput& input) {
            std::string typeName = "SensoryInput";
            return typeName + " at " + std::to_string(input.getTimestamp());
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Add string representation methods
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" + 
                   std::to_string(vision.getChannels()) + ">";
        })
        .def("__str__", [](const Vision& vision) {
            return "Vision " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" + 
                   std::to_string(vision.getChannels());
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Add string representation methods
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + 
                   " Hz, " + std::to_string(audio.getNumSamples()) + 
                   " samples>";
        })
        .def("__str__", [](const Audio& audio) {
            return "Audio " + std::to_string(audio.getSampleRate()) + 
                   " Hz, " + std::to_string(audio.getNumSamples()) + 
                   " samples";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Add string representation methods
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: " + std::to_string(signals.getDimensions()) + 
                   " signals>";
        })
        .def("__str__", [](const InternalSignals& signals) {
            return "InternalSignals (" + std::to_string(signals.getDimensions()) + 
                   " signals)";
        });

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Add string representation methods
        .def("__repr__", [](const Action& action) {
            std::string paramsStr = "";
            if (!action.getParameters().empty()) {
                paramsStr = " params=[";
                for (size_t i = 0; i < action.getParameters().size(); ++i) {
                    if (i > 0) paramsStr += ", ";
                    paramsStr += std::to_string(action.getParameters()[i]);
                }
                paramsStr += "]";
            }
            return "<Action: " + action.getName() + paramsStr + ">";
        })
        .def("__str__", [](const Action& action) {
            std::string paramsStr = "";
            if (!action.getParameters().empty()) {
                paramsStr = " [";
                for (size_t i = 0; i < action.getParameters().size(); ++i) {
                    if (i > 0) paramsStr += ", ";
                    paramsStr += std::to_string(action.getParameters()[i]);
                }
                paramsStr += "]";
            }
            return action.getName() + paramsStr;
        });

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + " (time=" + 
                   std::to_string(world.getSimulationTime()) + ")>";
        })
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + ", time=" + 
                   std::to_string(world.getSimulationTime());
        });

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
             "Log brain status")
        // Add string representation methods
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses, stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) +
                   ">";
        })
        .def("__str__", [](const Brain& brain) {
            return "Brain " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getFiringNeuronCount()) +
                   " firing, " + std::to_string(brain.getTotalSpikeCount()) +
                   " spikes";
        });

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Add string representation methods
        .def("__repr__", [](const AgentBrain& agent) {
            auto* brain = agent.getBrain();
            return "<AgentBrain: brain with " + std::to_string(brain ? brain->getTotalNeuronCount() : 0) + 
                   " neurons, neuromodulation=" + std::to_string(agent.getNeuromodulationLevel()) +
                   ", curiosity=" + std::to_string(agent.getCuriosityLevel()) + ">";
        })
        .def("__str__", [](const AgentBrain& agent) {
            auto* brain = agent.getBrain();
            return "AgentBrain (stage=" + std::to_string(agent.getDevelopmentalStage()) +
                   ", neuromodulation=" + std::to_string(agent.getNeuromodulationLevel()) +
                   ", curiosity=" + std::to_string(agent.getCuriosityLevel()) + ")";
        });

    // Add validation utilities
    m.def("validate_key", [](const std::string& key) {
        // Validate configuration key format
        if (key.empty()) {
            throw py::value_error("Configuration key cannot be empty");
        }
        if (key.length() > 100) {
            throw py::value_error("Configuration key cannot be longer than 100 characters");
        }
        
        // Check for valid characters (alphanumeric, underscore, dash, dot)
        for (char c : key) {
            if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
                throw py::value_error("Configuration key contains invalid character: " + std::string(1, c));
            }
        }
        
        return true;
    }, py::arg("key"), "Validate a configuration key format");
    
    m.def("validate_value", [](const std::string& type, const py::object& value) {
        // Validate configuration value based on type
        if (type == "int") {
            try {
                py::int_(value);
                return true;
            } catch (...) {
                throw py::type_error("Value must be an integer for type 'int'");
            }
        } else if (type == "float" || type == "double") {
            try {
                py::float_(value);
                return true;
            } catch (...) {
                throw py::type_error("Value must be a float/double for type 'float' or 'double'");
            }
        } else if (type == "bool") {
            try {
                py::bool_(value);
                return true;
            } catch (...) {
                throw py::type_error("Value must be a boolean for type 'bool'");
            }
        } else if (type == "str" || type == "string") {
            try {
                py::str(value);
                return true;
            } catch (...) {
                throw py::type_error("Value must be a string for type 'str' or 'string'");
            }
        } else if (type == "list" || type == "vector") {
            try {
                py::list(value);
                return true;
            } catch (...) {
                throw py::type_error("Value must be a list/vector for type 'list' or 'vector'");
            }
        }
        return true;  // Unknown type, assume valid
    }, py::arg("type"), py::arg("value"), "Validate a configuration value against a type");
    
    m.def("parse_config_value", [](const std::string& type, const py::object& value) -> py::object {
        // Parse Python value to appropriate C++ type
        if (type == "int") {
            return py::cast(py::int_(value).cast<long>());
        } else if (type == "float" || type == "double") {
            return py::cast(py::float_(value).cast<double>());
        } else if (type == "bool") {
            return py::cast(py::bool_(value).cast<bool>());
        } else if (type == "str" || type == "string") {
            return py::cast(py::str(value).cast<std::string>());
        } else if (type == "int_list" || type == "int_vector") {
            py::list list_value = py::list(value);
            std::vector<int> vec;
            for (size_t i = 0; i < list_value.size(); ++i) {
                vec.push_back(py::int_(list_value[i]).cast<int>());
            }
            return py::cast(vec);
        } else if (type == "float_list" || type == "double_vector") {
            py::list list_value = py::list(value);
            std::vector<double> vec;
            for (size_t i = 0; i < list_value.size(); ++i) {
                vec.push_back(py::float_(list_value[i]).cast<double>());
            }
            return py::cast(vec);
        } else if (type == "str_list" || type == "string_vector") {
            py::list list_value = py::list(value);
            std::vector<std::string> vec;
            for (size_t i = 0; i < list_value.size(); ++i) {
                vec.push_back(py::str(list_value[i]).cast<std::string>());
            }
            return py::cast(vec);
        }
        return py::none();
    }, py::arg("type"), py::arg("value"), "Parse a Python value to the specified C++ type");
    
    m.def("safe_config_operation", [](std::function<void()> operation, const std::string& operationName) {
        // Wrapper for safe configuration operations with error handling
        try {
            operation();
            return true;
        } catch (const py::error_already_set& e) {
            // Python exception already set, rethrow it
            throw;
        } catch (const std::exception& e) {
            throw py::runtime_error("Error in " + operationName + ": " + std::string(e.what()));
        } catch (...) {
            throw py::runtime_error("Unknown error in " + operationName);
        }
    }, py::arg("operation"), py::arg("operationName"), "Safely execute a configuration operation with error handling");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
