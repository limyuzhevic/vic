#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <iostream>

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

// Helper functions for Python bindings
namespace py_utils {
    template<typename T>
    T val_or(const py::object& obj, const T& default_value) {
        if (obj.is_none()) return default_value;
        return obj.cast<T>();
    }
    
    template<typename Container>
    Container validate_not_empty(const Container& container, const std::string& operation_name) {
        if (container.empty()) {
            throw std::invalid_argument("Cannot " + operation_name + " with empty container");
        }
        return container;
    }
    
    void validate_simulation_state(bool condition, const std::string& error_message) {
        if (!condition) {
            throw std::runtime_error(error_message);
        }
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - High-performance neural simulation with Python interface
        - Advanced brain configuration and management
        - Agent-world interaction utilities
        - Comprehensive sensory-motor integration
        - Built-in learning mechanisms (STDP, Hebbian, reward modulation)
        - Developmental system for brain maturation
        - Curiosity-driven exploration
        - Checkpoint/save/load capabilities
        - Extensive statistics and monitoring
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // ===== BASIC IDENTIFIER TYPES =====
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
        .def("__ne__", &PopulationId::operator!=);

    // ===== ENUMERATIONS =====
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

    // ===== CONFIGURATION CLASS =====
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
        .def("getAny", &Config::getAny, py::arg("key"),
             "Get configuration value as any type (returns std::optional)")
        .def("set", [](Config& self, const std::string& key, const std::string& value, 
                   const nlm::ConfigSource& source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime,
           "Set configuration value as string")
        .def("getString", &Config::get<std::string>, py::arg("key"),
             "Get configuration value as string")
        .def("getInt", &Config::get<int>, py::arg("key"),
             "Get configuration value as int")
        .def("getFloat", &Config::get<double>, py::arg("key"),
             "Get configuration value as float")
        .def("getBool", &Config::get<bool>, py::arg("key"),
             "Get configuration value as bool")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with default")
        .def("getOrInt", [](const Config& self, const std::string& key, int default_value) {
            return self.getOr<int>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as int with default")
        .def("getOrFloat", [](const Config& self, const std::string& key, double default_value) {
            return self.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as float with default")
        .def("getOrBool", [](const Config& self, const std::string& key, bool default_value) {
            return self.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as bool with default");

    // ===== SENSORY INPUT CLASSES =====
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

    // ===== ACTION CLASS =====
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
        .def("addParameter", &Action::addParameter, py::arg("param"))
        .def("clearParameters", &Action::clearParameters)
        .def("getParameterCount", &Action::getParameterCount);

    // ===== WORLD OBJECT CLASS =====
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
        .def("isActive", &WorldObject::active); // Alias for easier access

    // ===== AGENT BODY CLASS =====
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
        .def("setPosition", &AgentBody::x, py::arg("x"), py::arg("y"),
             "Set agent position (requires both x and y)")
        .def("getDistanceTo", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"),
           "Calculate distance to another agent body");

    // ===== ACTION RESULT CLASS =====
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& data) {
            self.reward = data["reward"].cast<float>();
            self.success = data["success"].cast<bool>();
            self.message = data["message"].cast<std::string>();
            return self;
        }, py::arg("data"), "Create action result from dictionary");

    // ===== SENSORY PERCEPT CLASS =====
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
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict percept;
            auto vision = self.getVision();
            if (vision) {
                percept["vision"] = py::dict({
                    {"width", vision->getWidth()},
                    {"height", vision->getHeight()},
                    {"channels", vision->getChannels()}
                });
            }
            auto touch = self.getTouch();
            if (touch) {
                percept["touch"] = *touch;
            }
            auto internal = self.getInternal();
            if (internal) {
                percept["internal"] = *internal;
            }
            auto proprioception = self.getProprioception();
            if (proprioception) {
                percept["proprioception"] = *proprioception;
            }
            auto audio = self.getAudio();
            if (audio) {
                percept["audio"] = py::dict({
                    {"sample_rate", audio->getSampleRate()},
                    {"samples", audio->getNumSamples()}
                });
            }
            percept["timestamp"] = self.getTimestamp();
            return percept;
        }, "Convert sensory percept to dictionary")
        .def("from_dict", [](SensoryPercept& self, const py::dict& data) {
            if (data.contains("vision")) {
                auto vision_data = data["vision"].cast<py::dict>();
                auto vision = std::make_shared<Vision>();
                vision->setData(std::vector<float>()); // Empty data for now
                self.setVision(vision);
            }
            if (data.contains("touch")) {
                auto touch_data = data["touch"].cast<std::vector<float>>();
                self.setTouch(touch_data);
            }
            if (data.contains("internal")) {
                auto internal_data = data["internal"].cast<std::vector<float>>();
                self.setInternal(internal_data);
            }
            if (data.contains("proprioception")) {
                auto proprio_data = data["proprioception"].cast<std::vector<float>>();
                self.setProprioception(proprio_data);
            }
            if (data.contains("audio")) {
                auto audio_data = data["audio"].cast<py::dict>();
                auto audio = std::make_shared<Audio>();
                audio->setSampleRate(audio_data["sample_rate"].cast<size_t>());
                audio->setData(std::vector<float>()); // Empty data for now
                self.setAudio(audio);
            }
            if (data.contains("timestamp")) {
                self.setTimestamp(data["timestamp"].cast<float>());
            }
            return self;
        }, py::arg("data"), "Create sensory percept from dictionary")
        .def("get_signal", [](const SensoryPercept& self, const std::string& signal_name) {
            if (signal_name == "vision") {
                auto vision = self.getVision();
                return vision ? vision->getData() : std::vector<float>();
            } else if (signal_name == "touch") {
                auto touch = self.getTouch();
                return touch ? *touch : std::vector<float>();
            } else if (signal_name == "internal") {
                auto internal = self.getInternal();
                return internal ? *internal : std::vector<float>();
            } else if (signal_name == "proprioception") {
                auto proprio = self.getProprioception();
                return proprio ? *proprio : std::vector<float>();
            } else if (signal_name == "audio") {
                auto audio = self.getAudio();
                return audio ? audio->getData() : std::vector<float>();
            }
            return std::vector<float>();
        }, py::arg("signal_name"), "Get specific signal data from percept");

    // ===== SIMPLE WORLD CLASS =====
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
        .def("getAgentPosition", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return py::make_tuple(body->x, body->y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            return self.getAgentBody()->orientation;
        }, "Get agent orientation in radians")
        .def("is_agent_at", [](const SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            float dx = body->x - x;
            float dy = body->y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < 0.5f; // Within agent's radius
        }, py::arg("x"), py::arg("y"), "Check if agent is at specific position");

    // ===== BRAIN CLASS =====
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
        .def("getFiringNeurons", [](const Brain& self) {
            // This would require access to internal neuron data
            // For now, just return a placeholder
            return std::vector<uint64_t>{};
        }, "Get list of currently firing neuron IDs")
        .def("getNeuronsByType", &Brain::getTotalNeuronCount,
             "Get neuron count by type (placeholder)")
        .def("get_region_stats", [](const Brain& self, RegionId region_id) {
            py::dict stats;
            auto region = self.getRegion(region_id);
            if (region) {
                stats["id"] = region_id.value;
                stats["neuron_count"] = region->getNeuronCount();
                stats["synapse_count"] = region->getSynapseCount();
            }
            return stats;
        }, py::arg("region_id"), "Get statistics for a specific region");

    // ===== AGENT BRAIN CLASS =====
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
        .def("set_neuromodulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted") = 0.0f,
             "Set neuromodulation level with reward signal")
        .def("update_all_subsystems", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update all neuromodulation subsystems");

    // ===== FACTORY FUNCTIONS =====
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

    // ===== CONSTANTS =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===== ADVANCED UTILITY FUNCTIONS =====

    // Simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                              std::shared_ptr<AgentBrain> agent, int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation loop for the specified number of steps");

    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent, int num_steps,
                                     bool enable_rewards = false, bool enable_development = false,
                                     bool enable_curiosity = false) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (enable_rewards) {
                auto internal = percept->getInternal();
                float reward = 0.0f;
                if (internal && !internal->empty()) {
                    reward = (*internal)[0];
                }
                agent->applyRewardModulation(reward, 0.0);
            }
            
            if (enable_development) {
                agent->updateDevelopment(0.1);
            }
            
            if (enable_curiosity) {
                double curiosity = agent->getCuriosityLevel();
                double novelty = agent->getNoveltyLevel();
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       py::arg("enable_rewards") = false, py::arg("enable_development") = false,
       py::arg("enable_curiosity") = false,
       "Run an agent simulation with optional neuromodulation subsystems");

    // Configuration utilities
    m.def("create_preset_config", [](const std::string& preset_name) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (preset_name == "minimal") {
            config->set("brain.neuron_count", 100, nlm::ConfigSource::Runtime);
            config->set("brain.synapse_density", 0.05, nlm::ConfigSource::Runtime);
        } else if (preset_name == "standard") {
            config->set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
            config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Runtime);
        } else if (preset_name == "advanced") {
            config->set("brain.neuron_count", 5000, nlm::ConfigSource::Runtime);
            config->set("brain.synapse_density", 0.15, nlm::ConfigSource::Runtime);
            config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, nlm::ConfigSource::Runtime);
            config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Runtime);
        } else if (preset_name == "developmental") {
            config->set("brain.neuron_count", 2000, nlm::ConfigSource::Runtime);
            config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
            config->set("development.stage_transition_rate", 0.01, nlm::ConfigSource::Runtime);
            config->set("development.synaptogenesis_rate", 0.1, nlm::ConfigSource::Runtime);
        } else {
            throw std::invalid_argument("Unknown preset config: " + preset_name);
        }
        
        return config;
    }, py::arg("preset_name"),
       "Create a configuration with predefined settings");

    // Performance monitoring utilities
    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["region_count"] = brain->getRegionCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        stats["config_keys"] = brain->getConfig()->getKeys();
        return stats;
    }, py::arg("brain"), "Get comprehensive statistics about the brain");

    m.def("get_agent_status", [](std::shared_ptr<AgentBrain> agent) {
        py::dict status;
        status["curiosity_level"] = agent->getCuriosityLevel();
        status["novelty_level"] = agent->getNoveltyLevel();
        status["prediction_error"] = agent->getPredictionError();
        status["neuromodulation_level"] = agent->getNeuromodulationLevel();
        status["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        status["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
        status["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
        status["development_enabled"] = agent->isDevelopmentEnabled();
        status["curiosity_enabled"] = agent->isCuriosityEnabled();
        return status;
    }, py::arg("agent"), "Get current status of the agent");

    // Simulation utilities and utilities
    m.def("create_simulation_step", [](int step, float time = 0.0f) {
        nlm::SimulationStep step_obj;
        step_obj.step = step;
        step_obj.time = time;
        return step_obj;
    }, py::arg("step"), py::arg("time") = 0.0f,
       "Create a simulation step object");

    m.def("create_timestamp", [](float time) {
        return nlm::Timestamp(time);
    }, py::arg("time"), "Create a timestamp object");

    // Advanced brain manipulation utilities
    m.def("inject_burst_current", [](std::shared_ptr<Brain> brain, nlm::NeuronType neuron_type, 
                                    float current, int num_neurons) {
        for (int i = 0; i < num_neurons; ++i) {
            brain->injectCurrent(nlm::NeuronId(i), current);
        }
    }, py::arg("brain"), py::arg("neuron_type"), py::arg("current"), py::arg("num_neurons"),
       "Inject burst current to multiple neurons of a given type");

    m.def("reset_brain_with_config", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        brain->getConfig()->clear();
        for (const auto& key : config->getKeys()) {
            auto value_any = config->getAny(key);
            if (value_any.has_value()) {
                brain->getConfig()->set(key, value_any.value(), nlm::ConfigSource::Runtime);
            }
        }
        brain->reset();
        brain->initialize();
    }, py::arg("brain"), py::arg("config"),
       "Reset brain with new configuration");

    m.def("copy_brain_state", [](std::shared_ptr<Brain> source_brain) {
        auto config = std::make_shared<Config>();
        for (const auto& key : source_brain->getConfig()->getKeys()) {
            auto value_any = source_brain->getConfig()->getAny(key);
            if (value_any.has_value()) {
                config->set(key, value_any.value(), nlm::ConfigSource::Runtime);
            }
        }
        return config;
    }, py::arg("source_brain"), "Create a copy of brain's configuration");

    // Debug and monitoring utilities
    m.def("log_brain_state", [](std::shared_ptr<Brain> brain, const std::string& tag = "") {
        std::string timestamp_str = "[" + std::to_string(brain->getConfig()->get<int>("simulation.time", 0)) + "]";
        std::string tag_str = !tag.empty() ? " [" + tag + "]" : "";
        brain->logStatus();
        std::cout << timestamp_str << tag_str << " Brain state: " << brain->getTotalNeuronCount() 
                  << " neurons, " << brain->getFiringNeuronCount() << " firing" << std::endl;
    }, py::arg("brain"), py::arg("tag") = "",
       "Log brain state with optional tag");

    m.def("monitor_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent, int interval_steps) {
        int step_count = 0;
        return py::cpp_function([=](int step) {
            if (++step_count % interval_steps == 0) {
                auto stats = py::cast<py::dict>(py::module::import("pynlm").attr("get_brain_stats")(brain));
                auto status = py::cast<py::dict>(py::module::import("pynlm").attr("get_agent_status")(agent));
                std::cout << "Step " << step << ": Neurons=" << stats["neuron_count"].cast<int>() 
                          << ", Firing=" << stats["firing_neurons"].cast<int>()
                          << ", Curiosity=" << status["curiosity_level"].cast<double>() << std::endl;
            }
        });
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("interval_steps"),
       "Create a simulation monitor that logs statistics at intervals");

    // Version and information utilities
    m.def("get_version", []() {
        return "NLM Python Bindings v0.1.0";
    }, "Get version information");

    m.def("get_build_info", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["build_type"] = "Release";
        info["compiler"] = "C++20";
        info["python_support"] = "3.8+";
        info["features"] = std::vector<std::string>{
            "brain_simulation", "agent_world_interaction", "neuromodulation",
            "plasticity", "development", "curiosity", "novelty_detection",
            "prediction_systems", "memory_systems", "concept_formation",
            "planning", "attention", "social_learning", "continual_learning"
        };
        return info;
    }, "Get build and feature information");

    m.def("validate_python_interface", []() {
        py::dict results;
        auto config = std::make_shared<Config>();
        results["config_creation"] = true;
        results["config_summary"] = config->summary();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        results["brain_creation"] = true;
        
        auto stats = py::cast<py::dict>(py::module::import("pynlm").attr("get_brain_stats")(brain));
        results["brain_stats"] = stats;
        
        return results;
    }, "Validate Python interface functionality");
}

} // namespace nlm