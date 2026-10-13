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
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/MemoryReplaySystem.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 1.0.0
        Author: NLM Development Team
        Description: Python bindings for Neural Learning Machine simulation framework
    )pbdoc";

    // Define Python-specific exception hierarchy
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "KeyError");
    py::register_exception<std::logic_error>(m, "NLMError");
    
    // Module-level attributes
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Python bindings for Neural Learning Machine simulation framework";

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
        
        // Python dictionary-like interface
        .def("__getitem__", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw py::key_error("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get configuration value by key (as string)")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string)")
        .def("__contains__", &Config::has, py::arg("key"), "Check if key exists")
        .def("__len__", &Config::getKeys,
             "Get number of configuration keys")
        .def("__iter__", [](const Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        }, py::return_value_policy::reference_internal,
             "Iterate over configuration keys")
        
        // Convenience methods
        .def("get_or", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get value or return default")
        
        // Type-specific getters
        .def("get_int", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value")
        .def("get_float", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get float value")
        .def("get_bool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value")
        
        // Get with optional return
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            return py::cast(value); // Return py::none if not found
        }, py::arg("key"), "Get configuration value (returns None if not found)")
        
        // Batch operations
        .def("update", [](Config& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = py::str(item.first);
                auto value = item.second;
                if (value.is(py::int_())) {
                    self.set(key, value.cast<int>());
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else {
                    self.set(key, value.cast<std::string>());
                }
            }
        }, py::arg("updates"), "Update multiple configuration values from dictionary")
        
        // Remove
        .def("pop", [](Config& self, const std::string& key, const std::string& defaultValue) {
            if (self.has(key)) {
                std::string value = self.get<std::string>(key);
                self.remove(key);
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get and remove key, return default if not found")
        
        // Python repr
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        
        // Dictionary conversion methods
        .def("toDict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (!vision.empty()) {
                result["vision"] = vision;
            }
            auto touch = self.getTouch();
            if (!touch.empty()) {
                result["touch"] = touch;
            }
            auto internal = self.getInternal();
            if (!internal.empty()) {
                result["internal"] = internal;
            }
            auto proprioception = self.getProprioception();
            if (!proprioception.empty()) {
                result["proprioception"] = proprioception;
            }
            auto audio = self.getAudio();
            if (!audio.empty()) {
                result["audio"] = audio;
            }
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert sensory percept to Python dictionary")
        .def("fromDict", [](SensoryPercept& self, const py::dict& data) {
            if (data.contains("vision")) {
                self.setVision(data["vision"].cast<std::vector<float>>());
            }
            if (data.contains("touch")) {
                self.setTouch(data["touch"].cast<std::vector<float>>());
            }
            if (data.contains("internal")) {
                self.setInternal(data["internal"].cast<std::vector<float>>());
            }
            if (data.contains("proprioception")) {
                self.setProprioception(data["proprioception"].cast<std::vector<float>>());
            }
            if (data.contains("audio")) {
                self.setAudio(data["audio"].cast<std::vector<float>>());
            }
            if (data.contains("timestamp")) {
                self.setTimestamp(data["timestamp"].cast<double>());
            }
            return self;
        }, py::arg("data"), "Update sensory percept from Python dictionary")
        
        // Batch processing methods
        .def("batchUpdate", [](SensoryPercept& self, const py::dict& updates) {
            // For each key-value pair, update the corresponding field
            if (updates.contains("vision")) {
                self.setVision(updates["vision"].cast<std::vector<float>>());
            }
            if (updates.contains("touch")) {
                self.setTouch(updates["touch"].cast<std::vector<float>>());
            }
            if (updates.contains("internal")) {
                self.setInternal(updates["internal"].cast<std::vector<float>>());
            }
            if (updates.contains("proprioception")) {
                self.setProprioception(updates["proprioception"].cast<std::vector<float>>());
            }
            if (updates.contains("audio")) {
                self.setAudio(updates["audio"].cast<std::vector<float>>());
            }
            if (updates.contains("timestamp")) {
                self.setTimestamp(updates["timestamp"].cast<double>());
            }
            return self;
        }, py::arg("updates"), "Batch update multiple sensory fields")
        
        // Convenience methods
        .def("getSensorData", [](const SensoryPercept& self, const std::string& sensorType) {
            if (sensorType == "vision") {
                return py::make_tuple("vision", self.getVision(), 
                                    static_cast<double>(self.getVisionWidth()),
                                    static_cast<double>(self.getVisionHeight()));
            } else if (sensorType == "touch") {
                return py::make_tuple("touch", self.getTouch(), py::none(), py::none());
            } else if (sensorType == "internal") {
                return py::make_tuple("internal", self.getInternal(), py::none(), py::none());
            } else if (sensorType == "proprioception") {
                return py::make_tuple("proprioception", self.getProprioception(), py::none(), py::none());
            } else if (sensorType == "audio") {
                return py::make_tuple("audio", self.getAudio(), py::none(), py::none());
            }
            throw py::value_error("Unknown sensor type: " + sensorType);
        }, py::arg("sensorType"), "Get sensor data as tuple (type, data, width, height)")
        
        // Scale vision data (common operation)
        .def("scaleVision", [](SensoryPercept& self, float factor) {
            auto& vision = const_cast<std::vector<float>&>(self.getVision());
            for (auto& v : vision) {
                v = std::max(0.0f, std::min(1.0f, v * factor));
            }
            return self;
        }, py::arg("factor"), "Scale vision data by factor (0-1)")
        
        // Clear all data
        .def("clear", [](SensoryPercept& self) {
            self.setVision({});
            self.setTouch({});
            self.setInternal({});
            self.setProprioception({});
            self.setAudio({});
            self.setTimestamp(0.0);
            return self;
        }, "Clear all sensory data")
        
        // Check if any data is present
        .def("isEmpty", [](const SensoryPercept& self) {
            return self.getVision().empty() && self.getTouch().empty() &&
                   self.getInternal().empty() && self.getProprioception().empty() &&
                   self.getAudio().empty();
        }, "Check if sensory percept contains any data")
        
        // Python repr
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision(" + std::to_string(percept.getVisionWidth()) + 
                   "x" + std::to_string(percept.getVisionHeight()) + "), " +
                   "touch(" + std::to_string(percept.getTouch().size()) + "), " +
                   "internal(" + std::to_string(percept.getInternal().size()) + "), " +
                   "proprioception(" + std::to_string(percept.getProprioception().size()) + "), " +
                   "audio(" + std::to_string(percept.getAudio().size()) + ")>";
        });

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
        
        // Pythonic convenience methods
        .def("getPosition", [](const SimpleWorld& self) {
            return py::make_tuple(self.getAgentBody().x, self.getAgentBody().y);
        }, "Get agent position as (x, y) tuple")
        .def("getVelocity", [](const SimpleWorld& self) {
            return py::make_tuple(self.getAgentBody().velocityX, self.getAgentBody().velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("getOrientation", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent orientation (returns AgentBody)")
        .def("getEnergy", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent energy (returns AgentBody)")
        .def("getHealth", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent health (returns AgentBody)")
        .def("stepWithTime", [](SimpleWorld& self, double timestep, double currentTime) {
            self.update(timestep);
            return self.getSensoryPercept();
        }, py::arg("timestep"), py::arg("currentTime"), "Update world and return percept")
        .def("createAgent", [](SimpleWorld& self, float x, float y, float orientation = 0.0f) {
            self.setAgentStart(x, y);
            return self;
        }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f, "Set up agent in world")
        .def("getBounds", [](const SimpleWorld& self) {
            return py::make_tuple(0, 0, self.getWidth(), self.getHeight());
        }, "Get world bounds as (minX, minY, maxX, maxY)")
        .def("isActive", [](const SimpleWorld& self) {
            return self.getAgentBody().health > 0.0f;
        }, "Check if agent is alive")
        .def("getState", [](const SimpleWorld& self) {
            py::dict state;
            auto body = self.getAgentBody();
            state["x"] = body.x;
            state["y"] = body.y;
            state["orientation"] = body.orientation;
            state["velocityX"] = body.velocityX;
            state["velocityY"] = body.velocityY;
            state["angularVelocity"] = body.angularVelocity;
            state["energy"] = body.energy;
            state["health"] = body.health;
            state["age"] = body.age;
            state["isMoving"] = body.isMoving;
            state["isTurning"] = body.isTurning;
            state["lastActionTime"] = body.lastActionTime;
            return state;
        }, "Get agent state as dictionary")
        .def("setState", [](SimpleWorld& self, const py::dict& state) {
            auto& body = const_cast<AgentBody&>(self.getAgentBody());
            if (state.contains("x")) body.x = state["x"].cast<float>();
            if (state.contains("y")) body.y = state["y"].cast<float>();
            if (state.contains("orientation")) body.orientation = state["orientation"].cast<float>();
            if (state.contains("velocityX")) body.velocityX = state["velocityX"].cast<float>();
            if (state.contains("velocityY")) body.velocityY = state["velocityY"].cast<float>();
            if (state.contains("angularVelocity")) body.angularVelocity = state["angularVelocity"].cast<float>();
            if (state.contains("energy")) body.energy = state["energy"].cast<float>();
            if (state.contains("health")) body.health = state["health"].cast<float>();
            if (state.contains("age")) body.age = state["age"].cast<float>();
            if (state.contains("isMoving")) body.isMoving = state["isMoving"].cast<bool>();
            if (state.contains("isTurning")) body.isTurning = state["isTurning"].cast<bool>();
            if (state.contains("lastActionTime")) body.lastActionTime = state["lastActionTime"].cast<float>();
            return self;
        }, py::arg("state"), "Set agent state from dictionary")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " agents: " +
                   std::to_string(world.getAgentBody().health > 0.0f ? 1 : 0) + ">";
        });

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
        
        // Pythonic convenience methods
        .def("stepWithTime", [](Brain& self, SimulationStep currentStep, Timestamp currentTime) {
            self.step(currentStep, currentTime);
            return self;
        }, py::arg("currentStep"), py::arg("currentTime"),
           "Perform a simulation step with timestamp (returns self for chaining)")
        .def("initializeWithConfig", [](Brain& self) {
            // Check if config has needed parameters first
            auto config = self.getConfig();
            if (config->has("brain") && config->has("world")) {
                return self.initialize();
            }
            throw py::value_error("Configuration missing 'brain' or 'world' sections");
        }, "Initialize brain with configuration (throws error if config invalid)")
        .def("saveWithTimestamp", [](Brain& self, const std::string& filepath) {
            auto timestamp = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(timestamp);
            std::string timestampedFilepath = filepath + "_" + std::to_string(time_t);
            return self.save(timestampedFilepath);
        }, py::arg("filepath"), "Save brain state with timestamp suffix")
        .def("getRegionNames", [](const Brain& self) {
            auto regionIds = self.getRegionIds();
            std::vector<std::string> names;
            for (const auto& id : regionIds) {
                names.push_back(std::to_string(id.value));
            }
            return names;
        }, "Get region names as strings")
        .def("getNeuronStats", [](const Brain& self) {
            py::dict stats;
            stats["total"] = self.getTotalNeuronCount();
            stats["active"] = self.getActiveNeuronCount();
            stats["firing"] = self.getFiringNeuronCount();
            stats["averageFiringRate"] = self.getAverageFiringRate();
            stats["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            stats["totalSpikeCount"] = self.getTotalSpikeCount();
            return stats;
        }, "Get comprehensive neuron statistics as dictionary")
        .def("getStats", [](const Brain& self) {
            py::dict stats;
            stats["neurons"] = self.getTotalNeuronCount();
            stats["synapses"] = self.getTotalSynapseCount();
            stats["regions"] = self.getRegionCount();
            auto neuronStats = py::dict();
            neuronStats["active"] = self.getActiveNeuronCount();
            neuronStats["firing"] = self.getFiringNeuronCount();
            neuronStats["averageFiringRate"] = self.getAverageFiringRate();
            neuronStats["totalSpikeCount"] = self.getTotalSpikeCount();
            neuronStats["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            stats["neuronStats"] = neuronStats;
            stats["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("injectCurrentBatch", [](Brain& self, const py::dict& currentMap) {
            for (auto item : currentMap) {
                std::string neuronIdStr = py::str(item.first);
                float current = item.second.cast<float>();
                
                try {
                    NeuronId neuronId(std::stoull(neuronIdStr));
                    self.injectCurrent(neuronId, current);
                } catch (const std::exception& e) {
                    throw py::value_error("Invalid neuron ID: " + neuronIdStr);
                }
            }
            return self;
        }, py::arg("currentMap"), "Inject currents to multiple neurons (dict: neuron_id -> current)")
        .def("injectCurrentToTypeBatch", [](Brain& self, const py::dict& typeMap) {
            for (auto item : typeMap) {
                std::string typeStr = py::str(item.first);
                float current = item.second.cast<float>();
                
                NeuronType type;
                if (typeStr == "Excitatory") type = NeuronType::Excitatory;
                else if (typeStr == "Inhibitory") type = NeuronType::Inhibitory;
                else if (typeStr == "Modulatory") type = NeuronType::Modulatory;
                else if (typeStr == "Sensory") type = NeuronType::Sensory;
                else if (typeStr == "Motor") type = NeuronType::Motor;
                else if (typeStr == "Internal") type = NeuronType::Internal;
                else {
                    throw py::value_error("Invalid neuron type: " + typeStr);
                }
                
                self.injectCurrentToNeurons(type, current);
            }
            return self;
        }, py::arg("typeMap"), "Inject currents to multiple neuron types (dict: type -> current)")
        .def("isInitialized", [](const Brain& self) {
            return self.getRegionCount() > 0;
        }, "Check if brain is initialized (has regions)")
        .def("clone", [](const Brain& self) {
            // This is a simplified clone - in real implementation might need proper deep copy
            return std::make_shared<Brain>(self.getConfig());
        }, "Create a copy of the brain (shallow)")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getRegionCount()) + " regions, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses>";
        });

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
        
        // Pythonic convenience methods
        .def("getState", [](const AgentBrain& self) {
            py::dict state;
            auto brain = self.getBrain();
            state["brain"] = brain;
            state["developmentalStage"] = self.getDevelopmentalStage();
            state["neuromodulationLevel"] = self.getNeuromodulationLevel();
            state["curiosityLevel"] = self.getCuriosityLevel();
            state["noveltyLevel"] = self.getNoveltyLevel();
            state["predictionError"] = self.getPredictionError();
            state["rewardModulationEnabled"] = self.isRewardModulationEnabled();
            state["structuralPlasticityEnabled"] = self.isStructuralPlasticityEnabled();
            state["developmentEnabled"] = self.isDevelopmentEnabled();
            state["curiosityEnabled"] = self.isCuriosityEnabled();
            return state;
        }, "Get agent brain state as dictionary")
        .def("setState", [](AgentBrain& self, const py::dict& state) {
            if (state.contains("rewardModulationEnabled")) {
                self.enableRewardModulation(state["rewardModulationEnabled"].cast<bool>());
            }
            if (state.contains("structuralPlasticityEnabled")) {
                self.enableStructuralPlasticity(state["structuralPlasticityEnabled"].cast<bool>());
            }
            if (state.contains("developmentEnabled")) {
                self.enableDevelopment(state["developmentEnabled"].cast<bool>());
            }
            if (state.contains("curiosityEnabled")) {
                self.enableCuriosity(state["curiosityEnabled"].cast<bool>());
            }
            return self;
        }, py::arg("state"), "Set agent brain state from dictionary")
        .def("getStatus", [](const AgentBrain& self) {
            py::dict status;
            status["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            status["neuromodulation"] = self.getNeuromodulationLevel();
            status["curiosity"] = self.getCuriosityLevel();
            status["novelty"] = self.getNoveltyLevel();
            status["predictionError"] = self.getPredictionError();
            return status;
        }, "Get formatted status string for agent brain")
        .def("updateWithTime", [](AgentBrain& self, double timestep) {
            self.updateDevelopment(timestep);
            return self;
        }, py::arg("timestep"), "Update agent brain with timestep (returns self)")
        .def("processSensoryBatch", [](AgentBrain& self, const py::list& percepts) {
            for (auto item : percepts) {
                auto percept = item.cast<std::shared_ptr<SensoryPercept>>();
                self.processSensoryInput(*percept);
            }
            return self;
        }, py::arg("percepts"), "Process multiple sensory percepts (list)")
        .def("getEnabledFeatures", [](const AgentBrain& self) {
            std::vector<std::string> features;
            if (self.isRewardModulationEnabled()) features.push_back("reward_modulation");
            if (self.isStructuralPlasticityEnabled()) features.push_back("structural_plasticity");
            if (self.isDevelopmentEnabled()) features.push_back("development");
            if (self.isCuriosityEnabled()) features.push_back("curiosity");
            return features;
        }, "Get list of enabled agent brain features")
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            return self;
        }, "Enable all agent brain features (returns self)")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
            return self;
        }, "Disable all agent brain features (returns self)")
        .def("isActive", [](const AgentBrain& self) {
            return self.getNeuromodulationLevel() > 0.0f &&
                   self.getCuriosityLevel() > 0.0f &&
                   self.getPredictionError() > 0.0f;
        }, "Check if agent brain is actively learning")
        .def("clone", [](const AgentBrain& self) {
            return std::make_shared<AgentBrain>(self.getBrain());
        }, "Create a copy of the agent brain (shallow)")
        .def("__repr__", [](const AgentBrain& agentBrain) {
            return "<AgentBrain: developmental_stage=" +
                   std::to_string(static_cast<int>(agentBrain.getDevelopmentalStage())) +
                   ", neuromodulation=" + std::to_string(agentBrain.getNeuromodulationLevel()) +
                   ", curiosity=" + std::to_string(agentBrain.getCuriosityLevel()) + ">";
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

    .def("isActive", [](const AgentBrain& self) {
        return self.getNeuromodulationLevel() > 0.0f &&
               self.getCuriosityLevel() > 0.0f &&
               self.getPredictionError() > 0.0f;
    }, "Check if agent brain is actively learning")
    .def("clone", [](const AgentBrain& self) {
        return std::make_shared<AgentBrain>(self.getBrain());
    }, "Create a copy of the agent brain (shallow)")
    .def("__repr__", [](const AgentBrain& agentBrain) {
        return "<AgentBrain: developmental_stage=" +
               std::to_string(static_cast<int>(agentBrain.getDevelopmentalStage())) +
               ", neuromodulation=" + std::to_string(agentBrain.getNeuromodulationLevel()) +
               ", curiosity=" + std::to_string(agentBrain.getCuriosityLevel()) + ">";
    });

    // MemoryReplaySystem Python bindings
    py::class_<MemoryReplaySystem>(m, "MemoryReplaySystem", 
        R"pbdoc(Enhanced memory replay system for learning consolidation)pbdoc")
        .def(py::init<>())
        .def("initialize", &MemoryReplaySystem::initialize,
            py::arg("episodicMemory"), py::arg("config"),
            "Initialize the replay system with episodic memory and configuration")
        .def("startReplay", &MemoryReplaySystem::startReplay,
            py::arg("episodeIds"), py::arg("mode") = ReplayMode::SEQUENTIAL,
            "Start replay of selected episodes with specified mode")
        .def("stopReplay", &MemoryReplaySystem::stopReplay,
            "Stop the replay process")
        .def("isReplaying", &MemoryReplaySystem::isReplaying,
            "Check if replay is currently active")
        .def("getNextEpisode", &MemoryReplaySystem::getNextEpisode,
            "Get the next episode to be replayed")
        .def("advanceToNextEpisode", &MemoryReplaySystem::advanceToNextEpisode,
            "Advance to the next episode in the replay sequence")
        .def("setReplayRate", &MemoryReplaySystem::setReplayRate,
            py::arg("rate"), "Set replay rate (episodes per second)")
        .def("getReplayRate", &MemoryReplaySystem::getReplayRate,
            "Get the current replay rate")
        .def("setReplayStrategy", &MemoryReplaySystem::setReplayStrategy,
            py::arg("strategy"), "Set the replay strategy for episode selection")
        .def("getReplayStrategy", &MemoryReplaySystem::getReplayStrategy,
            "Get the current replay strategy")
        .def("setReplayTimeWindow", &MemoryReplaySystem::setReplayTimeWindow,
            py::arg("startTime"), py::arg("endTime"),
            "Set the time window for replay selection")
        .def("setReplayRelevanceThreshold", &MemoryReplaySystem::setReplayRelevanceThreshold,
            py::arg("threshold"), "Set relevance threshold for episode selection")
        .def("selectEpisodesForReplay", &MemoryReplaySystem::selectEpisodesForReplay,
            py::arg("maxEpisodes") = 10,
            "Select episodes for replay based on strategy and thresholds")
        .def("getReplayStats", &MemoryReplaySystem::getReplayStats,
            "Get current replay statistics")
        .def("enableReplay", &MemoryReplaySystem::enableReplay,
            py::arg("enable"), "Enable or disable replay functionality")
        .def("isReplayEnabled", &MemoryReplaySystem::isReplayEnabled,
            "Check if replay is enabled")
        .def("update", &MemoryReplaySystem::update,
            py::arg("currentTime"), "Update replay system (called each simulation step)")
        .def("getReplayProgress", &MemoryReplaySystem::getReplayProgress,
            "Get replay progress percentage (0.0 to 1.0)")
        .def("getCurrentEpisodeIndex", &MemoryReplaySystem::getCurrentEpisodeIndex,
            "Get current episode index in replay sequence")
        .def("getTotalEpisodesInReplay", &MemoryReplaySystem::getTotalEpisodesInReplay,
            "Get total number of episodes in current replay")
        .def("clear", &MemoryReplaySystem::clear,
            "Clear replay state and stop replay")
        .def("__repr__", [](const MemoryReplaySystem& replaySystem) {
            auto stats = replaySystem.getReplayStats();
            std::stringstream ss;
            ss << "<MemoryReplaySystem: " << stats.totalReplayed << "/" << stats.totalEpisodes << " episodes, "
               << stats.currentIndex << "/" << stats.totalEpisodes << " current, "
               << "progress: " << std::fixed << std::setprecision(1) << (replaySystem.getReplayProgress() * 100) << "%>";
            return ss.str();
        });

    // VisualizationInterface Python bindings
    py::class_<VisualizationInterface>(m, "VisualizationInterface",
        R"pbdoc(Enhanced visualization interface for debugging and analysis)pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize,
            "Initialize visualization system")
        .def("update", &VisualizationInterface::update,
            "Update visualization data")
        .def("render", &VisualizationInterface::render,
            "Render current state")
        .def("close", &VisualizationInterface::close,
            "Close visualization")
        .def("isActive", &VisualizationInterface::isActive,
            "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate,
            py::arg("hz"), "Set update rate (Hz)")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate,
            "Get the current update rate")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork,
            py::arg("brain"), "Visualize neural network structure")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity,
            py::arg("brain"), "Visualize neural activity patterns")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes,
            py::arg("spikes"), "Create spike raster plot")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights,
            py::arg("brain"), "Visualize synaptic weight matrices")
        .def("visualizeMemoryReplay", &VisualizationInterface::visualizeMemoryReplay,
            py::arg("replaySystem"), "Visualize memory replay process")
        .def("visualizeEpisodicMemory", &VisualizationInterface::visualizeEpisodicMemory,
            py::arg("episodicMemory"), "Visualize episodic memory structure")
        .def("visualizeReplayStatistics", &VisualizationInterface::visualizeReplayStatistics,
            py::arg("stats"), "Visualize replay statistics")
        .def("visualizeBrainStatesForReplay", &VisualizationInterface::visualizeBrainStatesForReplay,
            py::arg("brain"), "Visualize brain states optimized for replay")
        .def("visualizeReplayTimeline", &VisualizationInterface::visualizeReplayTimeline,
            py::arg("replaySystem"), "Visualize replay timeline")
        .def("visualizeReplayQuality", &VisualizationInterface::visualizeReplayQuality,
            py::arg("stats"), "Visualize replay quality metrics")
        .def("visualizeMemoryConsolidation", &VisualizationInterface::visualizeMemoryConsolidation,
            py::arg("episodicMemory"), "Visualize memory consolidation process")
        .def("visualizeMemoryRetrieval", &VisualizationInterface::visualizeMemoryRetrieval,
            py::arg("episodicMemory"), "Visualize memory retrieval patterns")
        .def("setReplaySystem", &VisualizationInterface::setReplaySystem,
            py::arg("replaySystem"), "Set the replay system for visualization")
        .def("setEpisodicMemory", &VisualizationInterface::setEpisodicMemory,
            py::arg("episodicMemory"), "Set the episodic memory for visualization")
        .def("enableReplayVisualization", &VisualizationInterface::enableReplayVisualization,
            py::arg("enable"), "Enable or disable replay visualization features")
        .def("isReplayVisualizationEnabled", &VisualizationInterface::isReplayVisualizationEnabled,
            "Check if replay visualization is enabled")
        .def("__repr__", [](const VisualizationInterface& viz) {
            return "<VisualizationInterface: " + std::to_string(viz.getUpdateRate()) + " Hz, " +
                   std::string(viz.isActive() ? "active" : "inactive") + ">";
        });

    // ReplayMode enum bindings
    py::enum_<ReplayMode>(m, "ReplayMode", R"pbdoc(Replay mode enumeration)pbdoc")
        .value("SEQUENTIAL", ReplayMode::SEQUENTIAL)
        .value("RANDOM", ReplayMode::RANDOM)
        .value("PRIORITY", ReplayMode::PRIORITY)
        .value("LOOPING", ReplayMode::LOOPING)
        .export_values();

    // ReplayStrategy enum bindings
    py::enum_<ReplayStrategy>(m, "ReplayStrategy", R"pbdoc(Replay strategy enumeration)pbdoc")
        .value("RELEVANCE_BASED", ReplayStrategy::RELEVANCE_BASED)
        .value("TIME_BASED", ReplayStrategy::TIME_BASED)
        .value("RANDOM", ReplayStrategy::RANDOM)
        .value("QUALITY_BASED", ReplayStrategy::QUALITY_BASED)
        .value("CONSOLIDATION", ReplayStrategy::CONSOLIDATION)
        .export_values();

    // ReplayStats struct bindings
    py::class_<ReplayStats>(m, "ReplayStats", R"pbdoc(Statistics for replay operations)pbdoc")
        .def(py::init<>())
        .def_readwrite("totalReplayed", &ReplayStats::totalReplayed)
        .def_readwrite("successfulReplays", &ReplayStats::successfulReplays)
        .def_readwrite("averageQuality", &ReplayStats::averageQuality)
        .def_readwrite("currentIndex", &ReplayStats::currentIndex)
        .def_readwrite("totalEpisodes", &ReplayStats::totalEpisodes)
        .def("getSuccessRate", [](const ReplayStats& stats) {
            return stats.totalEpisodes > 0 ? 
                static_cast<double>(stats.successfulReplays) / stats.totalEpisodes : 0.0;
        }, "Get success rate (0.0 to 1.0)")
        .def("__repr__", [](const ReplayStats& stats) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "<ReplayStats: " << stats.totalReplayed << "/" << stats.totalEpisodes << " episodes, "
               << stats.successfulReplays << " successful, "
               << stats.averageQuality << " avg quality, "
               << (stats.totalEpisodes > 0 ? (stats.successfulReplays * 100 / stats.totalEpisodes) : 0) << "% success rate>";
            return ss.str();
        });

    // Factory functions for new components
    m.def("createMemoryReplaySystem", []() -> std::shared_ptr<MemoryReplaySystem> {
        return std::make_shared<MemoryReplaySystem>();
    }, "Create a new memory replay system");

    m.def("createVisualizationInterface", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create a new visualization interface");

    // Module-level attributes for new components
    m.attr("ReplayMode") = py::module_local().attr("ReplayMode");
    m.attr("ReplayStrategy") = py::module_local().attr("ReplayStrategy");

}

} // namespace nlm

