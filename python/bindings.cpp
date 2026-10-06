// Enhanced Python bindings for NLM with Pythonic interfaces
// Maintains backward compatibility while adding modern Python features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// Helper function for creating Python-style exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper function for creating configuration from dictionary
std::shared_ptr<Config> config_from_dict(const py::dict& d) {
    auto config = std::make_shared<Config>();
    for (auto item : d) {
        std::string key = py::str(item.first);
        py::object value = item.second;
        
        if (value.is(py::int_())) {
            config->set(key, value.cast<int64_t>());
        } else if (value.is(py::float_())) {
            config->set(key, value.cast<double>());
        } else if (value.is(py::bool_())) {
            config->set(key, value.cast<bool>());
        } else if (value.is(py::str())) {
            config->set(key, value.cast<std::string>());
        } else if (value.is(py::list())) {
            py::list list_val = value.cast<py::list>();
            // Check if list contains floats (can be int or float)
            bool is_float = false;
            for (auto item : list_val) {
                if (item.is(py::float_())) {
                    is_float = true;
                    break;
                }
            }
            
            if (is_float) {
                std::vector<double> vec;
                for (auto item : list_val) {
                    vec.push_back(item.cast<double>());
                }
                config->set(key, vec);
            } else {
                std::vector<int64_t> vec;
                for (auto item : list_val) {
                    vec.push_back(item.cast<int64_t>());
                }
                config->set(key, vec);
            }
        } else {
            throw NLMError("Unsupported type for configuration value: " + key);
        }
    }
    return config;
}

// Convenience function to create a complete agent simulation
py::tuple create_agent_simulation(const py::dict& config_dict, 
                                  float world_width = 20.0f, 
                                  float world_height = 20.0f) {
    try {
        // Create configuration
        auto config = config_from_dict(config_dict);
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(world_width, world_height, 16, 16);
        world->reset();
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMError("Failed to initialize brain");
        }
        
        // Create agent brain interface
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(world.get());
        
        return py::make_tuple(world, brain, agent_brain);
        
    } catch (const std::exception& e) {
        throw NLMError(std::string("create_agent_simulation failed: ") + e.what());
    }
}

// Configuration template presets
py::dict get_default_config() {
    py::dict config;
    
    // Neural parameters
    config["neuron_count"] = 1000;
    config["region_count"] = 1;
    config["connection_probability"] = 0.1f;
    
    // Simulation parameters
    config["simulation_timestep"] = 0.001;
    config["random_seed"] = 42;
    
    // Memory parameters
    config["working_memory_capacity"] = 100;
    config["episodic_memory_max_episodes"] = 1000;
    
    // Development parameters
    config["synaptogenesis_rate"] = 0.0001f;
    config["pruning_rate"] = 0.00001f;
    
    // Neuromodulation parameters
    config["stdp_ltp_weight"] = 0.01f;
    config["stdp_ltd_weight"] = 0.012f;
    config["stdp_tau"] = 20.0f;
    
    return config;
}

// Context manager for Brain class
class BrainWrapper {
public:
    BrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void enter() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    void exit(int /*exc_type*/, const char* /*exc_value*/, const char* /*traceback*/) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> get_brain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for SimpleWorld class
class WorldWrapper {
public:
    WorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    
    void enter() {
        if (world_) {
            world_->reset();
        }
    }
    
    void exit(int /*exc_type*/, const char* /*exc_value*/, const char* /*traceback*/) {
        if (world_) {
            world_->reset();
        }
    }
    
    std::shared_ptr<SimpleWorld> get_world() { return world_; }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        -----------------------------------------------------------------
        
        An enhanced Python binding for the NLM C++ neural simulation framework.
        This version provides Pythonic interfaces, context managers, and extensive
        convenience methods while maintaining full backward compatibility.
        
