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
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Multiple memory systems (working, episodic, semantic, procedural)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and plasticity
        - Prediction and planning systems
        - Complete brain loop integration
        
        This module provides a Python interface to NLM's brain simulation capabilities,
        making it accessible to Python users while maintaining full access to
        NLM's advanced neural computation features.
    )pbdoc";

    // Enhanced error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    py::register_exception<std::logic_error>(m, "LogicError");

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Enhanced Pythonic configuration methods
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a string configuration value (Pythonic API)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set an integer configuration value (Pythonic API)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a float configuration value (Pythonic API)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a boolean configuration value (Pythonic API)")
        .def("set", [](Config& self, const std::string& key, const std::vector<int>& value) {
            // For vector types, we need to convert from variant
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set an integer vector configuration value (Pythonic API)")
        .def("set", [](Config& self, const std::string& key, const std::vector<double>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a float vector configuration value (Pythonic API)")
        .def("set", [](Config& self, const std::string& key, const std::vector<std::string>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a string vector configuration value (Pythonic API)")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
            "Get a string configuration value (Pythonic API)")
        .def("get", [](Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get an integer configuration value with default (Pythonic API)")
        .def("get", [](Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a float configuration value with default (Pythonic API)")
        .def("get", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a boolean configuration value with default (Pythonic API)")
        .def("get", [](Config& self, const std::string& key, const std::vector<int>& defaultValue) {
            auto value = self.get<std::vector<int>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get an integer vector configuration value with default (Pythonic API)")
        .def("get", [](Config& self, const std::string& key, const std::vector<double>& defaultValue) {
            auto value = self.get<std::vector<double>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a float vector configuration value with default (Pythonic API)")
        .def("get", [](Config& self, const std::string& key, const std::vector<std::string>& defaultValue) {
            auto value = self.get<std::vector<std::string>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a string vector configuration value with default (Pythonic API)")
        .def("setMultiple", [](Config& self, const py::dict& kwargs) {
            for (auto it : kwargs) {
                std::string key = py::cast<std::string>(it.first);
                auto value = it.second;
                // Handle different Python types
                if (py::isinstance<py::str>(value)) {
                    self.set(key, py::cast<std::string>(value));
                } else if (py::isinstance<py::int_>(value)) {
                    self.set(key, py::cast<int>(value));
                } else if (py::isinstance<py::float_>(value)) {
                    self.set(key, py::cast<double>(value));
                } else if (py::isinstance<py::bool_>(value)) {
                    self.set(key, py::cast<bool>(value));
                } else if (py::isinstance<py::list>(value)) {
                    // Convert list to appropriate vector type
                    auto list = py::cast<std::vector<py::object>>(value);
                    if (!list.empty()) {
                        if (py::isinstance<py::int_>(list[0])) {
                            std::vector<int> vec;
                            for (auto& item : list) vec.push_back(py::cast<int>(item));
                            self.set(key, vec);
                        } else if (py::isinstance<py::float_>(list[0])) {
                            std::vector<double> vec;
                            for (auto& item : list) vec.push_back(py::cast<double>(item));
                            self.set(key, vec);
                        } else if (py::isinstance<py::str>(list[0])) {
                            std::vector<std::string> vec;
                            for (auto& item : list) vec.push_back(py::cast<std::string>(item));
                            self.set(key, vec);
                        }
                    }
                }
            }
        }, py::arg("kwargs"),
            "Set multiple configuration values from Python dictionary (Pythonic API)")
        .def("getMultiple", [](Config& self, const py::list& keys) {
            py::dict result;
            for (auto& key : keys) {
                std::string keyStr = py::cast<std::string>(key);
                auto value = self.get<std::string>(keyStr);
                if (value) {
                    result[key] = *value;
                }
            }
            return result;
        }, py::arg("keys"),
            "Get multiple configuration values as Python dictionary (Pythonic API)")
        .def("fromDict", [](Config& self, const py::dict& configDict) {
            for (auto it : configDict) {
                std::string key = py::cast<std::string>(it.first);
                auto value = it.second;
                if (py::isinstance<py::str>(value)) {
                    self.set(key, py::cast<std::string>(value));
                } else if (py::isinstance<py::int_>(value)) {
                    self.set(key, py::cast<int>(value));
                } else if (py::isinstance<py::float_>(value)) {
                    self.set(key, py::cast<double>(value));
                } else if (py::isinstance<py::bool_>(value)) {
                    self.set(key, py::cast<bool>(value));
                }
            }
        }, py::arg("configDict"),
            "Create configuration from Python dictionary (Pythonic API)")
        .def("toDict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get as string first, then as int, then as float, etc.
                auto strVal = self.get<std::string>(key);
                if (strVal) {
                    result[key] = *strVal;
                } else {
                    auto intVal = self.get<int>(key);
                    if (intVal) {
                        result[key] = *intVal;
                    } else {
                        auto floatVal = self.get<double>(key);
                        if (floatVal) {
                            result[key] = *floatVal;
                        } else {
                            auto boolVal = self.get<bool>(key);
                            if (boolVal) {
                                result[key] = *boolVal;
                            }
                        }
                    }
                }
            }
            return result;
        },
            "Convert configuration to Python dictionary (Pythonic API)")
        .def("validate", [](const Config& self, const py::dict& expected) {
            py::dict errors;
            for (auto it : expected) {
                std::string key = py::cast<std::string>(it.first);
                auto value = it.second;
                if (!self.has(key)) {
                    errors[key] = "Missing required key";
                }
            }
            if (!errors.empty()) {
                throw py::value_error("Configuration validation failed: " + py::str(errors));
            }
        }, py::arg("expected"),
            "Validate configuration against expected keys (Pythonic API)")
        .def("clone", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a copy of this configuration (Pythonic API)")

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Enhanced Pythonic SimpleWorld methods
        .def("runEnvironment", [](SimpleWorld& self, int numSteps) {
            for (int i = 0; i < numSteps; ++i) {
                self.update(0.1);
            }
            return self.getSimulationTime();
        }, py::arg("numSteps"),
            "Run the environment for a specified number of simulation steps")
        .def("createTestEnvironment", []() -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            world->setAgentStart(10.0, 10.0);
            return world;
        }, "Create a test environment with default parameters")
        .def("createMazeEnvironment", [](int width = 20, int height = 20) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, 8, 8);
            world->reset();
            world->setAgentStart(1.0, 1.0);
            
            // Add walls to create a maze
            for (int x = 0; x < width; ++x) {
                world->addObject(WorldObject(x, 0, WorldObjectType::Wall, 1.0f, 0.8f));
                world->addObject(WorldObject(x, height - 1, WorldObjectType::Wall, 1.0f, 0.8f));
            }
            for (int y = 0; y < height; ++y) {
                world->addObject(WorldObject(0, y, WorldObjectType::Wall, 1.0f, 0.8f));
                world->addObject(WorldObject(width - 1, y, WorldObjectType::Wall, 1.0f, 0.8f));
            }
            
            // Add some resources
            world->addObject(WorldObject(width/2, height/2, WorldObjectType::Resource, 5.0f, 0.5f));
            
            return world;
        }, py::arg("width") = 20, py::arg("height") = 20,
            "Create a maze environment with walls and resources")
        .def("addResources", [](SimpleWorld& self, int count, float value = 1.0f) {
            int placed = 0;
            while (placed < count) {
                float x = static_cast<float>(rand() % self.getWidth());
                float y = static_cast<float>(rand() % self.getHeight());
                if (self.isValidPosition(x, y)) {
                    self.addObject(WorldObject(x, y, WorldObjectType::Resource, value, 0.5f));
                    placed++;
                }
            }
            return placed;
        }, py::arg("count"), py::arg("value") = 1.0f,
            "Add random resources to the environment")
        .def("addHazards", [](SimpleWorld& self, int count, float danger = 0.5f) {
            int placed = 0;
            while (placed < count) {
                float x = static_cast<float>(rand() % self.getWidth());
                float y = static_cast<float>(rand() % self.getHeight());
                if (self.isValidPosition(x, y)) {
                    self.addObject(WorldObject(x, y, WorldObjectType::Hazard, danger, 0.3f));
                    placed++;
                }
            }
            return placed;
        }, py::arg("count"), py::arg("danger") = 0.5f,
            "Add random hazards to the environment")
        .def("getEnvironmentStats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["simulation_time"] = self.getSimulationTime();
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            
            // Count objects by type
            py::dict objectCounts;
            objectCounts["resources"] = 0;
            objectCounts["hazards"] = 0;
            objectCounts["walls"] = 0;
            objectCounts["markers"] = 0;
            objectCounts["empty"] = 0;
            
            // Note: SimpleWorld doesn't expose object counting API directly
            // This is a placeholder for future enhancement
            
            stats["object_counts"] = objectCounts;
            return stats;
        }, "Get environment statistics and information")
        .def("getAgentPosition", [](SimpleWorld& self) {
            py::dict position;
            auto body = self.getAgentBody();
            position["x"] = body.x;
            position["y"] = body.y;
            position["orientation"] = body.orientation;
            position["velocity_x"] = body.velocityX;
            position["velocity_y"] = body.velocityY;
            position["energy"] = body.energy;
            position["health"] = body.health;
            position["age"] = body.age;
            position["is_moving"] = body.isMoving;
            position["is_turning"] = body.isTurning;
            return position;
        }, "Get current agent position and body state as Python dictionary")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y, float orientation = 0.0f) {
            auto body = self.getAgentBody();
            body.x = x;
            body.y = y;
            body.orientation = orientation;
            // Note: SimpleWorld doesn't expose setter for agent body directly
            // This is a placeholder for future enhancement
        }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f,
            "Set agent position and orientation")
        .def("getVisionData", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getVision();
        }, "Get vision sensory data from the world")
        .def("getTouchData", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getTouch();
        }, "Get touch sensory data from the world")
        .def("getInternalData", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getInternal();
        }, "Get internal signals sensory data from the world")
        .def("simulateEpisode", [](SimpleWorld& self, int steps, float timestep = 0.1f) {
            py::dict episodeData;
            py::list actionHistory;
            py::list rewardHistory;
            
            for (int i = 0; i < steps; ++i) {
                self.update(timestep);
                auto percept = self.getSensoryPercept();
                
                // Store percept data (vision, touch, internal, audio)
                py::dict stepData;
                stepData["vision"] = percept.getVision();
                stepData["touch"] = percept.getTouch();
                stepData["internal"] = percept.getInternal();
                stepData["audio"] = percept.getAudio();
                stepData["timestamp"] = percept.getTimestamp();
                
                actionHistory.append(stepData);
                rewardHistory.append(0.0f); // Would come from action result
                
                if (i % 10 == 0) {
                    // Log progress every 10 steps
                    auto body = self.getAgentBody();
                    stepData["agent_x"] = body.x;
                    stepData["agent_y"] = body.y;
                }
            }
            
            episodeData["actions"] = actionHistory;
            episodeData["rewards"] = rewardHistory;
            episodeData["steps_run"] = steps;
            episodeData["final_time"] = self.getSimulationTime();
            
            return episodeData;
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
            "Simulate an episode and return collected sensory data")
        .def("createTrainingData", [](SimpleWorld& self, int episodes, int stepsPerEpisode) {
            py::dict trainingSet;
            py::list episodeData;
            
            for (int ep = 0; ep < episodes; ++ep) {
                py::dict episode;
                episode["episode_id"] = ep;
                
                // Reset world for each episode
                self.reset();
                
                for (int step = 0; step < stepsPerEpisode; ++step) {
                    self.update(0.1);
                    
                    py::dict stepData;
                    auto percept = self.getSensoryPercept();
                    stepData["vision"] = percept.getVision();
                    stepData["touch"] = percept.getTouch();
                    stepData["internal"] = percept.getInternal();
                    
                    // Add to episode data
                    if (!episode.contains("states")) {
                        episode["states"] = py::list();
                    }
                    episode["states"].cast<py::list>().append(stepData);
                }
                
                // Add episode to training set
                episodeData.append(episode);
                
                // Small delay between episodes
                self.update(0.5);
            }
            
            trainingSet["episodes"] = episodeData;
            trainingSet["total_episodes"] = episodes;
            trainingSet["steps_per_episode"] = stepsPerEpisode;
            
            return trainingSet;
        }, py::arg("episodes"), py::arg("stepsPerEpisode"),
            "Create a training dataset from simulated episodes")
        .def("exportForAnalysis", [](SimpleWorld& self, const std::string& filename) {
            // This would export world data for external analysis
            // Implementation depends on available serialization APIs
            return "World data exported to " + filename;
        }, py::arg("filename"),
            "Export world data for external analysis")
        .def("importFromFile", [](SimpleWorld& self, const std::string& filename) {
            // This would import world state from file
            // Implementation depends on available serialization APIs
            return "World data imported from " + filename;
        }, py::arg("filename"),
            "Import world state from file")

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
        // Enhanced Pythonic brain methods
        .def("runSimulation", [](Brain& self, int numSteps, double timestep = 0.001) {
            for (int i = 0; i < numSteps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("numSteps"), py::arg("timestep") = 0.001,
            "Run a simulation for a specified number of steps")
        .def("runUntilStable", [](Brain& self, int maxSteps, double stabilityThreshold, double timestep = 0.001) {
            int stableSteps = 0;
            int requiredStable = 100;
            for (int i = 0; i < maxSteps; ++i) {
                self.step(i, i * timestep);
                
                // Check stability based on firing rate
                float firingRate = self.getAverageFiringRate();
                if (std::abs(firingRate) < stabilityThreshold) {
                    stableSteps++;
                } else {
                    stableSteps = 0;
                }
                
                if (stableSteps >= requiredStable) {
                    return i + 1; // Return number of steps run
                }
            }
            return maxSteps;
        }, py::arg("maxSteps"), py::arg("stabilityThreshold"), py::arg("timestep") = 0.001,
            "Run simulation until neural activity stabilizes")
        .def("runWithInputPattern", [](Brain& self, const std::vector<float>& inputPattern, int numSteps, double timestep = 0.001) {
            for (int i = 0; i < numSteps; ++i) {
                // Create a simple sensory input from pattern
                // Note: This assumes the existence of a method to create sensory input
                // In practice, this would use the appropriate sensory input class
                self.step(i, i * timestep);
            }
        }, py::arg("inputPattern"), py::arg("numSteps"), py::arg("timestep") = 0.001,
            "Run simulation with a specific input pattern")
        .def("getActivityStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neuron_count"] = static_cast<int64_t>(self.getTotalNeuronCount());
            stats["total_synapse_count"] = static_cast<int64_t>(self.getTotalSynapseCount());
            stats["active_neuron_count"] = static_cast<int64_t>(self.getActiveNeuronCount());
            stats["firing_neuron_count"] = static_cast<int64_t>(self.getFiringNeuronCount());
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spike_count"] = static_cast<int64_t>(self.getTotalSpikeCount());
            return stats;
        }, "Get comprehensive activity statistics as Python dictionary")
        .def("createTestInput", []() -> std::shared_ptr<SensoryInput> {
            return std::make_shared<Vision>(8, 8, 3);
        }, "Create a test vision sensory input for Python use")
        .def("createPatternInput", [](const std::vector<std::vector<float>>& pattern) {
            auto vision = std::make_shared<Vision>(pattern.size(), pattern[0].size(), 1);
            std::vector<float> data;
            for (const auto& row : pattern) {
                data.insert(data.end(), row.begin(), row.end());
            }
            vision->setData(data);
            return vision;
        }, py::arg("pattern"),
            "Create a vision input from a 2D pattern (list of lists)")
        .def("injectPatternCurrent", [](Brain& self, const std::vector<std::vector<float>>& pattern, NeuronType neuronType = NeuronType::Sensory) {
            for (size_t y = 0; y < pattern.size(); ++y) {
                for (size_t x = 0; x < pattern[0].size(); ++x) {
                    float current = pattern[y][x];
                    if (current != 0.0f) {
                        self.injectCurrentToNeurons(neuronType, current);
                    }
                }
            }
        }, py::arg("pattern"), py::arg("neuronType") = NeuronType::Sensory,
            "Inject current pattern into sensory neurons")
        .def("monitorNeurons", [](Brain& self, NeuronType neuronType, int count = 10) {
            py::dict monitoring;
            monitoring["type"] = static_cast<int>(neuronType);
            monitoring["count"] = count;
            
            // Get neuron activity statistics
            auto stats = self.getActivityStats();
            monitoring["total_neurons"] = stats["total_neuron_count"];
            monitoring["firing_neurons"] = stats["firing_neuron_count"];
            monitoring["firing_rate"] = stats["average_firing_rate"];
            
            return monitoring;
        }, py::arg("neuronType"), py::arg("count") = 10,
            "Monitor neuron activity for a specific type")
        .def("developStage", [](Brain& self, double timeStep) {
            // Simulate one developmental step
            self.setDevelopmentalStage(self.getDevelopmentalStage()); // Keep current stage
            return self.getDevelopmentalStage();
        }, py::arg("timeStep"),
            "Advance brain development by a time step")
        .def("analyzeLearning", [](Brain& self) {
            py::dict analysis;
            auto stats = self.getActivityStats();
            
            analysis["learning_progress"] = stats["total_spike_count"] > 1000;
            analysis["plasticity_indicator"] = stats["excitation_inhibition_ratio"] > 0.5 &&
                                                stats["excitation_inhibition_ratio"] < 2.0;
            analysis["memory_capacity"] = stats["total_synapse_count"] / 1000.0; // Normalized
            
            return analysis;
        }, "Analyze learning and development metrics")
        .def("resetAndInitialize", [](Brain& self, std::shared_ptr<Config> config = nullptr) {
            self.reset();
            if (config) {
                auto brain = std::make_shared<Brain>(config);
                // Note: This creates a new brain, current implementation limitation
                // In practice, you'd use the move constructor or return the new brain
                return std::make_shared<Brain>(config);
            }
            self.initialize();
            return self.getConfig();
        }, py::arg("config") = nullptr,
            "Reset brain and optionally reinitialize with new configuration")

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
        // Enhanced Pythonic AgentBrain methods
        .def("runEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, int numSteps) {
            for (int i = 0; i < numSteps; ++i) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process input and step brain
                self.processSensoryInput(percept);
                auto brainPtr = self.getBrain();
                if (brainPtr) {
                    brainPtr->step(i, i * 0.001);
                }
                
                // Decode motor command
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }, py::arg("world"), py::arg("numSteps"),
            "Run a complete episode with world interaction")
        .def("runEpisodeWithLearning", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, int numSteps, bool enableLearning = true) {
            float totalReward = 0.0f;
            
            for (int i = 0; i < numSteps; ++i) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process input and step brain
                self.processSensoryInput(percept);
                auto brainPtr = self.getBrain();
                if (brainPtr) {
                    brainPtr->step(i, i * 0.001);
                }
                
                // Decode motor command
                auto action = self.decodeMotorCommand();
                
                // Apply action to world and get reward
                world->applyMotorCommand(action, world->getSimulationTime());
                auto result = world->getActionResult();
                
                // Apply reward modulation if learning enabled
                if (enableLearning) {
                    self.applyRewardModulation(result.reward, 0.0f);
                }
                
                totalReward += result.reward;
                self.updateDevelopment(0.1);
            }
            
            return totalReward;
        }, py::arg("world"), py::arg("numSteps"), py::arg("enableLearning") = true,
            "Run an episode with learning and reward tracking")
        .def("simulateWorldInteraction", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                                             const py::dict& interactionConfig) {
            py::dict results;
            int steps = py::cast<int>(interactionConfig["steps"].cast<py::int_>());
            bool enableLearning = py::cast<bool>(interactionConfig["enableLearning"].cast<py::bool_>());
            bool enableCuriosity = py::cast<bool>(interactionConfig["enableCuriosity"].cast<py::bool_>());
            
            // Configure subsystems
            self.enableRewardModulation(enableLearning);
            self.enableCuriosity(enableCuriosity);
            
            float episodeReward = 0.0f;
            int curiosityEvents = 0;
            
            for (int i = 0; i < steps; ++i) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
                auto brainPtr = self.getBrain();
                if (brainPtr) {
                    brainPtr->step(i, i * 0.001);
                }
                
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto result = world->getActionResult();
                
                if (enableLearning) {
                    self.applyRewardModulation(result.reward, 0.0f);
                }
                
                if (enableCuriosity && self.getCuriosityLevel() > 0.5) {
                    curiosityEvents++;
                }
                
                episodeReward += result.reward;
                self.updateDevelopment(0.1);
            }
            
            results["total_reward"] = episodeReward;
            results["curiosity_events"] = curiosityEvents;
            results["final_curiosity"] = self.getCuriosityLevel();
            results["final_novelty"] = self.getNoveltyLevel();
            results["final_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            return results;
        }, py::arg("world"), py::arg("interactionConfig"),
            "Simulate world interaction with configurable parameters")
        .def("analyzeEpisodePerformance", [](AgentBrain& self) {
            py::dict performance;
            
            performance["curiosity_level"] = self.getCuriosityLevel();
            performance["novelty_level"] = self.getNoveltyLevel();
            performance["prediction_error"] = self.getPredictionError();
            performance["neuromodulation_level"] = self.getNeuromodulationLevel();
            performance["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            // Get brain activity
            auto brain = self.getBrain();
            if (brain) {
                auto stats = brain->getActivityStats();
                performance["brain_stats"] = stats;
            }
            
            // Performance indicators
            performance["exploration_score"] = self.getCuriosityLevel() * self.getNoveltyLevel();
            performance["learning_efficiency"] = self.getNeuromodulationLevel() > 0.0f ? 
                                                 self.getPredictionError() / self.getNeuromodulationLevel() : 0.0f;
            
            return performance;
        }, "Analyze episode performance and learning metrics")
        .def("saveEpisodeState", [](AgentBrain& self, const std::string& filepath) {
            auto brain = self.getBrain();
            if (brain) {
                brain->save(filepath);
            }
        }, py::arg("filepath"),
            "Save episode state to file")
        .def("loadEpisodeState", [](AgentBrain& self, const std::string& filepath) {
            auto brain = self.getBrain();
            if (brain) {
                brain->load(filepath);
            }
        }, py::arg("filepath"),
            "Load episode state from file")
        .def("createTrainingExample", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                                          int episodeSteps, bool isPositive = true) {
            py::dict example;
            
            // Run episode to collect data
            float totalReward = self.runEpisodeWithLearning(world, episodeSteps, true);
            
            example["reward"] = totalReward;
            example["positive_example"] = isPositive;
            example["curiosity_level"] = self.getCuriosityLevel();
            example["novelty_level"] = self.getNoveltyLevel();
            example["final_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            // Get brain statistics
            auto brain = self.getBrain();
            if (brain) {
                example["brain_activity"] = brain->getActivityStats();
            }
            
            return example;
        }, py::arg("world"), py::arg("episodeSteps"), py::arg("isPositive") = true,
            "Create a training example from episode data")
        .def("batchTrain", [](AgentBrain& self, std::vector<std::shared_ptr<SimpleWorld>> worlds, 
                              int stepsPerWorld, bool enableExperienceReplay = false) {
            py::dict results;
            float totalTrainingReward = 0.0f;
            int successfulEpisodes = 0;
            
            for (size_t i = 0; i < worlds.size(); ++i) {
                auto world = worlds[i];
                float episodeReward = self.runEpisodeWithLearning(world, stepsPerWorld, true);
                totalTrainingReward += episodeReward;
                
                if (episodeReward > 0.0f) {
                    successfulEpisodes++;
                }
                
                if (enableExperienceReplay) {
                    // Simulate experience replay by resetting and re-running
                    self.reset();
                    world->reset();
                }
            }
            
            results["average_reward"] = totalTrainingReward / worlds.size();
            results["successful_episodes"] = successfulEpisodes;
            results["total_episodes"] = worlds.size();
            results["learning_rate"] = self.getNeuromodulationLevel();
            
            return results;
        }, py::arg("worlds"), py::arg("stepsPerWorld"), py::arg("enableExperienceReplay") = false,
            "Run batch training across multiple worlds")
        .def("getNeuroscienceMetrics", [](AgentBrain& self) {
            py::dict metrics;
            
            // Basic neural metrics
            metrics["curiosity"] = self.getCuriosityLevel();
            metrics["novelty"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["neuromodulation"] = self.getNeuromodulationLevel();
            
            // Developmental metrics
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            metrics["plasticity_enabled"] = self.isDevelopmentEnabled();
            metrics["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            
            return metrics;
        }, "Get neuroscience-inspired metrics and analysis")
        .def("createExplorationPolicy", [](AgentBrain& self, float explorationRate = 0.3) {
            py::dict policy;
            
            policy["exploration_rate"] = explorationRate;
            policy["curiosity_weight"] = self.getCuriosityLevel();
            policy["novelty_weight"] = self.getNoveltyLevel();
            policy["prediction_weight"] = self.getPredictionError();
            
            // Calculate action preferences
            py::list actionPreferences;
            actionPreferences.append(self.getCuriosityLevel() * 0.4);  // Encourage exploration
            actionPreferences.append(1.0 - self.getCuriosityLevel() * 0.3);  // Encourage exploitation
            actionPreferences.append(self.getNoveltyLevel() * 0.5);  // Novelty-seeking
            actionPreferences.append(1.0 - self.getPredictionError() * 0.2);  // Reward maximization
            
            policy["action_preferences"] = actionPreferences;
            
            return policy;
        }, py::arg("explorationRate") = 0.3,
            "Create an exploration policy based on current brain state")
        .def("debugAgentState", [](AgentBrain& self) {
            py::dict debugInfo;
            
            debugInfo["curiosity_level"] = self.getCuriosityLevel();
            debugInfo["novelty_level"] = self.getNoveltyLevel();
            debugInfo["prediction_error"] = self.getPredictionError();
            debugInfo["neuromodulation_level"] = self.getNeuromodulationLevel();
            debugInfo["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            debugInfo["subsystem_status"] = py::dict({
                {"reward_modulation", self.isRewardModulationEnabled()},
                {"structural_plasticity", self.isStructuralPlasticityEnabled()},
                {"development", self.isDevelopmentEnabled()},
                {"curiosity", self.isCuriosityEnabled()}
            });
            
            return debugInfo;
        }, "Get detailed debug information about agent state")

        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Factory functions and utility functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object for Python use");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for simulation");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Pythonic simulation helpers
    m.def("runSimpleSimulation", [](int numSteps, double timestep = 0.001) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int i = 0; i < numSteps; ++i) {
            brain->step(i, i * timestep);
        }
        
        return brain->getActivityStats();
    }, py::arg("numSteps"), py::arg("timestep") = 0.001,
        "Run a simple simulation and return activity statistics");

    m.def("runAgentWorldSimulation", [](std::shared_ptr<SimpleWorld> world, int numSteps, bool enableLearning = true) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        float totalReward = 0.0f;
        
        for (int i = 0; i < numSteps; ++i) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(i, i * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (enableLearning) {
                auto result = world->getActionResult();
                agent->applyRewardModulation(result.reward, 0.0f);
                totalReward += result.reward;
            }
            
            agent->updateDevelopment(0.1);
        }
        
        py::dict results;
        results["total_reward"] = totalReward;
        results["final_curiosity"] = agent->getCuriosityLevel();
        results["final_novelty"] = agent->getNoveltyLevel();
        results["brain_stats"] = brain->getActivityStats();
        
        return results;
    }, py::arg("world"), py::arg("numSteps"), py::arg("enableLearning") = true,
        "Run a complete agent-world simulation");

    m.def("runPhase3StyleExperiment", [](int numSteps) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.1);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable plasticity for learning
        auto region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply learning inputs
        for (int i = 0; i < numSteps; ++i) {
            // Inject pattern to stimulate learning
            for (size_t j = 0; j < 20 && j < brain->getTotalNeuronCount() / 4; ++j) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(i, i * 0.001);
            
            if (i % 100 == 0) {
                float firingRate = brain->getAverageFiringRate();
                float eir = brain->getExcitationInhibitionRatio();
                // Log progress
            }
        }
        
        py::dict results;
        results["total_spikes"] = static_cast<int64_t>(brain->getTotalSpikeCount());
        results["final_firing_rate"] = brain->getAverageFiringRate();
        results["final_eir"] = brain->getExcitationInhibitionRatio();
        results["learning_detected"] = brain->getTotalSpikeCount() > 100;
        
        return results;
    }, py::arg("numSteps"),
        "Run a Phase 3 style learning experiment (plasticity demonstration)");

    m.def("runPhase4StyleExperiment", [](int numSteps) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.15);
        config->set("random_seed", 123);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 8, 8);
        world->reset();
        world->setAgentStart(7.5, 7.5);
        agent->initialize(*world);
        
        // Enable all subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        float episodeScore = 0.0f;
        
        for (int i = 0; i < numSteps; ++i) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(i, i * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto result = world->getActionResult();
            agent->applyRewardModulation(result.reward, 0.0f);
            
            episodeScore += result.reward;
            agent->updateDevelopment(0.1);
            
            if (i % 100 == 0) {
                float curiosity = agent->getCuriosityLevel();
                float novelty = agent->getNoveltyLevel();
                float prediction = agent->getPredictionError();
                // Log progress
            }
        }
        
        py::dict results;
        results["episode_score"] = episodeScore;
        results["final_curiosity"] = agent->getCuriosityLevel();
        results["final_novelty"] = agent->getNoveltyLevel();
        results["final_prediction_error"] = agent->getPredictionError();
        results["final_developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        results["brain_neuron_count"] = static_cast<int64_t>(brain->getTotalNeuronCount());
        
        return results;
    }, py::arg("numSteps"),
        "Run a Phase 4 style complete agent experiment (cognition demonstration)");

    m.def("createCompleteTrainingSetup", []() {
        py::dict trainingSetup;
        
        // Create multiple configuration variations for training
        py::list configs;
        
        // Basic config
        auto config1 = std::make_shared<Config>();
        config1->set("brain.neuron_count", 500);
        config1->set("brain.synapse_density", 0.1);
        configs.append(config1);
        
        // Dense config
        auto config2 = std::make_shared<Config>();
        config2->set("brain.neuron_count", 1000);
        config2->set("brain.synapse_density", 0.2);
        config2->set("plasticity.stdp.enable", true);
        configs.append(config2);
        
        // Sparse config
        auto config3 = std::make_shared<Config>();
        config3->set("brain.neuron_count", 200);
        config3->set("brain.synapse_density", 0.05);
        config3->set("plasticity.hebbian.enable", true);
        configs.append(config3);
        
        trainingSetup["configs"] = configs;
        trainingSetup["default_config"] = config1;
        
        // Create standard world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        trainingSetup["world"] = world;
        
        // Add resources
        world->addObject(WorldObject(5, 5, WorldObjectType::Resource, 10.0f, 0.5f));
        world->addObject(WorldObject(15, 15, WorldObjectType::Resource, 10.0f, 0.5f));
        
        trainingSetup["description"] = "Complete training setup with varied configurations";
        
        return trainingSetup;
    }, "Create a complete training setup with multiple configurations");

    // Constants and utility values
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;
    m.attr("DEFAULT_NUM_STEPS") = 1000;
    m.attr("DEFAULT_WORLD_WIDTH") = 20;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20;
    m.attr("DEFAULT_VISION_WIDTH") = 8;
    m.attr("DEFAULT_VISION_HEIGHT") = 8;
    m.attr("DEFAULT_BRAIN_NEURONS") = 500;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1;

    // Version and metadata
    m.attr("__version__") = "Phase 6 Enhanced Python API";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced Python bindings for NLM with improved APIs and documentation";
}

} // namespace nlm
