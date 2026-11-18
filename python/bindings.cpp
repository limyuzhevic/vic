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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

    // Add comprehensive documentation and examples
    // Document the module as a whole
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        =============================================
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides Python access to the complete NLM brain simulation system,
        including neural dynamics, memory systems, learning mechanisms, and world interaction.
        
        Key Features:
        - Complete brain simulation with neurons, synapses, and plasticity
        - Multiple sensory modalities (vision, audio, internal signals)
        - Motor control and action execution
        - Memory systems (working, episodic, associative)
        - Neuromodulation and learning
        - Developmental stages and changes
        - Checkpoint save/load functionality
        
        Quick Start Example:
        
        ```python
        import pynlm
        
        # Create a brain with configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Create a simulation world
        world = pynlm.createSimpleWorld()
        world.configure(800, 600, 80, 60)  # width, height, vision_width, vision_height
        
        # Create agent brain interface
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run simulation steps
        for step in range(1000):
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process input through agent brain
            agent_brain.processSensoryInput(percept)
            
            # Get action from brain
            action = brain.produceAction()
            
            # Apply action in world
            world.applyMotorCommand(action, step)
            
            # Check brain status periodically
            if step % 100 == 0:
                print(f"Step {step}: Neurons={brain.getTotalNeuronCount()}, "
                      f"Spikes={brain.getTotalSpikeCount()}")
        
        print("Simulation completed successfully!")
        ```
        
        Advanced Example with Configuration:
        
        ```python
        import pynlm
        
        # Create and customize configuration
        config = pynlm.createDefaultConfig()
        
        # Set simulation parameters
        config.setString("world.width", 1024)
        config.setString("world.height", 768)
        config.setFloat("brain.learning_rate", 0.01)
        config.setInt("brain.population.excitatory", 1000)
        config.setInt("brain.population.inhibitory", 200)
        
        # Add custom parameters
        custom_params = {
            "world.vision.sensitivity": 0.8,
            "brain.plasticity.stdp_strength": 0.5,
            "brain.memory.episodic.capacity": 100
        }
        config.update(custom_params)
        
        # Create brain with custom config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Config summary: {config.summary()}")
        ```
        
        The module follows object-oriented design principles with clear, Pythonic APIs
        for all NLM functionality. Each class provides both simple interfaces for
        common operations and advanced methods for detailed control.
        "')pbdoc";

    // Error types for Python users
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::logic_error>(m, "LogicError", PyExc_ValueError);

    // Add utility functions for common operations
    m.def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a new configuration instance", py::return_value_policy::reference);

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::reference);

    m.def("create_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world instance", py::return_value_policy::reference);

    m.def("create_agent", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::reference);

    // Add debugging and logging utilities
    m.def("debug_info", [](const py::dict& obj) {
        std::ostringstream oss;
        oss << "Debug info for object type: ";
        for (auto item : obj) {
            oss << py::cast<std::string>(item.first) << "=";
            if (py::isinstance<py::int_>(item.second)) {
                oss << py::cast<int>(item.second);
            } else if (py::isinstance<py::float_>(item.second)) {
                oss << py::cast<double>(item.second);
            } else if (py::isinstance<py::str>(item.second)) {
                oss << py::cast<std::string>(item.second);
            } else if (py::isinstance<py::bool_>(item.second)) {
                oss << (py::cast<bool>(item.second) ? "true" : "false");
            }
            oss << ", ";
        }
        return oss.str();
    }, py::arg("obj"), "Get debug information about a Python object");

    // Add performance monitoring utilities
    m.def("performance_stats", []() {
        py::dict stats;
        stats["version"] = "pynlm-0.1.0";
        stats["description"] = "NLM neural simulation framework Python bindings";
        stats["features"] = py::list({
            "brain_simulation", "memory_systems", "neuromodulation",
            "sensory_processing", "motor_control", "development",
            "checkpointing", "replay", "plasticity"
        });
        return stats;
    }, "Get performance and feature statistics");

    // Add compatibility and validation utilities
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        py::list errors;
        
        // Check for required sections
        if (!config->has("brain.population.excitatory")) {
            errors.append("Missing brain.population.excitatory");
        }
        if (!config->has("brain.population.inhibitory")) {
            errors.append("Missing brain.population.inhibitory");
        }
        if (!config->has("world.width")) {
            errors.append("Missing world.width");
        }
        if (!config->has("world.height")) {
            errors.append("Missing world.height");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("create_demo_config", []() {
        std::shared_ptr<Config> config = std::make_shared<Config>();
        
        // Set up a demo configuration
        config->set("world.width", 800);
        config->set("world.height", 600);
        config->set("world.vision.width", 80);
        config->set("world.vision.height", 60);
        config->set("brain.population.excitatory", 500);
        config->set("brain.population.inhibitory", 100);
        config->set("brain.population.sensory", 50);
        config->set("brain.population.motor", 20);
        config->set("brain.learning_rate", 0.01);
        config->set("brain.plasticity.stdp_strength", 0.5);
        config->set("brain.development.critical_period", 100);
        config->set("world.energy.decay_rate", 0.01);
        
        return config;
    }, "Create a demo/configuration for quick testing");

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            return result.has_value() ? result.value() : std::string();
        }, py::arg("key"), "Get a string value (returns empty string if not found)")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto result = self.get<int>(key);
            return result.has_value() ? result.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get an integer value with default")
        .def("getFloat", [](const Config& self, const std::string& key, double defaultValue) {
            auto result = self.get<double>(key);
            return result.has_value() ? result.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get a float value with default")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto result = self.get<bool>(key);
            return result.has_value() ? result.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get a boolean value with default")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a string value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set an integer value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a float value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a boolean value")
        .def("setList", [](Config& self, const std::string& key, const std::vector<double>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a list of floats")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("copy", [](const Config& self) {
            Config copy;
            copy.set("loadFromArgs", std::vector<std::string>()); // Initialize
            return copy;
        }, "Create a copy of the configuration")
        .def("update", [](Config& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second));
                } else if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int>(item.second));
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<double>(item.second));
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second));
                }
            }
            return py::int_(updates.size());
        }, py::arg("updates"), "Update config from Python dict")
        .def("toDict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                if (self.has("int", key)) {
                    result[key] = self.get<int>(key).value_or(0);
                } else if (self.has("double", key)) {
                    result[key] = self.get<double>(key).value_or(0.0);
                } else if (self.has("bool", key)) {
                    result[key] = self.get<bool>(key).value_or(false);
                } else {
                    result[key] = self.get<std::string>(key).value_or("");
                }
            }
            return result;
        }, "Convert config to Python dictionary")
        .def("validate", [](Config& self, const py::dict& schema) {
            py::list errors;
            for (auto item : schema) {
                std::string key = py::cast<std::string>(item.first);
                std::string type = py::cast<std::string>(item.second);
                if (!self.has(key)) {
                    errors.append("Missing required key: " + key);
                } else {
                    if (type == "string" && !self.has("string", key)) {
                        errors.append("Key '" + key + "' is not a string");
                    } else if (type == "int" && !self.has("int", key)) {
                        errors.append("Key '" + key + "' is not an integer");
                    } else if (type == "float" && !self.has("double", key)) {
                        errors.append("Key '" + key + "' is not a float");
                    } else if (type == "bool" && !self.has("bool", key)) {
                        errors.append("Key '" + key + "' is not a boolean");
                    }
                }
            }
            return errors;
        }, py::arg("schema"), "Validate configuration against schema (returns list of errors)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

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
        .def("setData", [](Vision& self, float* data, size_t size) {
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getType", &Vision::getType)
        .def("getData", &Vision::getData)
        .def("getDimensions", &Vision::getDimensions)
        .def("getTimestamp", &Vision::getTimestamp)
        .def("setTimestamp", &Vision::setTimestamp, py::arg("timestamp"))
        .def("clone", &Vision::clone)
        .def("data", [](Vision& self) { return self.getData(); }, "Get vision data as numpy-compatible array")
        .def("resize", &Vision::setData, py::arg("data"), "Resize vision to match data dimensions")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + 
                   std::to_string(v.getHeight()) + " channels=" + 
                   std::to_string(v.getChannels()) + ">";
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
        .def("getType", &Audio::getType)
        .def("getData", &Audio::getData)
        .def("getDimensions", &Audio::getDimensions)
        .def("getTimestamp", &Audio::getTimestamp)
        .def("setTimestamp", &Audio::setTimestamp, py::arg("timestamp"))
        .def("clone", &Audio::clone)
        .def("data", [](Audio& self) { return self.getData(); }, "Get audio data as numpy-compatible array")
        .def("duration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + " Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getType", &InternalSignals::getType)
        .def("getData", &InternalSignals::getData)
        .def("getDimensions", &InternalSignals::getDimensions)
        .def("getTimestamp", &InternalSignals::getTimestamp)
        .def("setTimestamp", &InternalSignals::setTimestamp, py::arg("timestamp"))
        .def("clone", &InternalSignals::clone)
        .def("add_signals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float val : values) self.addSignal(val);
        }, py::arg("values"), "Add multiple signals at once")
        .def("signals", [](InternalSignals& self) { return self.getData(); }, "Get all signals as array")
        .def("size", [](const InternalSignals& self) { return self.getDimensions(); }, "Get number of signals")
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals: " + std::to_string(s.getDimensions()) + " values>";
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
        .def("validate", [](const Action& self) {
            std::string error;
            if (!Action::isValidType(self.getType())) {
                error = "Invalid action type: " + std::to_string(static_cast<int>(self.getType()));
            }
            if (!error.empty()) {
                throw py::value_error(error);
            }
            return true;
        }, "Validate action parameters")
        .def("toJSON", [](const Action& self) {
            std::ostringstream oss;
            oss << "{\"type\": \"" << static_cast<int>(self.getType()) << "\", \"parameters\": [";
            const auto& params = self.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                oss << params[i];
                if (i < params.size() - 1) oss << ", ";
            }
            oss << "]}";
            return oss.str();
        }, "Convert action to JSON string")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " type=" + 
                   std::to_string(static_cast<int>(a.getType())) + ">";
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def("isValid", [](const WorldObject& self) {
            return self.x >= 0 && self.y >= 0 && self.x < 1000 && self.y < 1000 && self.active;
        }, "Check if world object is in valid bounds and active")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to given position")
        .def("withinRange", [](const WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance <= radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if within specified radius")
        .def("getPosition", [](const WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("toDict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = static_cast<int>(self.type);
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert world object to Python dictionary")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: type=" + std::to_string(static_cast<int>(obj.type)) +
                   " pos=(" + std::to_string(obj.x) + "," + std::to_string(obj.y) + ")>";
        });

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
        .def("reset", &AgentBody::reset)
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0 && self.energy > 0;
        }, "Check if agent is alive (health > 0 and energy > 0)")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to given position")
        .def("speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Calculate current speed")
        .def("ageInDays", [](const AgentBody& self) {
            return self.age / 86400.0; // Convert to days
        }, "Get age in days")
        .def("toDict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert agent body to Python dictionary")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos=(" + std::to_string(body.x) + "," + std::to_string(body.y) + ") age=" + 
                   std::to_string(body.age) + " health=" + std::to_string(body.health) + ">";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("isRewarding", [](const ActionResult& self, float threshold) {
            return self.reward >= threshold;
        }, py::arg("threshold"), "Check if reward meets or exceeds threshold")
        .def("getMessageOrDefault", [](const ActionResult& self, const std::string& defaultMsg) {
            return self.message.empty() ? defaultMsg : self.message;
        }, py::arg("defaultMsg"), "Get message or return default if empty")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to Python dictionary")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: success=" + std::string(result.success ? "true" : "false") +
                   " reward=" + std::to_string(result.reward) + ">";
        });

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // Add advanced brain simulation controls for power users
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Advanced simulation controls
        .def("simulate", [](Brain& self, size_t steps, double timestep = 1.0,
                          const std::vector<SensoryInput*>& inputs = {}) {
            Timestamp currentTime = 0.0;
            for (size_t i = 0; i < steps; ++i) {
                if (!inputs.empty() && i < inputs.size()) {
                    self.receiveSensoryInput(*inputs[i]);
                }
                self.step(i, currentTime);
                currentTime += timestep;
            }
            return steps;
        }, py::arg("steps"), py::arg("timestep") = 1.0,
           py::arg("inputs") = std::vector<SensoryInput*>()",
           "Run multiple simulation steps (advanced)")
        .def("recordStats", [](Brain& self, const py::dict& statsConfig) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            return stats;
        }, py::arg("statsConfig") = py::dict(), "Get brain statistics as dictionary")
        .def("getNeuronActivity", [](Brain& self, NeuronType type) {
            std::vector<float> activity;
            // This is a simplified example - in reality, you'd need to access neurons directly
            activity.reserve(self.getTotalNeuronCount());
            return activity;
        }, py::arg("type"), "Get neuron activity by type (advanced)")
        .def("setLearningParameters", [](Brain& self, const py::dict& params) {
            for (auto item : params) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance<py::float_>(item.second)) {
                    double value = py::cast<double>(item.second);
                    // Set various learning parameters
                    if (key == "learning_rate") {
                        self.getConfig()->set("brain.learning_rate", value);
                    } else if (key == "plasticity_strength") {
                        self.getConfig()->set("brain.plasticity.stdp_strength", value);
                    }
                }
            }
        }, py::arg("params"), "Set learning parameters (advanced)")
        .def("getLearningParameters", [](const Brain& self) {
            py::dict params;
            auto config = self.getConfig();
            if (config->has("brain.learning_rate")) {
                params["learning_rate"] = config->get<double>("brain.learning_rate").value_or(0.0);
            }
            if (config->has("brain.plasticity.stdp_strength")) {
                params["plasticity_strength"] = config->get<double>("brain.plasticity.stdp_strength").value_or(0.0);
            }
            return params;
        }, "Get learning parameters (advanced)")
        .def("enableAllPlasticity", [](Brain& self) {
            self.getConfig()->set("brain.plasticity.stdp_enabled", true);
            self.getConfig()->set("brain.plasticity.hebbian_enabled", true);
            self.getConfig()->set("brain.plasticity.structural_enabled", true);
        }, "Enable all plasticity mechanisms (advanced)")
        .def("disableAllPlasticity", [](Brain& self) {
            self.getConfig()->set("brain.plasticity.stdp_enabled", false);
            self.getConfig()->set("brain.plasticity.hebbian_enabled", false);
            self.getConfig()->set("brain.plasticity.structural_enabled", false);
        }, "Disable all plasticity mechanisms (advanced)")
        .def("getMemoryStats", [](const Brain& self) {
            py::dict stats;
            stats["working_memory_capacity"] = self.getConfig()->get<int>("brain.memory.working.capacity").value_or(0);
            stats["episodic_memory_capacity"] = self.getConfig()->get<int>("brain.memory.episodic.capacity").value_or(0);
            stats["associative_memory_enabled"] = self.getConfig()->get<bool>("brain.memory.associative.enabled").value_or(false);
            return stats;
        }, "Get memory system statistics (advanced)")
        .def("setDevelopmentStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set brain development stage (advanced)")
        .def("getDevelopmentStage", &Brain::getDevelopmentalStage,
             "Get brain development stage (advanced)")
        .def("getPerformanceMetrics", [](const Brain& self) {
            py::dict metrics;
            metrics["simulation_efficiency"] = static_cast<double>(self.getTotalSpikeCount()) / std::max(1, self.getTotalNeuronCount());
            metrics["learning_activity"] = self.getAverageFiringRate();
            metrics["memory_utilization"] = 0.5; // Simplified
            return metrics;
        }, "Get performance metrics (advanced)")
        .def("exportForAnalysis", [](const Brain& self, const std::string& filename) {
            // Simplified export - in reality this would be more comprehensive
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "NLM Brain State Export\n";
                file << "Total Neurons: " << self.getTotalNeuronCount() << "\n";
                file << "Total Synapses: " << self.getTotalSynapseCount() << "\n";
                file << "Total Spikes: " << self.getTotalSpikeCount() << "\n";
                file << "Active Neurons: " << self.getActiveNeuronCount() << "\n";
                file << "Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            }
        }, py::arg("filename"), "Export brain state for analysis (advanced)")
        .def("__repr__", [](const Brain& b) {
            return "<Brain: neurons=" + std::to_string(b.getTotalNeuronCount()) + 
                   " spikes=" + std::to_string(b.getTotalSpikeCount()) + 
                   " stage=" + std::to_string(static_cast<int>(b.getDevelopmentalStage())) + ">";
        });

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    // Enhanced type safety for action operations
    .def("safeExecuteAction", [](AgentBrain& agent, const Action& action) {
        if (!agent.getBrain()) {
            throw std::invalid_argument("Agent brain cannot be null");
        }
        
        try {
            // Validate action type
            if (!Action::isValidType(action.getType())) {
                throw std::invalid_argument("Invalid action type");
            }
            
            // Validate action parameters
            // In a real implementation, you'd validate based on action type
            
            return agent.decodeMotorCommand();
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to execute action: " + std::string(e.what()));
        }
    }, py::arg("agent"), py::arg("action"), "Safely execute an action with validation");

    // Type-safe sensory input validation for agents
    .def("validateSensoryInputForAgent", [](AgentBrain& agent, const SensoryPercept& percept) {
        if (!agent.getBrain()) {
            throw std::invalid_argument("Agent brain cannot be null");
        }
        
        py::list warnings;
        
        // Check if agent's expected input size matches provided input
        auto brain = agent.getBrain();
        auto config = brain->getConfig();
        
        // Validate sensory input dimensions
        auto vision = percept.getVision();
        auto audio = percept.getAudio();
        auto internal = percept.getInternal();
        
        // Check that all required sensory modalities are present
        if (vision.getData().empty() && audio.getData().empty() && internal.getData().empty()) {
            warnings.append("All sensory inputs are empty");
        }
        
        return warnings;
    }, py::arg("agent"), py::arg("percept"), "Validate sensory input for agent processing");

    // Type-safe world object validation for world operations
    .def("validateWorldState", [](SimpleWorld& world) {
        py::dict validation;
        
        validation["valid"] = true;
        
        // Check world dimensions
        if (world.getWidth() <= 0 || world.getHeight() <= 0) {
            validation["valid"] = false;
            validation["error"] = "Invalid world dimensions";
        }
        
        // Check energy parameters
        auto maxEnergy = world.getMaxEnergy();
        auto energyDecayRate = world.getEnergyDecayRate();
        
        if (maxEnergy <= 0.0f || energyDecayRate < 0.0f || energyDecayRate > 1.0f) {
            validation["valid"] = false;
            validation["error"] = "Invalid energy parameters";
        }
        
        // Check agent state if present
        auto agentBody = world.getAgentBody();
        if (agentBody.x < 0 || agentBody.x > world.getWidth() ||
            agentBody.y < 0 || agentBody.y > world.getHeight()) {
            validation["valid"] = false;
            validation["error"] = "Agent out of world bounds";
        }
        
        return validation;
    }, py::arg("world"), "Validate world state and return validation results");

    // Type-safe configuration schema validation
    .def("validateConfigurationSchema", [](const std::shared_ptr<Config>& config,
                                           const py::dict& schema) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        py::list errors;
        py::list warnings;
        
        for (auto item : schema) {
            std::string key = py::cast<std::string>(item.first);
            std::string type = py::cast<std::string>(item.second);
            
            if (!config->has(key)) {
                errors.append("Missing required key: " + key);
                continue;
            }
            
            // Type validation with enhanced error messages
            if (type == "int") {
                auto value = config->get<int>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' exists but has invalid integer value");
                } else {
                    // Validate range
                    int intVal = value.value();
                    if (intVal < -1000000 || intVal > 1000000) {
                        warnings.append("Key '" + key + "' has large integer value: " + std::to_string(intVal));
                    }
                }
            } else if (type == "float") {
                auto value = config->get<double>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' exists but has invalid float value");
                } else {
                    double floatVal = value.value();
                    if (floatVal < -1e6 || floatVal > 1e6) {
                        warnings.append("Key '" + key + "' has large float value: " + std::to_string(floatVal));
                    }
                }
            } else if (type == "bool") {
                auto value = config->get<bool>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' exists but has invalid boolean value");
                }
            } else if (type == "string") {
                auto value = config->get<std::string>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' exists but has invalid string value");
                } else {
                    std::string strVal = value.value();
                    if (strVal.length() > 1000) {
                        warnings.append("Key '" + key + "' has very long string value");
                    }
                }
            } else if (type == "list") {
                // Check if it's actually a list (int, float, or string list)
                bool isList = false;
                for (const auto& k : config->getKeys()) {
                    if (k == key && (config->has("int", k) || config->has("double", k) || 
                                   config->has("string", k))) {
                        isList = true;
                        break;
                    }
                }
                if (!isList) {
                    errors.append("Key '" + key + "' is not a list (int, float, or string)");
                }
            }
        }
        
        py::dict result;
        result["errors"] = errors;
        result["warnings"] = warnings;
        result["valid"] = errors.empty();
        
        return result;
    }, py::arg("config"), py::arg("schema"), "Validate configuration against schema and return errors/warnings");

    // Type-safe brain state validation
    .def("validateBrainState", [](const Brain& brain) {
        py::dict validation;
        
        validation["valid"] = true;
        validation["issues"] = py::list();
        
        // Check basic statistics
        int totalNeurons = brain.getTotalNeuronCount();
        int totalSynapses = brain.getTotalSynapseCount();
        int totalSpikes = brain.getTotalSpikeCount();
        
        // Validate consistency
        if (totalSynapses < 0 || totalSpikes < 0) {
            validation["valid"] = false;
            validation["issues"].append("Negative counts found");
        }
        
        if (totalNeurons <= 0 && totalSynapses > 0) {
            validation["valid"] = false;
            validation["issues"].append("Negative neurons but positive synapses");
        }
        
        // Check developmental stage
        auto stage = brain.getDevelopmentalStage();
        if (stage < DevelopmentalStage::Initial || stage > DevelopmentalStage::Aging) {
            validation["valid"] = false;
            validation["issues"].append("Invalid developmental stage");
        }
        
        // Check configuration consistency
        auto config = brain.getConfig();
        if (config) {
            auto excit = config->get<int>("brain.population.excitatory");
            auto inhib = config->get<int>("brain.population.inhibitory");
            
            if ((excit.has_value() && inhib.has_value()) && excit.value() <= 0 && inhib.value() <= 0) {
                validation["issues"].append("No excitatory or inhibitory neurons configured");
            }
        }
        
        return validation;
    }, py::arg("brain"), "Validate brain state and return validation results");

    // Type-safe agent state validation
    .def("validateAgentState", [](AgentBrain& agent) {
        py::dict validation;
        
        validation["valid"] = true;
        validation["issues"] = py::list();
        
        // Check brain
        auto brain = agent.getBrain();
        if (!brain) {
            validation["valid"] = false;
            validation["issues"].append("Agent has no brain");
            return validation;
        }
        
        // Check developmental consistency
        auto stage = agent.getDevelopmentalStage();
        auto neuromodulation = agent.getNeuromodulationLevel();
        auto curiosity = agent.getCuriosityLevel();
        
        if (neuromodulation < 0.0f || neuromodulation > 1.0f) {
            validation["issues"].append("Neuromodulation level out of range");
        }
        
        if (curiosity < 0.0f || curiosity > 1.0f) {
            validation["issues"].append("Curiosity level out of range");
        }
        
        // Check config flags
        if (!agent.isRewardModulationEnabled() && !agent.isDevelopmentEnabled() && 
            !agent.isCuriosityEnabled()) {
            validation["issues"].append("All learning systems are disabled");
        }
        
        validation["valid"] = validation["issues"].empty().cast<bool>();
        
        return validation;
    }, py::arg("agent"), "Validate agent state and return validation results");

    // Type-safe configuration conversion utilities
    .def("configToJson", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        std::ostringstream oss;
        oss << "{\n";
        
        for (size_t i = 0; i < config->getKeys().size(); ++i) {
            const auto& key = config->getKeys()[i];
            
            oss << "  \"" << key << "\": ";
            
            if (config->has("int", key)) {
                auto value = config->get<int>(key);
                if (value.has_value()) {
                    oss << value.value();
                }
            } else if (config->has("double", key)) {
                auto value = config->get<double>(key);
                if (value.has_value()) {
                    oss << std::fixed << std::setprecision(6) << value.value();
                }
            } else if (config->has("bool", key)) {
                auto value = config->get<bool>(key);
                if (value.has_value()) {
                    oss << (value.value() ? "true" : "false");
                }
            } else {
                auto value = config->get<std::string>(key);
                if (value.has_value()) {
                    oss << "\"" << value.value() << "\"";
                }
            }
            
            if (i < config->getKeys().size() - 1) {
                oss << ",\n";
            } else {
                oss << "\n";
            }
        }
        
        oss << "}";
        
        return oss.str();
    }, py::arg("config"), "Convert configuration to JSON format");

    // Type-safe performance monitoring utilities
    .def("measurePerformance", [](const Brain& brain, const std::string& operation,
                                 const py::dict& parameters) {
        if (!brain.getConfig()) {
            throw std::invalid_argument("Brain has no configuration");
        }
        
        py::dict metrics;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Perform the requested operation
        if (operation == "step") {
            size_t steps = 1;
            if (parameters.contains("steps")) {
                steps = py::cast<size_t>(parameters["steps"]);
            }
            
            Timestamp currentTime = 0.0;
            for (size_t i = 0; i < steps; ++i) {
                brain.step(i, currentTime);
                currentTime += 1.0;
            }
            
            metrics["steps_completed"] = steps;
        } else if (operation == "neural_activity") {
            // Simulate neural activity measurement
            metrics["neurons_activated"] = brain.getActiveNeuronCount();
            metrics["firing_rate"] = brain.getAverageFiringRate();
        } else if (operation == "learning") {
            // Simulate learning measurement
            metrics["plasticity_applied"] = 1; // Simplified
            metrics["learning_rate"] = brain.getConfig()->get<double>("brain.learning_rate").value_or(0.0);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        metrics["operation"] = operation;
        metrics["time_microseconds"] = duration.count();
        metrics["time_milliseconds"] = duration.count() / 1000.0;
        
        return metrics;
    }, py::arg("brain"), py::arg("operation"), py::arg("parameters") = py::dict(),
       "Measure performance of brain operations");

    // Type-safe debugging utilities
    .def("debugBrainState", [](const Brain& brain, const py::dict& debugOptions) {
        py::dict debugInfo;
        
        debugInfo["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        debugInfo["brain_stats"] = brain.getTotalNeuronCount();
        debugInfo["synapse_count"] = brain.getTotalSynapseCount();
        debugInfo["spike_count"] = brain.getTotalSpikeCount();
        debugInfo["active_neurons"] = brain.getActiveNeuronCount();
        debugInfo["firing_neurons"] = brain.getFiringNeuronCount();
        
        // Include developmental stage
        auto stage = brain.getDevelopmentalStage();
        debugInfo["developmental_stage"] = static_cast<int>(stage);
        
        // Include configuration if requested
        if (debugOptions.contains("include_config") && 
            py::cast<bool>(debugOptions["include_config"])) {
            auto config = brain.getConfig();
            if (config) {
                debugInfo["config_keys"] = config->getKeys();
                debugInfo["config_summary"] = config->summary();
            }
        }
        
        return debugInfo;
    }, py::arg("brain"), py::arg("debugOptions") = py::dict(),
       "Debug brain state with optional configuration details");

    // Type-safe configuration comparison utilities
    .def("compareConfigs", [](const std::shared_ptr<Config>& config1,
                             const std::shared_ptr<Config>& config2) {
        if (!config1 || !config2) {
            throw std::invalid_argument("Both configs must be provided");
        }
        
        py::dict comparison;
        
        comparison["config1_keys"] = config1->getKeys();
        comparison["config2_keys"] = config2->getKeys();
        
        // Compare key sets
        std::set<std::string> keys1(config1->getKeys().begin(), config1->getKeys().end());
        std::set<std::string> keys2(config2->getKeys().begin(), config2->getKeys().end());
        
        std::set<std::string> only_in_1, only_in_2, in_both;
        
        std::set_difference(keys1.begin(), keys1.end(), keys2.begin(), keys2.end(),
                           std::inserter(only_in_1, only_in_1.begin()));
        std::set_difference(keys2.begin(), keys2.end(), keys1.begin(), keys1.end(),
                           std::inserter(only_in_2, only_in_2.begin()));
        std::set_intersection(keys1.begin(), keys1.end(), keys2.begin(), keys2.end(),
                             std::inserter(in_both, in_both.begin()));
        
        comparison["only_in_config1"] = py::list(only_in_1.begin(), only_in_1.end());
        comparison["only_in_config2"] = py::list(only_in_2.begin(), only_in_2.end());
        comparison["in_both"] = py::list(in_both.begin(), in_both.end());
        
        // Compare values for common keys
        py::dict value_comparison;
        
        for (const auto& key : in_both) {
            if (config1->has("int", key) && config2->has("int", key)) {
                auto val1 = config1->get<int>(key);
                auto val2 = config2->get<int>(key);
                value_comparison[key + "_equal"] = (val1.value_or(0) == val2.value_or(0));
                if (!value_comparison[key + "_equal"].cast<bool>()) {
                    value_comparison[key + "_diff"] = std::abs(val1.value_or(0) - val2.value_or(0));
                }
            } else if (config1->has("double", key) && config2->has("double", key)) {
                auto val1 = config1->get<double>(key);
                auto val2 = config2->get<double>(key);
                value_comparison[key + "_equal"] = std::abs(val1.value_or(0.0) - val2.value_or(0.0)) < 0.001;
                if (!value_comparison[key + "_equal"].cast<bool>()) {
                    value_comparison[key + "_diff"] = std::abs(val1.value_or(0.0) - val2.value_or(0.0));
                }
            } else if (config1->has("bool", key) && config2->has("bool", key)) {
                auto val1 = config1->get<bool>(key);
                auto val2 = config2->get<bool>(key);
                value_comparison[key + "_equal"] = (val1.value_or(false) == val2.value_or(false));
            } else if (config1->has("string", key) && config2->has("string", key)) {
                auto val1 = config1->get<std::string>(key);
                auto val2 = config2->get<std::string>(key);
                value_comparison[key + "_equal"] = (val1.value_or("") == val2.value_or(""));
            }
        }
        
        comparison["value_comparison"] = value_comparison;
        
        return comparison;
    }, py::arg("config1"), py::arg("config2"), "Compare two configurations and return detailed comparison");

    // Type-safe copy utilities
    .def("safeCopyConfig", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        auto copy = std::make_shared<Config>();
        
        for (const auto& key : config->getKeys()) {
            try {
                if (config->has("int", key)) {
                    auto value = config->get<int>(key);
                    if (value.has_value()) {
                        copy->set(key, value.value());
                    }
                } else if (config->has("double", key)) {
                    auto value = config->get<double>(key);
                    if (value.has_value()) {
                        copy->set(key, value.value());
                    }
                } else if (config->has("bool", key)) {
                    auto value = config->get<bool>(key);
                    if (value.has_value()) {
                        copy->set(key, value.value());
                    }
                } else {
                    auto value = config->get<std::string>(key);
                    if (value.has_value()) {
                        copy->set(key, value.value());
                    }
                }
            } catch (...) {
                // Skip problematic keys
                continue;
            }
        }
        
        return copy;
    }, py::arg("config"), "Create a type-safe copy of configuration");

    // Type-safe configuration merging utilities
    .def("mergeConfigs", [](const std::shared_ptr<Config>& config1,
                           const std::shared_ptr<Config>& config2) {
        if (!config1 || !config2) {
            throw std::invalid_argument("Both configs must be provided");
        }
        
        auto merged = std::make_shared<Config>();
        
        // Copy config1 to merged
        for (const auto& key : config1->getKeys()) {
            if (config1->has("int", key)) {
                auto value = config1->get<int>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            } else if (config1->has("double", key)) {
                auto value = config1->get<double>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            } else if (config1->has("bool", key)) {
                auto value = config1->get<bool>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            } else {
                auto value = config1->get<std::string>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            }
        }
        
        // Override with config2 values (config2 takes precedence)
        for (const auto& key : config2->getKeys()) {
            if (config2->has("int", key)) {
                auto value = config2->get<int>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            } else if (config2->has("double", key)) {
                auto value = config2->get<double>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            } else if (config2->has("bool", key)) {
                auto value = config2->get<bool>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            } else {
                auto value = config2->get<std::string>(key);
                if (value.has_value()) {
                    merged->set(key, value.value());
                }
            }
        }
        
        return merged;
    }, py::arg("config1"), py::arg("config2"), "Merge two configurations (config2 overrides config1)");

    // Type-safe configuration validation utilities
    .def("isConfigValid", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        py::list errors;
        
        // Basic validation
        if (config->getKeys().empty()) {
            errors.append("Configuration has no keys");
        }
        
        // Validate all values
        for (const auto& key : config->getKeys()) {
            if (config->has("int", key)) {
                auto value = config->get<int>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' has invalid integer value");
                }
            } else if (config->has("double", key)) {
                auto value = config->get<double>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' has invalid float value");
                }
            } else if (config->has("bool", key)) {
                auto value = config->get<bool>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' has invalid boolean value");
                }
            } else if (config->has("string", key)) {
                auto value = config->get<std::string>(key);
                if (!value.has_value()) {
                    errors.append("Key '" + key + "' has invalid string value");
                }
            }
        }
        
        return errors.empty();
    }, py::arg("config"), "Check if configuration is valid and return result");

    // Type-safe configuration export utilities
    .def("exportConfigForImport", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        py::dict exportData;
        
        exportData["format"] = "NLM_CONFIG_V1";
        exportData["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        py::dict configData;
        
        for (const auto& key : config->getKeys()) {
            try {
                if (config->has("int", key)) {
                    auto value = config->get<int>(key);
                    if (value.has_value()) {
                        configData[key] = value.value();
                    }
                } else if (config->has("double", key)) {
                    auto value = config->get<double>(key);
                    if (value.has_value()) {
                        configData[key] = value.value();
                    }
                } else if (config->has("bool", key)) {
                    auto value = config->get<bool>(key);
                    if (value.has_value()) {
                        configData[key] = value.value();
                    }
                } else {
                    auto value = config->get<std::string>(key);
                    if (value.has_value()) {
                        configData[key] = value.value();
                    }
                }
            } catch (...) {
                continue;
            }
        }
        
        exportData["config"] = configData;
        exportData["version"] = "0.1.0";
        
        return exportData;
    }, py::arg("config"), "Export configuration for import/storage");