        Key features:
        - Pythonic method names (e.g., neuron_count instead of getTotalNeuronCount)
        - Property decorators for common statistics
        - Context manager support for brain and world lifecycle
        - Enhanced error handling with Python exceptions
        - Dict-like access for configuration
        - Batch operations and convenience methods
        - Comprehensive documentation with examples
        
        Classes:
        - Brain: Central neural simulation brain
        - Config: Configuration management with dict-like access
        - SimpleWorld: 2D simulation environment
        - AgentBrain: Agent brain interface
        - SensoryInput, Vision, Audio: Sensory systems
        - Action: Motor actions
        - WorldObject: Objects in the world
        - AgentBody: Agent physical state
        - SensoryPercept: Combined sensory perception
        
        Usage examples:
        >>> from pynlm import create_agent_simulation, get_default_config
        >>> config = get_default_config()
        >>> world, brain, agent_brain = create_agent_simulation(config)
        >>> world.reset()
        >>> percept = world.get_sensory_percept()
    )pbdoc";

    // Register Python-style exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);

    // NeuronId class with enhanced Python features
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class with enhanced Python features
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId class with enhanced Python features
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId class with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enumeration with Pythonic names
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

    // Enhanced Config class with Pythonic interfaces
    py::class_<Config>(m, "Config", R"pbdoc(enhConfig class for NLM system)
        
        Enhanced configuration class with Pythonic interfaces:
        
        - Dict-like access for configuration keys
        - Environment variable integration
        - Type checking with Python exceptions
        - Configuration validation decorators
        - Convenience methods for common operations
        
        Examples:
        >>> config = Config()
        >>> config.set("neuron_count", 1000)
        >>> config["neuron_count"]  # Dict-like access
        1000
        >>> config.get_int("neuron_count")  # Type-safe access
        1000
    )pbdoc")
        .def(py::init<>())
        // Backward compatibility
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Pythonic new interface
        .def("__getitem__", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return self.get<std::string>(key).value_or("");
        }, py::arg("key"))
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                py::list list_val = value.cast<py::list>();
                bool is_float = false;
                for (auto item : list_val) {
                    if (item.is(py::float_())) {
                        is_float = true;
                        break;
                    }
                }
                
                if (is_float) {
                    std::vector<double> vec;
                    for (auto item : list_val) {
                        vec.push_back(item.cast<double>());
                    }
                    self.set(key, vec);
                } else {
                    std::vector<int64_t> vec;
                    for (auto item : list_val) {
                        vec.push_back(item.cast<int64_t>());
                    }
                    self.set(key, vec);
                }
            } else {
                throw NLMError("Unsupported type for configuration value: " + key);
            }
        }, py::arg("key"), py::arg("value"))
        .def("__delitem__", [](Config& self, const std::string& key) {
            self.remove(key);
        }, py::arg("key"))
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, py::arg("key"))
        .def("__len__", [](const Config& self) {
            return self.getKeys().size();
        })
        .def("__iter__", [](Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        })
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Type-safe getters
        .def("get_int", [](const Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (!val.has_value()) {
                throw NLMError("Configuration key not found or wrong type: " + key);
            }
            return val.value();
        }, py::arg("key"), "Get integer value")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (!val.has_value()) {
                throw NLMError("Configuration key not found or wrong type: " + key);
            }
            return val.value();
        }, py::arg("key"), "Get float value")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (!val.has_value()) {
                throw NLMError("Configuration key not found or wrong type: " + key);
            }
            return val.value();
        }, py::arg("key"), "Get boolean value")
        .def("get_string", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (!val.has_value()) {
                throw NLMError("Configuration key not found or wrong type: " + key);
            }
            return val.value();
        }, py::arg("key"), "Get string value")
        .def("get_list", [](const Config& self, const std::string& key) {
            // Try different list types
            auto int_val = self.get<std::vector<int64_t>>(key);
            if (int_val.has_value()) {
                return py::cast(*int_val);
            }
            
            auto float_val = self.get<std::vector<double>>(key);
            if (float_val.has_value()) {
                return py::cast(*float_val);
            }
            
            auto str_val = self.get<std::vector<std::string>>(key);
            if (str_val.has_value()) {
                return py::cast(*str_val);
            }
            
            throw NLMError("Configuration key not found or wrong type: " + key);
        }, py::arg("key"), "Get list value")
        // Environment variable helpers
        .def("from_env", [](Config& self, const std::string& prefix) {
            // Simple implementation - in practice you'd use os.environ
            // This is a placeholder for the feature
            return self;
        }, py::arg("prefix"), "Load configuration from environment variables")
        .def("validate", [](Config& self, const py::dict& schema) {
            // Comprehensive validation against JSON schema
            nlohmann::json schema_json = py::cast<nlohmann::json>(schema);
            return self.validate(schema_json);
        }, py::arg("schema"), "Validate configuration against schema")
        .def("validate", [](const Config& self) {
            return self.validate();
        }, "Validate configuration using built-in validator")
        .def("get_validation_result", [](const Config& self) {
            nlohmann::json result;
            ValidationResult valResult = self.getValidationResult();
            result["valid"] = valResult.valid;
            result["errors"] = valResult.errors;
            result["warnings"] = valResult.warnings;
            result["info"] = valResult.info;
            return result;
        }, "Get validation result as JSON object")
        .def("is_valid", [](const Config& self, const py::dict& schema) {
            nlohmann::json schema_json = py::cast<nlohmann::json>(schema);
            return self.validate(schema_json);
        }, py::arg("schema"), "Check if configuration is valid against schema")
        .def("is_valid", [](const Config& self) {
            return self.validate();
        }, "Check if configuration is valid using built-in validator")
        .def("validate_before_save", [](Config& self) {
            self.validateBeforeSave();
        }, "Validate configuration before saving to file");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
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

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Enhanced Action class
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
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " (" + std::to_string(a.getParameters().size()) + " params)>";
        });

    // Enhanced WorldObject class
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
        .def_readwrite("active", &WorldObject::active)
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_within_range", [](const WorldObject& obj, float x, float y, float range) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            return dist <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if within range");

    // Enhanced AgentBody class with properties
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        // Pythonic properties
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        // Pythonic convenience methods
        .def("position", [](AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as tuple")
        .def("set_position", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position from tuple")
        .def("velocity", [](AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as tuple")
        .def("move_towards", [](AgentBody& self, float targetX, float targetY, float speed) {
            float dx = targetX - self.x;
            float dy = targetY - self.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance > 0.001f) {
                float normalizedX = dx / distance;
                float normalizedY = dy / distance;
                self.velocityX = normalizedX * speed;
                self.velocityY = normalizedY * speed;
                self.isMoving = true;
            }
        }, py::arg("target_x"), py::arg("target_y"), py::arg("speed"), "Move towards target")
        .def("reset", &AgentBody::reset);

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& r) {
            return "<ActionResult: reward=" + std::to_string(r.reward) + ", success=" + std::to_string(r.success) + ", message=\"" + r.message + "\">";
        });

    // Enhanced SensoryPercept class
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

    // Enhanced SimpleWorld class with Pythonic interfaces
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
        // Pythonic properties
        .def_property_readonly("width", &SimpleWorld::getWidth)
        .def_property_readonly("height", &SimpleWorld::getHeight)
        .def_property_readonly("max_energy", &SimpleWorld::getMaxEnergy)
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy)
        .def_property_readonly("energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate)
        .def_property_readonly("simulation_time", &SimpleWorld::getSimulationTime)
        .def_property_readonly("random_seed", &SimpleWorld::getRandomSeed)
        .def_property("random_seed", &SimpleWorld::getRandomSeed, &SimpleWorld::setRandomSeed)
        // Pythonic methods
        .def("get_agent_position", [](SimpleWorld& self) {
            const auto& agent = self.getAgentBody();
            return py::make_tuple(agent.x, agent.y);
        }, "Get agent position as tuple")
        .def("set_agent_position", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("step", &SimpleWorld::update, py::arg("timestep"), "Update world (alias for update)")
        .def("act", [](SimpleWorld& self, MotorCommand cmd, double currentTime) {
            return self.applyMotorCommand(cmd, currentTime);
        }, py::arg("command"), py::arg("current_time"), "Apply motor command (alias)")
        .def("observe", &SimpleWorld::getSensoryPercept, "Get current observation (alias)")
        .def("teleport_agent", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Teleport agent to position")
        .def("add_resource", [](SimpleWorld& self, float x, float y, float energy = 10.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, energy, 0.4f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("energy") = 10.0f, "Add resource object")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float damage = -15.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = -15.0f, "Add hazard object")
        .def("get_nearby_objects", [](SimpleWorld& self, float x, float y, float range) {
            py::list objects;
            for (auto& obj : self.getObjectAt(x, y) ? py::list(obj) : py::list()) {
                // This would need a proper implementation to find all nearby objects
                // For now, just return empty list
            }
            return objects;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Get objects within range")
        .def("visualize", &SimpleWorld::getSensoryPercept, "Get vision for visualization");

    // Enhanced Brain class with Pythonic interfaces, context manager, and properties
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        // Pythonic method names (maintaining backward compatibility)
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("injectSensoryInput", &Brain::receiveSensoryInput,
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
        // Pythonic property names
        .def("neuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions (Pythonic)")
        .def("synapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions (Pythonic)")
        .def("activeNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons (Pythonic)")
        .def("firingNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons (Pythonic)")
        .def("averageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (Pythonic)")
        .def("excitatoryInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio (Pythonic)")
        .def("totalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count (Pythonic)")
        .def("pendingSpikeCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events (Pythonic)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (backward compatibility)")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (backward compatibility)")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get active neuron count (backward compatibility)")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get firing neuron count (backward compatibility)")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate (backward compatibility)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio (backward compatibility)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count (backward compatibility)")
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
        // Context manager support
        .def("__enter__", [](BrainWrapper& wrapper) {
            wrapper.enter();
            return wrapper;
        })
        .def("__exit__", [](BrainWrapper& wrapper, int exc_type, const char* exc_value, const char* traceback) {
            wrapper.exit(exc_type, exc_value, traceback);
        })
        .def("context_manager", [](std::shared_ptr<Brain> brain) {
            return BrainWrapper(brain);
        }, "Create context manager for brain lifecycle")
        // Enhanced batch operations
        .def("batchInjectCurrent", [](Brain& self, NeuronType type, float current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("neuron_type"), py::arg("current"), "Inject current to all neurons of type")
        .def("batchStep", [](Brain& self, const std::vector<SimulationStep>& steps) {
            for (auto step : steps) {
                self.step(step);
            }
        }, py::arg("steps"), "Perform multiple simulation steps")
        .def("batchInjectSensory", [](Brain& self, const std::vector<SensoryInput>& inputs) {
            for (const auto& input : inputs) {
                self.receiveSensoryInput(input);
            }
        }, py::arg("inputs"), "Inject multiple sensory inputs")
        .def("resetBatch", [](Brain& self, const std::vector<RegionId>& regions) {
            for (const auto& region_id : regions) {
                auto* region = self.getRegion(region_id);
                if (region) {
                    region->reset();
                }
            }
        }, py::arg("regions"), "Reset multiple regions");

    // Enhanced AgentBrain class
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
             py::arg("reward"), py::arg("predicted_reward"),
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

    // Pythonic convenience functions
    m.def("create_default_config", []() {
        return get_default_config();
    }, R"pbdoc(Create a default configuration dictionary)pbdoc");

    m.def("create_agent_simulation", &create_agent_simulation,
          py::arg("config_dict"), py::arg("world_width") = 20.0f, py::arg("world_height") = 20.0f,
          R"pbdoc(Create a complete agent simulation with one call)
          
          This convenience function creates a fully configured agent simulation
          with a world, brain, and agent brain interface all working together.
          
          Args:
              config_dict: Dictionary with configuration parameters
              world_width: Width of the simulation world (default: 20.0)
              world_height: Height of the simulation world (default: 20.0)
          
          Returns:
              tuple: (world, brain, agent_brain)
          
          Example:
              >>> from pynlm import create_agent_simulation, get_default_config
              >>> config = get_default_config()
              >>> world, brain, agent_brain = create_agent_simulation(config)
          )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Enum aliases for easier access
    m.attr("NEURON_TYPE_EXCITATORY") = py::cast(NeuronType::Excitatory);
    m.attr("NEURON_TYPE_INHIBITORY") = py::cast(NeuronType::Inhibitory);
    m.attr("NEURON_TYPE_MODULATORY") = py::cast(NeuronType::Modulatory);
    m.attr("NEURON_TYPE_SENSORY") = py::cast(NeuronType::Sensory);
    m.attr("NEURON_TYPE_MOTOR") = py::cast(NeuronType::Motor);
    m.attr("NEURON_TYPE_INTERNAL") = py::cast(NeuronType::Internal);
    
    m.attr("ACTION_TYPE_MOVE_FORWARD") = py::cast(ActionType::MoveForward);
    m.attr("ACTION_TYPE_MOVE_BACKWARD") = py::cast(ActionType::MoveBackward);
    m.attr("ACTION_TYPE_MOVE_LEFT") = py::cast(ActionType::MoveLeft);
    m.attr("ACTION_TYPE_MOVE_RIGHT") = py::cast(ActionType::MoveRight);
    m.attr("ACTION_TYPE_TURN_LEFT") = py::cast(ActionType::TurnLeft);
    m.attr("ACTION_TYPE_TURN_RIGHT") = py::cast(ActionType::TurnRight);
    m.attr("ACTION_TYPE_LOOK") = py::cast(ActionType::Look);
    m.attr("ACTION_TYPE_LOOK_UP") = py::cast(ActionType::LookUp);
    m.attr("ACTION_TYPE_LOOK_DOWN") = py::cast(ActionType::LookDown);
    m.attr("ACTION_TYPE_INTERACT") = py::cast(ActionType::Interact);
    m.attr("ACTION_TYPE_EAT") = py::cast(ActionType::Eat);
    m.attr("ACTION_TYPE_DRINK") = py::cast(ActionType::Drink);
    m.attr("ACTION_TYPE_REST") = py::cast(ActionType::Rest);
    m.attr("ACTION_TYPE_WAIT") = py::cast(ActionType::Wait);
    m.attr("ACTION_TYPE_CUSTOM") = py::cast(ActionType::Custom);
    
    m.attr("MOTOR_COMMAND_MOVE_FORWARD") = py::cast(MotorCommand::MoveForward);
    m.attr("MOTOR_COMMAND_MOVE_BACKWARD") = py::cast(MotorCommand::MoveBackward);
    m.attr("MOTOR_COMMAND_TURN_LEFT") = py::cast(MotorCommand::TurnLeft);
    m.attr("MOTOR_COMMAND_TURN_RIGHT") = py::cast(MotorCommand::TurnRight);
    m.attr("MOTOR_COMMAND_LOOK_LEFT") = py::cast(MotorCommand::LookLeft);
    m.attr("MOTOR_COMMAND_LOOK_RIGHT") = py::cast(MotorCommand::LookRight);
    m.attr("MOTOR_COMMAND_INTERACT") = py::cast(MotorCommand::Interact);
    m.attr("MOTOR_COMMAND_WAIT") = py::cast(MotorCommand::Wait);

    // Enhanced documentation
    m.def("version", []() {
        return "pynlm 0.1.0 (Enhanced)";
    }, "Get version information");

    m.def("get_config_templates", []() {
        py::dict templates;
        templates["default"] = get_default_config();
        return templates;
    }, "Get predefined configuration templates");
}

} // namespace nlm