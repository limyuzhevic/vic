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

namespace nlm {

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class InvalidArgumentError : public std::invalid_argument {
public:
    explicit InvalidArgumentError(const std::string& message) : std::invalid_argument(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration error: " + message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError("Simulation error: " + message) {}
};

// Helper function for type checking
auto check_positive = [](double value, const char* param_name) {
    if (value <= 0.0) {
        throw InvalidArgumentError("Parameter " + std::string(param_name) + " must be positive, got " + std::to_string(value));
    }
};
    
auto check_non_negative = [](double value, const char* param_name) {
    if (value < 0.0) {
        throw InvalidArgumentError("Parameter " + std::string(param_name) + " must be non-negative, got " + std::to_string(value));
    }
};
    
auto check_range = [](double value, double min, double max, const char* param_name) {
    if (value < min || value > max) {
        throw InvalidArgumentError("Parameter " + std::string(param_name) + " must be in range [" + 
                                 std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value));
    }
};
    
auto check_size = [](size_t size, size_t max_size, const char* param_name) {
    if (size > max_size) {
        throw InvalidArgumentError("Parameter " + std::string(param_name) + " size " + 
                                 std::to_string(size) + " exceeds maximum " + std::to_string(max_size));
    }
};

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Custom exceptions for better error handling
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError", PyExc_ValueError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // Helper function for type checking
    auto check_positive = [](double value, const char* param_name) {
        if (value <= 0.0) {
            throw InvalidArgumentError("Parameter " + std::string(param_name) + " must be positive, got " + std::to_string(value));
        }
    };
    
    auto check_non_negative = [](double value, const char* param_name) {
        if (value < 0.0) {
            throw InvalidArgumentError("Parameter " + std::string(param_name) + " must be non-negative, got " + std::to_string(value));
        }
    };
    