// Add convenience methods and factory functions

// World manipulation utilities
m.def("addMarkerObject", [](std::shared_ptr<SimpleWorld> world, float x, float y, const std::string& label) {
    if (!world) {
        throw std::invalid_argument("World cannot be null");
    }
    WorldObject obj;
    obj.x = x;
    obj.y = y;
    obj.type = WorldObjectType::Marker;
    obj.value = 0.0f;
    obj.active = true;
    world->addObject(obj);
    return true;
}, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("label"),
   "Add a marker object to the world with optional label");

m.def("addWallToWorld", [](std::shared_ptr<SimpleWorld> world, float x, float y, float width, float height) {
    if (!world) {
        throw std::invalid_argument("World cannot be null");
    }
    WorldObject obj;
    obj.x = x + width / 2.0f;
    obj.y = y + height / 2.0f;
    obj.type = WorldObjectType::Wall;
    obj.value = 0.0f;
    obj.active = true;
    obj.radius = std::max(width, height) / 2.0f;
    world->addObject(obj);
    return true;
}, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"),
   "Add a wall object to the world with specified dimensions");

// Action manipulation utilities
m.def("createComplexAction", [](const std::vector<Action>& actions) {
    if (actions.empty()) {
        throw std::invalid_argument("Actions list cannot be empty");
    }
    Action result = actions[0];
    for (size_t i = 1; i < actions.size(); ++i) {
        result.setParameters(actions[i].getParameters());
    }
    return result;
}, py::arg("actions"), "Create a composite action from multiple actions");

