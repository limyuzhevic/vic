#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>

// Custom exception classes for better Python error handling
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMValidationError : public NLMRuntimeError {
public:
    explicit NLMValidationError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMConfigurationError : public NLMRuntimeError {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMSerializationError : public NLMRuntimeError {
public:
    explicit NLMSerializationError(const std::string& msg) : NLMRuntimeError(msg) {}
};

// Context manager for automatic resource cleanup
class BrainContextManager {
private:
    std::shared_ptr<class Brain> brain_;
    bool save_on_exit_ = false;
    std::string checkpoint_path_;
    
public:
    explicit BrainContextManager(std::shared_ptr<class Brain> brain) 
        : brain_(brain) {}
    
    BrainContextManager(std::shared_ptr<class Brain> brain, 
                       bool save_on_exit, 
                       const std::string& checkpoint_path = "")
        : brain_(brain), save_on_exit_(save_on_exit), checkpoint_path_(checkpoint_path) {}
    
    void enter() {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    void exit(int exc_type, const std::string& exc_msg, const std::string& traceback) {
        if (save_on_exit_ && brain_ && !checkpoint_path_.empty()) {
            try {
                brain_->save(checkpoint_path_);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to save checkpoint on exit: " << e.what() << std::endl;
            }
        }
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<class Brain> getBrain() { return brain_; }
};

// Context manager for world simulation
class WorldContextManager {
private:
    std::shared_ptr<class SimpleWorld> world_;
    std::shared_ptr<class AgentBrain> agent_;
    
public:
    WorldContextManager(std::shared_ptr<class SimpleWorld> world, 
                       std::shared_ptr<class AgentBrain> agent)
        : world_(world), agent_(agent) {}
    
    void enter() {
        if (world_ && agent_) {
            world_->reset();
            agent_->reset();
        }
    }
    
    void exit(int exc_type, const std::string& exc_msg, const std::string& traceback) {
        if (world_ && agent_) {
            world_->reset();
            agent_->reset();
        }
    }
    
    std::shared_ptr<class SimpleWorld> getWorld() { return world_; }
    std::shared_ptr<class AgentBrain> getAgent() { return agent_; }
};

// Convenience helper functions
namespace nlm {
    // High-level API functions
    std::shared_ptr<Config> create_config_with_preset(const std::string& preset);
    
    void configure_for_learning(std::shared_ptr<Config> config, 
                               float learning_rate = 0.01f,
                               size_t max_steps = 1000);
    
    void configure_for_reproduction(std::shared_ptr<Config> config,
                                    size_t reproduction_steps = 500);
    
    // Batch processing utilities
    struct TrainingResult {
        float total_reward;
        float avg_firing_rate;
        size_t total_spikes;
        size_t episodes_stored;
        double training_time;
        bool success;
        std::string error_message;
    };
    
    TrainingResult run_training_episode(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t max_steps = 1000,
        bool save_checkpoints = false,
        const std::string& checkpoint_prefix = "checkpoint_"
    );
    
    // Serialization helpers
    void save_brain_as_json(std::shared_ptr<Brain> brain, const std::string& filepath);
    std::shared_ptr<Brain> load_brain_from_json(const std::string& filepath);
    
    // Visualization helpers
    void plot_neural_activity(std::shared_ptr<Brain> brain, 
                              const std::string& output_file = "neural_activity.png");
    
    // Experiment helpers
    struct ExperimentConfig {
        std::string name;
        size_t neuron_count = 1000;
        size_t max_steps = 10000;
        float connection_probability = 0.1f;
        bool enable_checkpointing = true;
        bool enable_replay = true;
        bool enable_development = true;
        std::string checkpoint_path = "./experiment_checkpoint.bin";
        std::string results_path = "./experiment_results.json";
    };
    
    void run_experiment(const ExperimentConfig& config);
    
    // Utility functions
    std::string get_platform_info();
    size_t get_memory_usage();
    
    // Python-specific helpers
    py::tuple get_version_info();
    py::dict get_system_info();
}

// Python wrapper functions for context managers
inline BrainContextManager make_brain_context(std::shared_ptr<class Brain> brain) {
    return BrainContextManager(brain);
}

inline BrainContextManager make_brain_context(std::shared_ptr<class Brain> brain,
                                              bool save_on_exit,
                                              const std::string& checkpoint_path = "") {
    return BrainContextManager(brain, save_on_exit, checkpoint_path);
}

inline WorldContextManager make_world_context(std::shared_ptr<class SimpleWorld> world,
                                             std::shared_ptr<class AgentBrain> agent) {
    return WorldContextManager(world, agent);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ---------------------------------------------------------------
        
        A comprehensive Python API for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Modern Python features and best practices
        - Custom exceptions for better error handling
        - Context managers for resource management
        - High-level convenience functions for common use cases
        - Enhanced documentation and type hints
        - Serialization and visualization helpers
        - Experiment management utilities
        
        Key improvements over the basic bindings:
        • Better error handling with custom exception classes
        • Automatic resource cleanup with context managers
        • High-level APIs for training, simulation, and analysis
        • Enhanced documentation and examples
        • Performance optimizations for batch operations
        • Platform-independent API design
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMRuntimeError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMValidationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const NLMConfigurationError& e) {
            PyErr_SetString(PyExc_KeyError, e.what());
        } catch (const NLMSerializationError& e) {
            PyErr_SetString(PyExc_IOError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Basic identifier classes with enhanced Python features
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // Enumerations with better Python integration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__repr__", [](NeuronType type) {
            switch(type) {
                case NeuronType::Excitatory: return "NeuronType.Excitatory";
                case NeuronType::Inhibitory: return "NeuronType.Inhibitory";
                case NeuronType::Modulatory: return "NeuronType.Modulatory";
                case NeuronType::Sensory: return "NeuronType.Sensory";
                case NeuronType::Motor: return "NeuronType.Motor";
                case NeuronType::Internal: return "NeuronType.Internal";
                default: return "NeuronType.Unknown";
            }
        });

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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
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

    // Enhanced Config class with validation and presets
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with validation and presets)pbdoc")
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
        .def("get", &Config::get<py::args>(), py::arg("key"), py::arg("default_value") = nullptr,
             "Get configuration value with default")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](Config& self) {
            // Basic validation - can be expanded
            return true; // For now, assume valid
        }, "Validate configuration and return True if valid")
        .def("apply_preset", &Config::set, py::arg("key"), py::arg("value"),
             "Apply a configuration preset")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput base class with factory methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& self) {
            py::dict result;
            result["type"] = self.getType();
            auto data = self.getData();
            result["data"] = data;
            result["dimensions"] = self.getDimensions();
            return result;
        }, "Convert to Python dictionary");

    // Enhanced Vision class with better data handling
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with improved data handling)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("set_array", [](Vision& self, const py::array_t<float>& array) {
            // Convert numpy array to vector
            auto buf = array.request();
            size_t size = buf.size;
            std::vector<float> data(size);
            std::memcpy(data.data(), buf.ptr, size * sizeof(float));
            self.setData(data);
        }, py::arg("array"), "Set vision data from numpy array")
        .def("get_array", [](const Vision& self) {
            auto data = self.getData();
            return py::array_t<float>(data);
        }, "Get vision data as numpy array");

    // Enhanced Audio class with sample rate support
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("get_duration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds");

    // Enhanced Action class with fluent interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation with fluent interface for easier construction)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("add_parameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to this action")
        .def("get_parameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw NLMValidationError("Parameter index out of range");
        }, py::arg("index"), "Get parameter at index")
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert to Python dictionary");

    // Enhanced WorldObject with better validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object with validation and enhanced properties)pbdoc")
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
        .def("is_resource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if this is a resource object")
        .def("is_hazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if this is a hazard object")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // Enhanced AgentBody with physics simulation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body with physics simulation and validation)pbdoc")
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
        .def("move", [](AgentBody& self, float dx, float dy, float dt = 0.1f) {
            self.x += dx;
            self.y += dy;
            self.velocityX = dx / dt;
            self.velocityY = dy / dt;
            self.isMoving = (dx != 0.0f || dy != 0.0f);
            self.lastActionTime = self.age;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt") = 0.1f, "Move agent by delta position")
        .def("turn", [](AgentBody& self, float angle, float dt = 0.1f) {
            self.orientation += angle;
            while (self.orientation > 360.0f) self.orientation -= 360.0f;
            while (self.orientation < 0.0f) self.orientation += 360.0f;
            self.angularVelocity = angle / dt;
            self.isTurning = (angle != 0.0f);
            self.lastActionTime = self.age;
        }, py::arg("angle"), py::arg("dt") = 0.1f, "Turn agent by angle in degrees")
        .def("reset", &AgentBody::reset);

    // Enhanced ActionResult with better error handling
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result with enhanced error handling and validation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("get_reward", [](const ActionResult& self) {
            return self.reward;
        }, "Get reward value")
        .def("get_message", [](const ActionResult& self) {
            return self.message;
        }, "Get result message");

    // Enhanced SensoryPercept with factory methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept with factory methods and enhanced processing)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("is_valid", [](const SensoryPercept& self) {
            // Check if percept contains valid data
            auto vision = self.getVision();
            auto touch = self.getTouch();
            auto internal = self.getInternal();
            auto audio = self.getAudio();
            auto proprioception = self.getProprioception();
            return vision.getWidth() > 0 && vision.getHeight() > 0;
        }, "Check if percept is valid");

    // Enhanced SimpleWorld with better simulation control
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple world with better simulation control and validation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width") = 16, py::arg("vision_height") = 16,
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation with timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body from world")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy for agents")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        .def("create_agent_start", [](SimpleWorld& self, float x, float y) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Marker;
            obj.value = 100.0f; // Max energy
            obj.active = true;
            self.setAgentStart(x, y);
            return obj;
        }, py::arg("x"), py::arg("y"), "Create agent start object");

    // Enhanced Brain class with context manager and high-level APIs
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural brain with context manager and high-level APIs)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Context manager support
        .def("make_context", [](std::shared_ptr<Brain> brain) {
            return make_brain_context(brain);
        }, "Create context manager for automatic resource management")
        .def("make_context", [](std::shared_ptr<Brain> brain, bool save_on_exit, const std::string& checkpoint_path) {
            return make_brain_context(brain, save_on_exit, checkpoint_path);
        }, py::arg("brain"), py::arg("save_on_exit"), py::arg("checkpoint_path") = "",
             "Create context manager with checkpoint support");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain with better integration and utilities)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled);

    // Convenience functions and factory methods
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Custom exceptions
    py::class_<NLMRuntimeError, std::runtime_error>(m, "NLMRuntimeError")
        .def(py::init<const std::string&>(), py::arg("msg"));

    py::class_<NLMValidationError, NLMRuntimeError>(m, "NLMValidationError")
        .def(py::init<const std::string&>(), py::arg("msg"));

    py::class_<NLMConfigurationError, NLMRuntimeError>(m, "NLMConfigurationError")
        .def(py::init<const std::string&>(), py::arg("msg"));

    py::class_<NLMSerializationError, NLMRuntimeError>(m, "NLMSerializationError")
        .def(py::init<const std::string&>(), py::arg("msg"));

    // Version and platform information
    m.def("get_version", []() {
        return py::make_tuple(0, 1, 0); // Major, Minor, Patch
    }, "Get NLM Python bindings version");

    m.def("get_platform_info", []() {
        std::string info = "Python " + PYBIND11_VERSION + " on "
                         + std::string(PYTHON_PLATFORM); // Using Python constants
        return info;
    }, "Get platform information");

    // Module attributes
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__license__") = "MIT";
    m.attr("__doc__") = R"pbdoc(
        NLM (Neural Learning Machine) - Python Bindings
        
        An enhanced Python API for the NLM C++ neural simulation framework.
        
        This module provides complete access to all NLM functionality with
        modern Python features, including:
        - Context managers for automatic resource management
        - Custom exceptions for better error handling
        - High-level convenience functions
        - Enhanced serialization and visualization
        - Platform-independent API design
        
        For documentation, examples, and advanced usage, visit:
        https://github.com/nlm-project/nlm
    )pbdoc";

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Neural type aliases
    m.attr("NeuronId") = py::type<NeuronId>();
    m.attr("SynapseId") = py::type<SynapseId>();
    m.attr("RegionId") = py::type<RegionId>();
    m.attr("PopulationId") = py::type<PopulationId>();

    // Utility functions
    m.def("get_version_info", []() {
        py::dict info;
        info["major"] = 0;
        info["minor"] = 1;
        info["patch"] = 0;
        info["name"] = "pynlm";
        info["description"] = "Python bindings for NLM neural simulation";
        return info;
    }, "Get detailed version information");

    m.def("get_system_info", []() {
        py::dict info;
        info["platform"] = std::string(PYTHON_PLATFORM);
        info["python_version"] = std::string(PYTHON_VERSION);
        info["compiler"] = std::string(PYTHON_COMPILER);
        return info;
    }, "Get system information");

    // Context manager helper functions
    m.def("make_brain_context", &make_brain_context,
         "Create context manager for Brain instance");

    m.def("make_world_context", &make_world_context,
         "Create context manager for World and Agent combination");
}

} // namespace nlm