    auto check_range = [](double value, double min, double max, const char* param_name) {
        if (value < min || value > max) {
            throw InvalidArgumentError("Parameter " + std::string(param_name) + " must be in range [" + 
                                     std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value));
        }
    };
    
    auto check_size = [](size_t size, size_t max_size, const char* param_name) {
        if (size > max_size) {
            throw InvalidArgumentError("Parameter " + std::string(param_name) + " size " + 
                                     std::to_string(size) + " exceeds maximum " + std::to_string(max_size));
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Config>& self) { return self; })
        .def("__exit__", [](std::shared_ptr<Config>& /*self*/, py::object /*type*/, py::object /*value*/, py::object /*traceback*/) {
            // Config uses shared_ptr, automatic cleanup
        })
        // Enhanced setters with validation
        .def("set", [](Config& self, const std::string& key, double value) {
            if (key.empty()) {
                throw InvalidArgumentError("Key cannot be empty");
            }
            if (key.find('.') == std::string::npos) {
                throw InvalidArgumentError("Key must be a dot-separated path (e.g., 'brain.neuron_count')");
            }
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value with validation")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            if (key.empty()) {
                throw InvalidArgumentError("Key cannot be empty");
            }
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration string value with validation")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            if (key.empty()) {
                throw InvalidArgumentError("Key cannot be empty");
            }
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration integer value with validation")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            if (key.empty()) {
                throw InvalidArgumentError("Key cannot be empty");
            }
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration boolean value with validation")
        // Pythonic property access for common Config methods
        .def("__getitem__", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key '" + key + "' not found");
            }
            // Get the value as string (simplified for demonstration)
            return self.get<std::string>(key);
        }, py::arg("key"),
            "Get configuration value using Pythonic syntax (config['key'])")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value using Pythonic syntax (config['key'] = value)")
        .def("__contains__", &Config::has,
             "Check if key exists (using 'in' operator)")
        .def("__len__", &Config::getKeys,
             "Get number of configuration keys (using len())")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, float width, float height, 
                           size_t visionWidth, size_t visionHeight) {
            check_positive(width, "width");
            check_positive(height, "height");
            check_size(visionWidth, 1024, "visionWidth");
            check_size(visionHeight, 1024, "visionHeight");
            return self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           "Configure world with validation")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            check_range(x, -1000.0f, 1000.0f, "x");
            check_range(y, -1000.0f, 1000.0f, "y");
            return self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent start position with validation")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            check_range(obj.x, -1000.0f, 1000.0f, "object.x");
            check_range(obj.y, -1000.0f, 1000.0f, "object.y");
            check_positive(obj.radius, "object.radius");
            check_range(obj.value, -10.0f, 10.0f, "object.value");
            return self.addObject(obj);
        }, py::arg("obj"), "Add world object with validation")
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            check_range(x, -1000.0f, 1000.0f, "x");
            check_range(y, -1000.0f, 1000.0f, "y");
            return self.removeObject(x, y);
        }, py::arg("x"), py::arg("y"), "Remove object at position with validation")
        .def("isValidPosition", [](SimpleWorld& self, float x, float y) {
            check_range(x, -1000.0f, 1000.0f, "x");
            check_range(y, -1000.0f, 1000.0f, "y");
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), "Check if position is valid with validation")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            check_positive(e, "maxEnergy");
            return self.setMaxEnergy(e);
        }, py::arg("e"), "Set maximum energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            check_non_negative(r, "energyDecayRate");
            return self.setEnergyDecayRate(r);
        }, py::arg("r"), "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Pythonic property access for common SimpleWorld methods
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width (using getWidth())")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height (using getHeight())")
        .def("getVisionWidth", &SimpleWorld::getVisionWidth,
             "Get vision width (using getVisionWidth())")
        .def("getVisionHeight", &SimpleWorld::getVisionHeight,
             "Get vision height (using getVisionHeight())")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy (using getMaxEnergy())")
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            check_positive(e, "maxEnergy");
            return self.setMaxEnergy(e);
        }, py::arg("e"), "Set maximum energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate (using getEnergyDecayRate())")
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            check_non_negative(r, "energyDecayRate");
            return self.setEnergyDecayRate(r);
        }, py::arg("r"), "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time (using getSimulationTime())")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed (using getRandomSeed())")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + 
                   "x" + std::to_string(world.getHeight()) + ", " +
                   "vision:" + std::to_string(world.getVisionWidth()) + 
                   "x" + std::to_string(world.getVisionHeight()) + ", " +
                   "time:" + std::to_string(world.getSimulationTime()) + ">";
        })
        // Context manager support
        .def("__enter__", [](std::shared_ptr<SimpleWorld>& self) { return self; })
        .def("__exit__", [](std::shared_ptr<SimpleWorld>& /*self*/, py::object /*type*/, py::object /*value*/, py::object /*traceback*/) {
            // SimpleWorld uses shared_ptr, automatic cleanup
        });

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
        // Batch operations for efficiency
        .def("stepMany", [](Brain& self, size_t numSteps, size_t startStep = 0, bool withTimestamps = false) {
            if (numSteps == 0) {
                throw InvalidArgumentError("Number of steps must be positive");
            }
            if (startStep > 1000000) {
                throw InvalidArgumentError("Start step too large, maximum allowed is 1000000");
            }
            if (withTimestamps) {
                for (size_t i = 0; i < numSteps; ++i) {
                    self.step(static_cast<SimulationStep>(startStep + i), 
                             static_cast<Timestamp>(startStep + i) * 0.1);
                }
            } else {
                for (size_t i = 0; i < numSteps; ++i) {
                    self.step(static_cast<SimulationStep>(startStep + i));
                }
            }
        }, py::arg("numSteps"), py::arg("startStep") = 0, py::arg("withTimestamps") = false,
            "Run multiple simulation steps efficiently")
        .def("receiveSensoryInputs", [](Brain& self, const std::vector<const SensoryInput*>& inputs) {
            if (inputs.size() > 1000) {
                throw InvalidArgumentError("Too many sensory inputs, maximum allowed is 1000");
            }
            for (const auto& input : inputs) {
                self.receiveSensoryInput(*input);
            }
        }, py::arg("inputs"),
             "Process multiple sensory inputs efficiently")
        .def("injectCurrents", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& currents) {
            for (const auto& pair : currents) {
                self.injectCurrent(pair.first, pair.second);
            }
        }, py::arg("currents"),
             "Inject current into multiple neurons efficiently")
        .def("produceActions", [](Brain& self, size_t count) {
            if (count == 0) {
                throw InvalidArgumentError("Action count must be positive");
            }
            std::vector<std::unique_ptr<Action>> actions;
            actions.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                actions.push_back(self.produceAction());
            }
            return actions;
        }, py::arg("count"),
             "Produce multiple actions efficiently")
        .def("getStatistics", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive statistics as dictionary")
        .def("getStatusSummary", &Brain::logStatus,
             "Get brain status summary (logs to stdout)")
        // Pythonic property access for common Brain methods
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (using getNeuronCount())")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (using getSynapseCount())")
        .def("getFiringNeurons", &Brain::getFiringNeuronCount,
             "Get number of firing neurons (using getFiringNeurons())")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate (using getAverageFiringRate())")
        .def("getEIRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio (using getEIRatio())")
        .def("getSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count (using getSpikeCount())")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get developmental stage (using getDevelopmentalStage())")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (using setDevelopmentalStage())")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get configuration (using getConfig())")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses>";
        })
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain>& self) { return self; })
        .def("__exit__", [](std::shared_ptr<Brain>& /*self*/, py::object /*type*/, py::object /*value*/, py::object /*traceback*/) {
            // Brain uses shared_ptr, automatic cleanup
        });

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, const SimpleWorld& world) {
            check_positive(world.getWidth(), "world.width");
            check_positive(world.getHeight(), "world.height");
            return self.initialize(world);
        }, py::arg("world"),
             "Initialize with world with validation")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            if (percept.getVisionWidth() > 0) {
                check_size(percept.getVisionWidth(), 10000, "vision width");
                check_size(percept.getVisionHeight(), 10000, "vision height");
            }
            if (percept.getTouch().size() > 0) {
                check_size(percept.getTouch().size(), 1000, "touch vector size");
            }
            if (percept.getInternal().size() > 0) {
                check_size(percept.getInternal().size(), 1000, "internal signals size");
            }
            return self.processSensoryInput(percept);
        }, py::arg("percept"),
             "Process sensory percept and inject into brain with validation")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        // Batch operations for efficiency
        .def("decodeMotorCommands", [](AgentBrain& self, size_t count) {
            if (count == 0) {
                throw InvalidArgumentError("Command count must be positive");
            }
            std::vector<MotorCommand> commands;
            commands.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                commands.push_back(self.decodeMotorCommand());
            }
            return commands;
        }, py::arg("count"),
             "Decode multiple motor commands efficiently")
        .def("applyRewardModulations", [](AgentBrain& self, const std::vector<std::pair<float, float>>& rewards) {
            if (rewards.size() > 1000) {
                throw InvalidArgumentError("Too many rewards, maximum allowed is 1000");
            }
            for (const auto& pair : rewards) {
                self.applyRewardModulation(pair.first, pair.second);
            }
        }, py::arg("rewards"),
             "Apply multiple reward modulations efficiently")
        .def("getAgentStatistics", [](AgentBrain& self) {
            py::dict stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics as dictionary")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get agent thought trace")
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activities")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get agent thought trace (using getThoughtTrace())")
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activities (using getMotorGroupActivities())")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal firing rates (using getInternalFiringRates())")
        .def("getTextAssociationProbe", [](AgentBrain& self, const std::string& text) {
            std::vector<float> rates = self.getInternalFiringRates();
            std::vector<size_t> indices = AgentBrain::encodeText(text);
            float associationStrength = 0.0f;
            for (size_t idx : indices) {
                if (idx < rates.size()) {
                    associationStrength += rates[idx];
                }
            }
            return std::make_pair(text, associationStrength);
        }, py::arg("text"),
             "Get association strength for text (Phase 6 feature)")
        .def("getBrainStats", [](AgentBrain& self) {
            py::dict stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics as dictionary (using getBrainStats())")
        // Pythonic property access for common AgentBrain methods
        .def("getCuriosity", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (using getCuriosity())")
        .def("getNovelty", &AgentBrain::getNoveltyLevel,
             "Get novelty level (using getNovelty())")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error (using getPredictionError())")
        .def("getNeuromodulation", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level (using getNeuromodulation())")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage (using getDevelopmentalStage())")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   ", novelty=" + std::to_string(agent.getNoveltyLevel()) + ", " +
                   "stage=" + std::to_string(static_cast<int>(agent.getDevelopmentalStage())) + ">";
        })
        // Context manager support
        .def("__enter__", [](std::shared_ptr<AgentBrain>& self) { return self; })
        .def("__exit__", [](std::shared_ptr<AgentBrain>& /*self*/, py::object /*type*/, py::object /*value*/, py::object /*traceback*/) {
            // AgentBrain uses shared_ptr, automatic cleanup
        });

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

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Factory functions for common use cases
    m.def("createBrainForSimulation", []() {
        auto config = std::make_shared<Config>();
        // Set up typical simulation configuration
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a brain pre-configured for simulation");

    m.def("createAgentForTask", [](std::shared_ptr<Brain> brain, const std::shared_ptr<SimpleWorld>& world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        // Enable typical agent capabilities
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an agent pre-configured for task execution");

    m.def("createSimpleWorldForAgent", [](size_t width, size_t height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        return world;
    }, py::arg("width"), py::arg("height"),
        "Create a simple world pre-configured for agent testing");

    m.def("createDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        // Development-focused configuration
        config->set("brain.neuron_count", 2000); // More neurons for development
        config->set("brain.synapse_density", 0.05);
        config->set("brain.initial_weight_mean", 0.3);
        config->set("brain.initial_weight_std", 0.05);
        config->set("development.stage", "CriticalPeriod");
        config->set("development.plasticity_rate", 0.5);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", true);
        return config;
    }, "Create a development-focused configuration");

    m.def("createLearningConfig", []() {
        auto config = std::make_shared<Config>();
        // Learning-focused configuration
        config->set("brain.neuron_count", 1500);
        config->set("brain.synapse_density", 0.15);
        config->set("brain.v_thresh", -40.0); // Higher threshold for more selective firing
        config->set("brain.v_rest", -65.0);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.005);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.5);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("prediction.enable", true);
        return config;
    }, "Create a learning-focused configuration");

    m.def("createExplorationConfig", []() {
        auto config = std::make_shared<Config>();
        // Exploration-focused configuration
        config->set("brain.neuron_count", 800);
        config->set("brain.synapse_density", 0.08);
        config->set("brain.v_thresh", -55.0);
        config->set("brain.v_rest", -70.0);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 0.8);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.strength", 2.0); // Strong curiosity for exploration
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 1.5);
        return config;
    }, "Create an exploration-focused configuration");

    m.def("createMinimalConfig", []() {
        return std::make_shared<Config>(); // Use default configuration
    }, "Create a minimal configuration");

    m.def("createDemoConfiguration", [](const std::string& demoType) {
        auto config = std::make_shared<Config>();
        
        if (demoType == "sensory") {
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.12);
            config->set("brain.v_thresh", -50.0);
            config->set("sensory.vision.width", 16);
            config->set("sensory.vision.height", 16);
        } else if (demoType == "motor") {
            config->set("brain.neuron_count", 750);
            config->set("brain.synapse_density", 0.08);
            config->set("brain.v_thresh", -45.0);
            config->set("sensory.touch.enabled", true);
            config->set("sensory.proprionception.enabled", true);
        } else if (demoType == "cognitive") {
            config->set("brain.neuron_count", 1200);
            config->set("brain.synapse_density", 0.06);
            config->set("brain.v_thresh", -40.0);
            config->set("cognition.planner.enabled", true);
            config->set("cognition.concept_formation.enabled", true);
            config->set("cognition.attention.enabled", true);
        } else {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
        }
        
        return config;
    }, py::arg("demoType"),
        "Create a configuration for a specific demo type (sensory, motor, cognitive, default)");

    m.def("createDemoConfiguration", [](const std::string& demoType) {
        auto config = std::make_shared<Config>();
        
        if (demoType == "sensory") {
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.12);
            config->set("brain.v_thresh", -50.0);
            config->set("sensory.vision.width", 16);
            config->set("sensory.vision.height", 16);
        } else if (demoType == "motor") {
            config->set("brain.neuron_count", 750);
            config->set("brain.synapse_density", 0.08);
            config->set("brain.v_thresh", -45.0);
            config->set("sensory.touch.enabled", true);
            config->set("sensory.proprionception.enabled", true);
        } else if (demoType == "cognitive") {
            config->set("brain.neuron_count", 1200);
            config->set("brain.synapse_density", 0.06);
            config->set("brain.v_thresh", -40.0);
            config->set("cognition.planner.enabled", true);
            config->set("cognition.concept_formation.enabled", true);
            config->set("cognition.attention.enabled", true);
        } else {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
        }
        
        return config;
    }, py::arg("demoType"),
        "Create a configuration for a specific demo type (sensory, motor, cognitive, default)");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register simulation utilities
    register_simulation_utilities(m);
}

} // namespace nlm