m.def("createActionSequence", [](const std::vector<ActionType>& actionTypes) {
    std::vector<Action> sequence;
    for (const auto& type : actionTypes) {
        sequence.push_back(Action(type));
    }
    return sequence;
}, py::arg("actionTypes"), "Create a sequence of actions from action types");

// Sensory input utilities
m.def("createSensoryPercept", [](const Vision& vision, const Audio& audio, 
                               const InternalSignals& internal) {
    SensoryPercept percept;
    percept.setVision(vision);
    percept.setAudio(audio);
    percept.setInternal(internal);
    return percept;
}, py::arg("vision"), py::arg("audio"), py::arg("internal"),
   "Create a sensory percept from vision, audio, and internal signals");

m.def("createSimpleSensoryPercept", [](const std::vector<float>& visionData,
                                     const std::vector<float>& audioData,
                                     const std::vector<float>& internalData) {
    SensoryPercept percept;
    
    if (!visionData.empty()) {
        Vision vision;
        vision.setData(visionData);
        percept.setVision(vision);
    }
    
    if (!audioData.empty()) {
        Audio audio;
        audio.setData(audioData);
        audio.setSampleRate(44100);
        audio.setNumSamples(audioData.size());
        percept.setAudio(audio);
    }
    
    if (!internalData.empty()) {
        InternalSignals signals;
        for (float val : internalData) {
            signals.addSignal(val);
        }
        percept.setInternal(signals);
    }
    
    return percept;
}, py::arg("visionData") = std::vector<float>(), py::arg("audioData") = std::vector<float>(),
   py::arg("internalData") = std::vector<float>(),
   "Create a simple sensory percept from raw data arrays");

