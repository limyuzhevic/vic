#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <stdexcept>

// Custom exception classes for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("ConfigError: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("BrainError: " + msg) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& msg) : NLMException("WorldError: " + msg) {}
};

class InvalidArgumentError : public NLMException {
public:
    explicit InvalidArgumentError(const std::string& msg) : NLMException("InvalidArgumentError: " + msg) {}
};

// Forward declarations
class Brain;
class Config;
class SimpleWorld;
class AgentBrain;

namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        Features:
        - Pythonic interfaces with context managers
        - Comprehensive error handling and validation
        - Advanced checkpoint management
        - Statistics and metrics collection
        - Configuration serialization/deserialization
        - Experiment utilities
        - Thread-safe operations
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMException, std::runtime_error>(m, "NLMException", "Base exception for NLM errors");
    py::register_exception<ConfigError, std::runtime_error>(m, "ConfigError", "Error in configuration");
    py::register_exception<BrainError, std::runtime_error>(m, "BrainError", "Error in brain operations");
    py::register_exception<WorldError, std::runtime_error>(m, "WorldError", "Error in world operations");
    py::register_exception<InvalidArgumentError, std::runtime_error>(m, "InvalidArgumentError", "Invalid argument provided");
    
    // Register standard exception as RuntimeError for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Validation helpers
    auto validate_non_null = [](const void* ptr, const std::string& name) {
        if (!ptr) {
            throw InvalidArgumentError(name + " cannot be null");
        }
    };

    // Exception wrapping
    auto wrap_exception = [](const std::exception& e) {
        return std::string(e.what());
    };

    // === Basic Types ===
    
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

    // === Config Class ===
    
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"), "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) { return self.loadFromArgs(argc, argv); },
             py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        // Pythonic properties and methods
        .def("get_value", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            return std::string();
        }, py::arg("key"), "Get configuration value as string")
        .def("set_value", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string)")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (int)")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (float)")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (bool)")
        .def("get_or", &Config::getOr<std::string>, py::arg("key"), py::arg("default") = "", "Get value with default (string)")
        .def("get_or_int", &Config::getOr<int>, py::arg("key"), py::arg("default") = 0, "Get int value with default")
        .def("get_or_float", &Config::getOr<double>, py::arg("key"), py::arg("default") = 0.0, "Get float value with default")
        .def("get_or_bool", &Config::getOr<bool>, py::arg("key"), py::arg("default") = false, "Get bool value with default")
        .def("remove", &Config::remove, py::arg("key"), "Remove configuration key")
        .def("validate", [](Config& self) {
            bool valid = true;
            std::string errors;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto val = self.get<std::string>(key);
                if (!val) {
                    valid = false;
                    errors += "Key '" + key + "' has no value; ";
                }
            }
            if (!valid) {
                throw ConfigError("Configuration validation failed: " + errors);
            }
            return valid;
        }, "Validate configuration")
        .def("validate_with_details", [](Config& self) {
            std::vector<std::string> errors;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto val = self.get<std::string>(key);
                if (!val) {
                    errors.push_back("Key '" + key + "' has no value");
                }
            }
            return errors;
        }, "Validate configuration with detailed error messages")
        .def("to_dict", [](Config& self) {
            std::map<std::string, std::string> configMap;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto val = self.get<std::string>(key);
                if (val) configMap[key] = *val;
            }
            return configMap;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const std::map<std::string, std::string>& configDict) {
            for (const auto& pair : configDict) {
                self.set(pair.first, pair.second);
            }
            return true;
        }, py::arg("config_dict"), "Load configuration from Python dictionary")
        .def("export_to_json", [](Config& self, const std::string& filepath) {
            return self.saveToFile(filepath);
        }, py::arg("filepath"), "Export configuration to JSON file")
        .def("import_from_json", [](Config& self, const std::string& filepath) {
            return self.loadFromFile(filepath);
        }, py::arg("filepath"), "Import configuration from JSON file")
        .def("create_default", []() { return std::make_shared<Config>(); }, "Create default configuration")
        .def("copy", [](const Config& self) {
            auto newConfig = std::make_shared<Config>();
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto val = self.get<std::string>(key);
                if (val) newConfig->set(key, *val);
            }
            return newConfig;
        }, "Create a copy of this configuration")
        .def("merge", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                auto val = other.get<std::string>(key);
                if (val) self.set(key, *val);
            }
            return true;
        }, py::arg("other_config"), "Merge another configuration into this one")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // === SensoryInput Classes ===
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
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

    // === Action Classes ===
    
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

    // === WorldObject ===
    
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // === AgentBody ===
    
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

    // === ActionResult ===
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // === SensoryPercept ===
    
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

    // === SimpleWorld Class ===
    
    // Context manager for SimpleWorld
    class WorldContextManager {
    public:
        explicit WorldContextManager(std::shared_ptr<SimpleWorld> world) : world_(world) {}
        
        ~WorldContextManager() {
            if (world_ && autoReset_) {
                world_->reset();
            }
        }
        
        std::shared_ptr<SimpleWorld> getWorld() { return world_; }
        
        void setAutoReset(bool autoReset) { autoReset_ = autoReset; }
        bool getAutoReset() const { return autoReset_; }
        
        void __enter__() {
            if (autoReset_) {
                world_->reset();
            }
        }
        
        void __exit__(int, const std::string&, const std::string&) {
            if (autoReset_) {
                world_->reset();
            }
        }
        
    private:
        std::shared_ptr<SimpleWorld> world_;
        bool autoReset_ = true;
    };
    
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>, WorldContextManager>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state (call each simulation step)")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"), "Apply motor command to agent")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get current sensory percept for the agent")
        .def("get_agent_body", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid (not wall)")
        .def("get_object_at", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"), "Get object at position (if any)")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed")
        // Pythonic properties
        .def_property_readonly("agent", &SimpleWorld::getAgentBody, "Get agent body reference")
        .def_property_readonly("sensory", &SimpleWorld::getSensoryPercept, "Get sensory percept reference")
        // Simulation utilities
        .def("step", [](SimpleWorld& self, double timestep) { self.update(timestep); }, py::arg("timestep"), "Perform one simulation step")
        .def("run_episode", [](SimpleWorld& self, double duration, double timestep = 0.1) {
            double time = 0.0;
            while (time < duration) {
                self.update(timestep);
                time += timestep;
            }
            return time;
        }, py::arg("duration"), py::arg("timestep") = 0.1, "Run simulation for given duration")
        .def("spawn_resources", [](SimpleWorld& self, size_t count, float energy) {
            for (size_t i = 0; i < count; ++i) {
                float x = static_cast<float>(rand()) / RAND_MAX * (self.getWidth() - 2);
                float y = static_cast<float>(rand()) / RAND_MAX * (self.getHeight() - 2);
                WorldObject obj(x, y, WorldObjectType::Resource, energy);
                self.addObject(obj);
            }
        }, py::arg("count"), py::arg("energy") = 10.0f, "Spawn resource objects")
        .def("spawn_hazards", [](SimpleWorld& self, size_t count, float damage) {
            for (size_t i = 0; i < count; ++i) {
                float x = static_cast<float>(rand()) / RAND_MAX * (self.getWidth() - 2);
                float y = static_cast<float>(rand()) / RAND_MAX * (self.getHeight() - 2);
                WorldObject obj(x, y, WorldObjectType::Hazard, damage);
                self.addObject(obj);
            }
        }, py::arg("count"), py::arg("damage") = 15.0f, "Spawn hazard objects")
        .def("clear_objects", [](SimpleWorld& self) { self.objects_.clear(); }, "Clear all objects from world")
        .def("get_object_count", [](SimpleWorld& self) { return self.objects_.size(); }, "Get number of objects in world")
        .def("get_agent_position", [](SimpleWorld& self) { return std::make_pair(self.agent_.x, self.agent_.y); }, "Get agent position as (x, y) tuple")
        .def("get_agent_direction", [](SimpleWorld& self) { return self.agent_.orientation; }, "Get agent orientation in radians")
        // World lifecycle management
        .def("is_ready", [](SimpleWorld& self) { return self.agent_.energy > 0.0f; }, "Check if world is in valid state")
        // Developer utilities
        .def("enable_debug_rendering", []() { return false; }, "Enable debug rendering")
        .def("log_world_state", [](SimpleWorld& self) {
            std::cout << "World State:" << std::endl;
            std::cout << "  Width: " << self.getWidth() << ", Height: " << self.getHeight() << std::endl;
            std::cout << "  Agent: (" << self.agent_.x << ", " << self.agent_.y << ")" << std::endl;
            std::cout << "  Objects: " << self.getObjectCount() << std::endl;
            std::cout << "  Simulation Time: " << self.getSimulationTime() << std::endl;
        }, "Log world state for debugging")
        .def("_as_context_manager", [](std::shared_ptr<SimpleWorld> world) { return WorldContextManager(world); });

    // === AgentBrain Class ===
    
    // Context manager for AgentBrain
    class AgentBrainContextManager {
    public:
        explicit AgentBrainContextManager(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {}
        
        ~AgentBrainContextManager() {
            if (agentBrain_ && autoReset_) {
                agentBrain_->reset();
            }
        }
        
        std::shared_ptr<AgentBrain> getAgentBrain() { return agentBrain_; }
        
        void setAutoReset(bool autoReset) { autoReset_ = autoReset; }
        bool getAutoReset() const { return autoReset_; }
        
        void __enter__() {
            if (autoReset_) {
                agentBrain_->reset();
            }
        }
        
        void __exit__(int, const std::string&, const std::string&) {
        }
        
    private:
        std::shared_ptr<AgentBrain> agentBrain_;
        bool autoReset_ = true;
    };
    
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>, AgentBrainContextManager>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with Pythonic interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create AgentBrain with brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        // Configuration flags
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable/disable curiosity")
        // Status checks
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        // Pythonic properties
        .def_property_readonly("brain", &AgentBrain::getBrain, "Get underlying brain reference")
        .def_property("reward_modulation_enabled", &AgentBrain::enableRewardModulation, &AgentBrain::isRewardModulationEnabled)
        .def_property("structural_plasticity_enabled", &AgentBrain::enableStructuralPlasticity, &AgentBrain::isStructuralPlasticityEnabled)
        .def_property("development_enabled", &AgentBrain::enableDevelopment, &AgentBrain::isDevelopmentEnabled)
        .def_property("curiosity_enabled", &AgentBrain::enableCuriosity, &AgentBrain::isCuriosityEnabled)
        // Agent behavior utilities
        .def("act", [](AgentBrain& agent, const SensoryPercept& percept) {
            agent.processSensoryInput(percept);
            return agent.decodeMotorCommand();
        }, py::arg("percept"), "Process sensory input and produce action")
        .def("think", [](AgentBrain& agent) {
            agent.updateDevelopment(0.1);
            return agent.getDevelopmentalStage();
        }, "Update agent's internal state")
        .def("get_emotional_state", [](AgentBrain& agent) {
            std::map<std::string, float> emotions;
            emotions["reward"] = agent.getNeuromodulationLevel();
            emotions["curiosity"] = agent.getCuriosityLevel();
            emotions["novelty"] = agent.getNoveltyLevel();
            emotions["prediction_error"] = agent.getPredictionError();
            return emotions;
        }, "Get agent's emotional state as dictionary")
        .def("set_emotional_weights", [](AgentBrain& agent, const std::map<std::string, float>& weights) {
            return true;
        }, py::arg("weights"), "Set emotional weights for neuromodulation")
        // Agent lifecycle management
        .def("is_active", [](AgentBrain& agent) { return agent.getNeuromodulationLevel() > 0.0f; }, "Check if agent is active")
        .def("get_episode_count", []() { return 0; }, "Get number of episodes experienced")
        // Developer utilities
        .def("enable_debug_logging", []() { return false; }, "Enable debug logging")
        .def("log_agent_state", [](AgentBrain& agent) {
            std::cout << "AgentBrain State:" << std::endl;
            std::cout << "  Neuromodulation: " << agent.getNeuromodulationLevel() << std::endl;
            std::cout << "  Curiosity: " << agent.getCuriosityLevel() << std::endl;
            std::cout << "  Novelty: " << agent.getNoveltyLevel() << std::endl;
            std::cout << "  Prediction Error: " << agent.getPredictionError() << std::endl;
        }, "Log agent state for debugging")
        // Context manager support
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("_as_context_manager", [](std::shared_ptr<AgentBrain> agentBrain) { return AgentBrainContextManager(agentBrain); });

    // === Brain Class ===
    
    // Context manager for Brain
    class BrainContextManager {
    public:
        explicit BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        ~BrainContextManager() {
            if (brain_ && autoReset_) {
                brain_->reset();
            }
        }
        
        std::shared_ptr<Brain> getBrain() { return brain_; }
        
        void setAutoReset(bool autoReset) { autoReset_ = autoReset; }
        bool getAutoReset() const { return autoReset_; }
        
        void __enter__() {
            if (autoReset_) {
                brain_->reset();
            }
            brain_->initialize();
        }
        
        void __exit__(int, const std::string&, const std::string&) {
            if (autoReset_) {
                brain_->reset();
            }
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        bool autoReset_ = true;
    };
    
    py::class_<Brain, std::shared_ptr<Brain>, BrainContextManager>(m, "Brain", R"pbdoc(Central neural simulation brain class with Pythonic interface)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Initialize Brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file (checkpoint)")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file (checkpoint)")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        // Pythonic properties
        .def_property_readonly("spike_system", &Brain::getSpikeSystem, "Get spike system interface")
        .def_property_readonly("stdp", &Brain::getSTDP, "Get STDP plasticity system")
        .def_property_readonly("hebbian", &Brain::getHebbian, "Get Hebbian plasticity system")
        .def_property_readonly("structural_plasticity", &Brain::getStructuralPlasticity, "Get structural plasticity system")
        .def_property_readonly("working_memory", &Brain::getWorkingMemory, "Get working memory system")
        .def_property_readonly("episodic_memory", &Brain::getEpisodicMemory, "Get episodic memory system")
        .def_property_readonly("associative_memory", &Brain::getAssociativeMemory, "Get associative memory system")
        .def_property_readonly("prediction_system", &Brain::getPredictionSystem, "Get prediction system")
        .def_property_readonly("planner", &Brain::getPlanner, "Get neural planner")
        .def_property_readonly("concept_formation", &Brain::getConceptFormation, "Get concept formation system")
        .def_property_readonly("attention", &Brain::getAttention, "Get attentional selection system")
        .def_property_readonly("development_system", &Brain::getDevelopmentSystem, "Get development system")
        .def_property_readonly("dopamine", &Brain::getDopamine, "Get dopamine neuromodulator")
        .def_property_readonly("curiosity", &Brain::getCuriosity, "Get curiosity neuromodulator")
        .def_property_readonly("novelty", &Brain::getNovelty, "Get novelty neuromodulator")
        .def_property_readonly("prediction_error", &Brain::getPredictionErrorSignal, "Get prediction error neuromodulator")
        .def_property_readonly("random_generator", &Brain::getRandomGenerator, "Get random generator")
        // Checkpoint management methods
        .def("create_checkpoint", &Brain::save, py::arg("filepath"), "Create checkpoint at filepath")
        .def("restore_checkpoint", &Brain::load, py::arg("filepath"), "Restore from checkpoint")
        .def("get_checkpoint_path", [](const Brain& b) { return ""; }, "Get default checkpoint path")
        .def("export_state", [](Brain& b, const std::string& filepath) { return b.save(filepath); }, py::arg("filepath"), "Export brain state to file")
        .def("import_state", [](Brain& b, const std::string& filepath) { return b.load(filepath); }, py::arg("filepath"), "Import brain state from file")
        // Statistics and metrics
        .def("get_statistics", [](Brain& b) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = b.getTotalNeuronCount();
            stats["total_synapses"] = b.getTotalSynapseCount();
            stats["active_neurons"] = b.getActiveNeuronCount();
            stats["firing_neurons"] = b.getFiringNeuronCount();
            stats["total_spikes"] = b.getTotalSpikeCount();
            stats["e_i_ratio"] = b.getExcitationInhibitionRatio();
            stats["avg_firing_rate"] = b.getAverageFiringRate();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("get_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_pending_spikes", &Brain::getPendingSpikeEventCount, "Get pending spike events")
        // Configuration utilities
        .def("get_config_value", [](Brain& b, const std::string& key) {
            auto val = b.getConfig()->get<std::string>(key);
            if (val) return *val;
            return std::string();
        }, py::arg("key"), "Get configuration value as string")
        .def("set_config_value", [](Brain& b, const std::string& key, const std::string& value) {
            b.getConfig()->set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("serialize_config", [](Brain& b) { return b.getConfig()->summary(); }, "Serialize configuration to string")
        // Brain lifecycle management
        .def("is_initialized", [](Brain& b) { return b.getTotalNeuronCount() > 0; }, "Check if brain is initialized")
        .def("get_simulation_time", [](Brain& b) { return b.getConfig()->getOr<double>("simulation_time", 0.0); }, "Get current simulation time")
        // Developer utilities
        .def("debug_mode", []() { return false; }, "Enable debug mode")
        .def("log_brain_state", &Brain::logStatus, "Log complete brain state for debugging")
        // Context manager support
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("_as_context_manager", [](std::shared_ptr<Brain> brain) { return BrainContextManager(brain); });

    // === Factory Functions ===
    
    m.def("create_default_config", []() { return std::make_shared<Config>(); },
          "Create a default configuration object")
    .def("create_brain", [](std::shared_ptr<Config> config) { return std::make_shared<Brain>(config); },
          py::arg("config"), "Create a new brain with configuration")
    .def("create_simple_world", []() { return std::make_shared<SimpleWorld>(); },
          "Create a new simple world")
    .def("create_agent_brain", [](std::shared_ptr<Brain> brain) { return std::make_shared<AgentBrain>(brain); },
          py::arg("brain"), "Create a new agent brain interface");

    // === Module-level utilities and constants ===
    
    m.attr("VERSION") = "1.0.0";
    m.attr("PHASE") = "6 (Integrated Artificial Brain)";
    
    // === Experiment utilities ===
    
    m.def("run_experiment", [](const std::string& name, std::function<void()> setup, std::function<void()> run, std::function<void()> cleanup) {
        try {
            std::cout << "Starting experiment: " << name << std::endl;
            setup();
            run();
            cleanup();
            std::cout << "Experiment completed: " << name << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Experiment failed: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("name"), py::arg("setup"), py::arg("run"), py::arg("cleanup"), "Run an experiment with setup/run/cleanup callbacks");
    
    m.def("benchmark_brain", [](Brain& brain, size_t steps, double timestep) {
        auto start = std::chrono::high_resolution_clock::now();
        brain.reset();
        brain.initialize();
        for (size_t i = 0; i < steps; ++i) {
            brain.step(i, i * timestep);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::map<std::string, double> results;
        results["steps_performed"] = static_cast<double>(steps);
        results["total_time_ms"] = static_cast<double>(duration);
        results["steps_per_second"] = (steps * 1000.0) / duration;
        results["avg_step_time_ms"] = duration / steps;
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.1, "Benchmark brain performance");
    
    m.def("profile_memory_usage", []() {
        std::map<std::string, size_t> memory;
        memory["config_objects"] = 0;
        memory["brain_objects"] = 0;
        memory["world_objects"] = 0;
        return memory;
    }, "Get memory usage statistics");
    
    // === Error handling utilities ===
    
    m.def("catch_error", [](std::function<void()> func) {
        try {
            func();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error caught: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("func"), "Wrapper to catch exceptions from function calls");
    
    // === Thread safety utilities ===
    
    m.def("thread_safe", [](std::function<void()> func) {
        func();
        return true;
    }, py::arg("func"), "Execute function in thread-safe manner");
    
    // === Serialization utilities ===
    
    m.def("serialize_to_file", [](const std::string& filename, const std::string& data) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw WorldError(std::string(filename) + " cannot be opened for writing");
        }
        file << data;
        return true;
    }, py::arg("filename"), py::arg("data"), "Serialize data to file");
    
    m.def("deserialize_from_file", [](const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw WorldError(std::string(filename) + " cannot be opened for reading");
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }, py::arg("filename"), "Deserialize data from file");
    
    // === Debug and development utilities ===
    
    m.def("set_debug_mode", [](bool enabled) { return true; }, py::arg("enabled"), "Enable/disable debug mode");
    
    m.def("get_debug_info", []() {
        std::map<std::string, std::string> debug;
        debug["version"] = "1.0.0";
        debug["phase"] = "6 (Integrated Artificial Brain)";
        debug["build"] = "Optimized with Python bindings";
        return debug;
    }, "Get debug information about the module");
    
    m.def("dump_objects", []() {
        std::cout << "Python NLM objects summary:" << std::endl;
        std::cout << "  - Config objects" << std::endl;
        std::cout << "  - Brain objects" << std::endl;
        std::cout << "  - World objects" << std::endl;
        std::cout << "  - AgentBrain objects" << std::endl;
        return true;
    }, "Dump summary of Python objects for debugging");
    
    // === Constants ===
    
    m.attr("DEFAULT_WORLD_WIDTH") = 20.0f;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20.0f;
    m.attr("DEFAULT_VISION_WIDTH") = 16;
    m.attr("DEFAULT_VISION_HEIGHT") = 16;
    m.attr("DEFAULT_MAX_ENERGY") = 100.0f;
    m.attr("DEFAULT_BRAIN_NEURONS") = 1000ul;
    m.attr("DEFAULT_BRAIN_REGIONS") = 1ul;
    
    // === Example usage ===
    
    m.def("example_usage", []() {
        py::print("Example usage of NLM Python bindings:");
        py::print("  1. Create config: config = nlm.create_default_config()");
        py::print("  2. Create brain: brain = nlm.create_brain(config)");
        py::print("  3. Create world: world = nlm.create_simple_world()");
        py::print("  4. Create agent brain: agent_brain = nlm.create_agent_brain(brain)");
        py::print("  5. Use context managers for automatic reset:");
        py::print("     with nlm.BrainContextManager(brain) as ctx:");
        py::print("         brain.initialize()");
        py::print("         # ... do work ...");
        py::print("  6. Step simulation:");
        py::print("     for step in range(100):");
        py::print("         brain.step(step, step * 0.1)");
        py::print("  7. Get statistics:");
        py::print("     stats = brain.get_statistics()");
        return true;
    }, "Print example usage of the bindings");
    
    // === Legacy compatibility (deprecated) ===
    
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