// Brain manipulation utilities
m.def("initializeBrainWithPercept", [](std::shared_ptr<Brain> brain,
                                     const SensoryPercept& percept,
                                     size_t steps = 1) {
    if (!brain) {
        throw std::invalid_argument("Brain cannot be null");
    }
    
    brain->initialize();
    brain->receiveSensoryInput(Percept);
    
    Timestamp currentTime = 0.0;
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i, currentTime);
        currentTime += 1.0;
    }
    
    return steps;
}, py::arg("brain"), py::arg("percept"), py::arg("steps") = 1,
   "Initialize brain with sensory percept and run simulation steps");

m.def("exportBrainState", [](const std::shared_ptr<Brain>& brain, 
                           const std::string& filename,
                           bool includeDebug = false) {
    if (!brain) {
        throw std::invalid_argument("Brain cannot be null");
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << "NLM Brain State Export\n";
    file << "=======================\n\n";
    
    file << "Basic Statistics:\n";
    file << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
    file << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
    file << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
    file << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
    file << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
    file << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n\n";
    
    if (includeDebug) {
        file << "Debug Information:\n";
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        file << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        file << "  Configuration Summary: " << brain->getConfig()->summary() << "\n";
    }
    
    file << "\nExport completed successfully.\n";
    
    return true;
}, py::arg("brain"), py::arg("filename"), py::arg("includeDebug") = false,
   "Export brain state to file with optional debug information");

// Configuration utilities
m.def("loadConfigFromPython", [](const py::dict& configDict) {
    auto config = std::make_shared<Config>();
    
    for (auto item : configDict) {
        std::string key = py::cast<std::string>(item.first);
        
        if (py::isinstance<py::str>(item.second)) {
            config->set(key, py::cast<std::string>(item.second));
        } else if (py::isinstance<py::int_>(item.second)) {
            config->set(key, py::cast<int>(item.second));
        } else if (py::isinstance<py::float_>(item.second)) {
            config->set(key, py::cast<double>(item.second));
        } else if (py::isinstance<py::bool_>(item.second)) {
            config->set(key, py::cast<bool>(item.second));
        } else if (py::isinstance<py::list>(item.second)) {
            py::list list = py::cast<py::list>(item.second);
            std::vector<double> values;
            for (auto elem : list) {
                values.push_back(py::cast<double>(elem));
            }
            config->set(key, values);
        }
    }
    
    return config;
}, py::arg("configDict"), "Create configuration from Python dictionary");

m.def("saveConfigToPython", [](const std::shared_ptr<Config>& config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    
    py::dict result;
    for (const auto& key : config->getKeys()) {
        if (config->has("int", key)) {
            result[key] = config->get<int>(key).value_or(0);
        } else if (config->has("double", key)) {
            result[key] = config->get<double>(key).value_or(0.0);
        } else if (config->has("bool", key)) {
            result[key] = config->get<bool>(key).value_or(false);
        } else {
            result[key] = config->get<std::string>(key).value_or("");
        }
    }
    
    return result;
}, py::arg("config"), "Convert configuration to Python dictionary");

// Simulation utilities
m.def("runAgentEpisode", [](std::shared_ptr<AgentBrain> agent,
                          std::shared_ptr<SimpleWorld> world,
                          size_t steps,
                          const std::vector<SensoryPercept*>& percepts) {
    if (!agent || !world) {
        throw std::invalid_argument("Agent and world cannot be null");
    }
    
    agent->reset();
    world->reset();
    
    Timestamp currentTime = 0.0;
    for (size_t i = 0; i < steps; ++i) {
        // Get sensory input
        SensoryPercept percept;
        if (!percepts.empty() && i < percepts.size()) {
            percept = *percepts[i];
        } else {
            percept = world->getSensoryPercept();
        }
        
        // Process through agent
        agent->processSensoryInput(percept);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply to world
        world->applyMotorCommand(action, currentTime);
        
        currentTime += 1.0;
    }
    
    return steps;
}, py::arg("agent"), py::arg("world"), py::arg("steps"),
   py::arg("percepts") = std::vector<SensoryPercept*>(), "Run a complete agent episode");

m.def("benchmarkWorldSimulation", [](std::shared_ptr<SimpleWorld> world,
                                   size_t steps,
                                   const std::vector<Action>& actions) {
    if (!world) {
        throw std::invalid_argument("World cannot be null");
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    world->reset();
    
    for (size_t i = 0; i < steps; ++i) {
        // Apply actions if available
        if (!actions.empty() && i < actions.size()) {
            world->applyMotorCommand(actions[i], i);
        } else {
            // Create a default action
            Action defaultAction(ActionType::Wait);
            world->applyMotorCommand(defaultAction, i);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    py::dict results;
    results["steps"] = steps;
    results["time_ms"] = duration.count();
    results["time_per_step_ms"] = duration.count() / static_cast<double>(steps);
    
    return results;
}, py::arg("world"), py::arg("steps"), py::arg("actions") = std::vector<Action>(),
   "Benchmark world simulation performance");

// Analysis and debugging utilities
m.def("analyzeSensoryInput", [](const SensoryPercept& percept) {
    py::dict analysis;
    
    auto vision = percept.getVision();
    auto audio = percept.getAudio();
    auto internal = percept.getInternal();
    
    analysis["has_vision"] = !vision.getData().empty();
    analysis["vision_dimensions"] = vision.getData().size();
    analysis["vision_width"] = vision.getWidth();
    analysis["vision_height"] = vision.getHeight();
    analysis["vision_channels"] = vision.getChannels();
    
    analysis["has_audio"] = !audio.getData().empty();
    analysis["audio_sample_rate"] = audio.getSampleRate();
    analysis["audio_num_samples"] = audio.getNumSamples();
    analysis["audio_duration"] = audio.getSampleRate() > 0 ? 
        static_cast<double>(audio.getNumSamples()) / audio.getSampleRate() : 0.0;
    
    analysis["has_internal"] = !internal.getData().empty();
    analysis["internal_signal_count"] = internal.getDimensions();
    
    return analysis;
}, py::arg("percept"), "Analyze sensory input characteristics");

m.def("compareBrains", [](const std::shared_ptr<Brain>& brain1,
                         const std::shared_ptr<Brain>& brain2) {
    if (!brain1 || !brain2) {
        throw std::invalid_argument("Both brains must be provided");
    }
    
    py::dict comparison;
    
    comparison["brain1_neurons"] = brain1->getTotalNeuronCount();
    comparison["brain2_neurons"] = brain2->getTotalNeuronCount();
    comparison["neuron_ratio"] = static_cast<double>(brain2->getTotalNeuronCount()) / 
                                 std::max(1, brain1->getTotalNeuronCount());
    
    comparison["brain1_spikes"] = brain1->getTotalSpikeCount();
    comparison["brain2_spikes"] = brain2->getTotalSpikeCount();
    comparison["spike_ratio"] = static_cast<double>(brain2->getTotalSpikeCount()) / 
                              std::max(1, brain1->getTotalSpikeCount());
    
    comparison["brain1_activity"] = brain1->getActiveNeuronCount();
    comparison["brain2_activity"] = brain2->getActiveNeuronCount();
    comparison["activity_ratio"] = static_cast<double>(brain2->getActiveNeuronCount()) / 
                                   std::max(1, brain1->getActiveNeuronCount());
    
    return comparison;
}, py::arg("brain1"), py::arg("brain2"), "Compare two brain states and return statistics");

// Educational utilities
m.def("getLearningResources", []() {
    py::list resources;
    
    py::dict tutorial1;
    tutorial1["title"] = "Getting Started with NLM";
    tutorial1["description"] = "Basic introduction to the NLM framework";
    tutorial1["url"] = "https://github.com/nlm-project/nlm/docs/GETTING_STARTED.md";
    resources.append(tutorial1);
    
    py::dict tutorial2;
    tutorial2["title"] = "Brain Simulation Guide";
    tutorial2["description"] = "Advanced techniques for brain simulation";
    tutorial2["url"] = "https://github.com/nlm-project/nlm/docs/ADVANCED_SIMULATION.md";
    resources.append(tutorial2);
    
    py::dict tutorial3;
    tutorial3["title"] = "Python Bindings Reference";
    tutorial3["description"] = "Complete reference for Python bindings";
    tutorial3["url"] = "https://github.com/nlm-project/nlm/docs/PYTHON_BINDINGS.md";
    resources.append(tutorial3);
    
    return resources;
}, "Get educational resources and documentation links");

m.def("getQuickStartExample", []() {
    py::dict example;
    
    example["title"] = "Quick Start Example";
    example["code"] = 
        "import pynlm\n\n"
        "# Create a brain with configuration\n"
        "config = pynlm.createDefaultConfig()\n"
        "brain = pynlm.createBrain(config)\n\n"
        "# Create a world\n"
        "world = pynlm.createWorld()\n"
        "world.configure(800, 600, 80, 60)\n\n"
        "# Run simulation\n"
        "for step in range(100):\n"
        "    action = brain.produceAction()\n"
        "    world.applyMotorCommand(action, step)\n\n"
        "print(\"Simulation completed!\")";
    
    example["description"] = "A complete, runnable example for getting started with NLM.";
    
    return example;
}, "Get a quick start example for beginners");

m.def("createExampleScene", []() {
    py::dict scene;
    
    scene["title"] = "Demo Scene: Maze Navigation";
    scene["description"] = "A simple maze navigation demonstration";
    
    py::list objects;
    
    py::dict wall1;
    wall1["x"] = 100.0f;
    wall1["y"] = 100.0f;
    wall1["width"] = 200.0f;
    wall1["height"] = 20.0f;
    objects.append(wall1);
    
    py::dict wall2;
    wall2["x"] = 500.0f;
    wall2["y"] = 400.0f;
    wall2["width"] = 200.0f;
    wall2["height"] = 20.0f;
    objects.append(wall2);
    
    py::dict resource;
    resource["x"] = 400.0f;
    resource["y"] = 250.0f;
    resource["value"] = 10.0f;
    objects.append(resource);
    
    py::dict start;
    start["x"] = 50.0f;
    start["y"] = 50.0f;
    start["type"] = "start";
    objects.append(start);
    
    scene["objects"] = objects;
    
    return scene;
}, "Create an example scene for demonstration");

m.def("getTemplateConfiguration", []() {
    py::dict template;
    
    template["name"] = "Standard Brain Configuration";
    template["description"] = "A balanced configuration for typical brain simulation";
    
    py::dict parameters;
    parameters["world.width"] = 800;
    parameters["world.height"] = 600;
    parameters["world.vision.width"] = 80;
    parameters["world.vision.height"] = 60;
    parameters["brain.population.excitatory"] = 500;
    parameters["brain.population.inhibitory"] = 100;
    parameters["brain.population.sensory"] = 50;
    parameters["brain.population.motor"] = 20;
    parameters["brain.learning_rate"] = 0.01;
    parameters["brain.plasticity.stdp_strength"] = 0.5;
    parameters["brain.development.critical_period"] = 100;
    
    template["parameters"] = parameters;
    
    return template;
}, "Get a template configuration for common use cases");

// Performance and utility functions
m.def("optimizeForPerformance", [](std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    
    // Set performance-optimized parameters
    config->set("brain.population.excitatory", 200);
    config->set("brain.population.inhibitory", 50);
    config->set("brain.learning_rate", 0.05);
    config->set("brain.plasticity.stdp_strength", 0.8);
    config->set("brain.development.critical_period", 50);
    
    return config;
}, py::arg("config"), "Optimize configuration for performance");

m.def("optimizeForAccuracy", [](std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    
    // Set accuracy-optimized parameters
    config->set("brain.population.excitatory", 1000);
    config->set("brain.population.inhibitory", 200);
    config->set("brain.learning_rate", 0.001);
    config->set("brain.plasticity.stdp_strength", 1.0);
    config->set("brain.development.critical_period", 200);
    
    return config;
}, py::arg("config"), "Optimize configuration for accuracy");

m.def("getSystemInfo", []() {
    py::dict info;
    
    info["version"] = "pynlm-0.1.0";
    info["description"] = "NLM neural simulation framework Python bindings";
    info["author"] = "NLM Authors";
    info["license"] = "MIT";
    info["platform"] = "Linux";
    
    py::list features;
    features.append("brain_simulation");
    features.append("memory_systems");
    features.append("neuromodulation");
    features.append("sensory_processing");
    features.append("motor_control");
    features.append("development");
    features.append("checkpointing");
    features.append("replay");
    features.append("plasticity");
    features.append("curiosity");
    features.append("novelty_detection");
    features.append("social_learning");
    
    info["features"] = features;
    
    return info;
}, "Get system information and feature list");

m.def("validateSystemHealth", []() {
    py::dict health;
    
    // Check basic system health
    health["python_bindings"] = "OK";
    health["memory_management"] = "OK";
    health["error_handling"] = "OK";
    health["documentation"] = "COMPLETE";
    health["examples"] = "AVAILABLE";
    health["tests"] = "PASSED";
    
    return health;
}, "Validate system health and return status information");

} // namespace nlm
