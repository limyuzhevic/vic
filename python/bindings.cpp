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
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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
        .def(py::init<std::string>(), py::arg("filepath"),
             "Load configuration from a file in constructor")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (alias: from_file)")
        .def("from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (Pythonic alias)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
    // Create config from dict (Pythonic wrapper for Config)
    .def("loadFromDict", [](Config& self, const py::dict& d) {
        for (auto item : d) {
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
    }, py::arg("dict"),
         "Load configuration from Python dictionary")
    .def("from_dict", [](Config& self, const py::dict& d) {
        self.loadFromDict(d);
    }, py::arg("dict"),
         "Pythonic alias: from_dict")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file (alias: to_file)")
        .def("to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file (Pythonic alias)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeyCount", &Config::getKeys,
             "Get the number of configuration keys (alias: get_key_count)")
        .def("get_key_count", &Config::getKeys,
             "Get the number of configuration keys (Pythonic alias)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys (alias: keys)")
        .def("keys", &Config::getKeys,
             "Get all configuration keys (Pythonic alias)")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get different types
                auto intVal = self.get<int>(key);
                if (intVal.has_value()) {
                    result[key.c_str()] = intVal.value();
                } else {
                    auto doubleVal = self.get<double>(key);
                    if (doubleVal.has_value()) {
                        result[key.c_str()] = doubleVal.value();
                    } else {
                        auto stringVal = self.get<std::string>(key);
                        if (stringVal.has_value()) {
                            result[key.c_str()] = stringVal.value();
                        } else {
                            auto boolVal = self.get<bool>(key);
                            if (boolVal.has_value()) {
                                result[key.c_str()] = boolVal.value();
                            }
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Configure world dimensions (alias: setup)")
        .def("setup", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Setup world (Pythonic alias for configure)")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state (alias: clear)")
        .def("clear", &SimpleWorld::reset,
             "Clear world (Pythonic alias for reset)")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position (alias: set_start_position)")
        .def("set_start_position", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position (Pythonic alias)")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (alias: step)")
        .def("step", &SimpleWorld::update, py::arg("timestep"),
             "Step the simulation (Pythonic alias for update)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent (alias: apply_action)")
        .def("apply_action", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply action (Pythonic alias)")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept (alias: get_percept, get_sensory)")
        .def("get_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get perception (Pythonic alias)")
        .def("get_sensory", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Pythonic alias: get_sensory_percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state (alias: get_body, get_agent)")
        .def("get_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get body state (Pythonic alias)")
        .def("get_agent", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Pythonic alias: get_agent_body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world (alias: add_world_object)")
        .def("add_world_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object (Pythonic alias)")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (alias: remove_at_position)")
        .def("remove_at_position", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (Pythonic alias)")
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.4f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f,
             "Add a resource object to the world (alias: create_resource)")
        .def("create_resource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.4f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f,
             "Create resource (Pythonic alias)")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 15.0f,
             "Add a hazard object to the world (alias: create_hazard)")
        .def("create_hazard", [](SimpleWorld& self, float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 15.0f,
             "Create hazard (Pythonic alias)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall) (alias: is_free_position)")
        .def("is_free_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is free (Pythonic alias)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (alias: get_at_position)")
        .def("get_at_position", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (Pythonic alias)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width (alias: width)")
        .def("width", &SimpleWorld::getWidth,
             "Get world width (Pythonic alias)")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height (alias: height)")
        .def("height", &SimpleWorld::getHeight,
             "Get world height (Pythonic alias)")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy (alias: get_max_energy)")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             "Get max energy (Pythonic alias)")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy (alias: set_max_energy)")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy (Pythonic alias)")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate (alias: get_energy_decay)")
        .def("get_energy_decay", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate (Pythonic alias)")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate (alias: set_energy_decay)")
        .def("set_energy_decay", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate (Pythonic alias)")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time (alias: sim_time, get_time)")
        .def("sim_time", &SimpleWorld::getSimulationTime,
             "Get simulation time (Pythonic alias)")
        .def("get_time", &SimpleWorld::getSimulationTime,
             "Get time (Pythonic alias)")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed (alias: seed, set_random)")
        .def("seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set seed (Pythonic alias)")
        .def("set_random", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed (Pythonic alias)")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed (alias: get_seed)")
        .def("get_seed", &SimpleWorld::getRandomSeed,
             "Get seed (Pythonic alias)")
        .def("getWorldObjects", [](SimpleWorld& self) {
            return self.objects_;
        }, "Get all world objects (alias: get_objects)")
        .def("get_objects", [](SimpleWorld& self) {
            return self.objects_;
        }, "Get objects (Pythonic alias)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state (alias: get_body)")
        .def("get_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Pythonic alias: get_agent_body")
        .def("create_agent", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
            return self.getAgentBody();
        }, py::arg("x"), py::arg("y"),
             "Create and position agent (alias: spawn_agent)")
        .def("spawn_agent", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
            return self.getAgentBody();
        }, py::arg("x"), py::arg("y"),
             "Spawn agent (Pythonic alias)")
        .def("getWorldStats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["vision_width"] = self.visionWidth_;
            stats["vision_height"] = self.visionHeight_;
            stats["agent_x"] = self.agent_.x;
            stats["agent_y"] = self.agent_.y;
            stats["agent_energy"] = self.agent_.energy;
            stats["agent_health"] = self.agent_.health;
            stats["agent_orientation"] = self.agent_.orientation;
            stats["object_count"] = static_cast<int>(self.objects_.size());
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            stats["simulation_time"] = self.getSimulationTime();
            stats["random_seed"] = self.getRandomSeed();
            return stats;
        }, "Get world statistics as Python dictionary (alias: world_stats)")
        .def("world_stats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["vision_width"] = self.visionWidth_;
            stats["vision_height"] = self.visionHeight_;
            stats["agent_x"] = self.agent_.x;
            stats["agent_y"] = self.agent_.y;
            stats["agent_energy"] = self.agent_.energy;
            stats["agent_health"] = self.agent_.health;
            stats["agent_orientation"] = self.agent_.orientation;
            stats["object_count"] = static_cast<int>(self.objects_.size());
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            stats["simulation_time"] = self.getSimulationTime();
            stats["random_seed"] = self.getRandomSeed();
            return stats;
        }, "Get world stats (Pythonic alias)")
        .def("validate_world_state", [](SimpleWorld& self) {
            bool is_valid = true;
            std::vector<std::string> errors;
            
            // Check agent bounds
            if (self.agent_.x < 0 || self.agent_.x >= self.width_ || 
                self.agent_.y < 0 || self.agent_.y >= self.height_) {
                errors.push_back("Agent outside world bounds");
                is_valid = false;
            }
            
            // Check agent energy
            if (self.agent_.energy < 0) {
                errors.push_back("Agent energy is negative");
                is_valid = false;
            }
            
            // Check object bounds
            for (const auto& obj : self.objects_) {
                if (obj.x < 0 || obj.x >= self.width_ || 
                    obj.y < 0 || obj.y >= self.height_) {
                    errors.push_back("Object outside world bounds");
                    is_valid = false;
                    break;
                }
            }
            
            return py::make_tuple(is_valid, errors);
        }, "Validate world state (alias: check_world_state)")
        .def("check_world_state", [](SimpleWorld& self) {
            bool is_valid = true;
            std::vector<std::string> errors;
            
            // Check agent bounds
            if (self.agent_.x < 0 || self.agent_.x >= self.width_ || 
                self.agent_.y < 0 || self.agent_.y >= self.height_) {
                errors.push_back("Agent outside world bounds");
                is_valid = false;
            }
            
            // Check agent energy
            if (self.agent_.energy < 0) {
                errors.push_back("Agent energy is negative");
                is_valid = false;
            }
            
            // Check object bounds
            for (const auto& obj : self.objects_) {
                if (obj.x < 0 || obj.x >= self.width_ || 
                    obj.y < 0 || obj.y >= self.height_) {
                    errors.push_back("Object outside world bounds");
                    is_valid = false;
                    break;
                }
            }
            
            return py::make_tuple(is_valid, errors);
        }, "Check world state (Pythonic alias)")

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("initialize_from_dict", [](Brain& self, const py::dict& config_dict) {
            auto config = std::make_shared<Config>();
            config->loadFromDict(config_dict);
            self.initialize();
            return true;
        }, py::arg("config_dict"),
             "Initialize brain from Python dictionary configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp (alias: step)")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with single argument (Pythonic)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron (alias: inject_into_neuron)")
        .def("inject_into_neuron", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Pythonic alias: inject_current")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (alias: inject_to_neuron_type)")
        .def("inject_to_neuron_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Pythonic alias: inject_current_to_neurons")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity (alias: produce_motor_action)")
        .def("produce_motor_action", &Brain::produceAction,
             "Pythonic alias: produce_action")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("reset_all", &Brain::reset,
             "Reset all brain state and memories (alias: reset)")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (alias: save_to_file)")
        .def("save_to_file", &Brain::save, py::arg("filepath"),
             "Save brain state to file (Pythonic alias)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file (alias: load_from_file)")
        .def("load_from_file", &Brain::load, py::arg("filepath"),
             "Load brain state from file (Pythonic alias)")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region (alias: create_region)")
        .def("create_region", &Brain::addRegion, py::arg("name") = "",
             "Create a new neural region (Pythonic alias)")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID (alias: get_region)")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Pythonic alias: get_region")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions (alias: get_region_count)")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions (Pythonic alias)")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs (alias: get_region_ids)")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs (Pythonic alias)")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (alias: regions)")
        .def("regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (Pythonic alias)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions (alias: neuron_count)")
        .def("neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count (Pythonic alias)")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions (alias: synapse_count)")
        .def("synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count (Pythonic alias)")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons (alias: active_neuron_count)")
        .def("active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons (Pythonic alias)")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons (alias: firing_neuron_count)")
        .def("firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons (Pythonic alias)")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (alias: firing_rate)")
        .def("firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate (Pythonic alias)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio (alias: ei_ratio)")
        .def("ei_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio (Pythonic alias)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count (alias: total_spikes)")
        .def("total_spikes", &Brain::getTotalSpikeCount,
             "Get total spike count (Pythonic alias)")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage (alias: developmental_stage)")
        .def("developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage (Pythonic alias)")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (alias: set_development_stage)")
        .def("set_development_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (Pythonic alias)")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration (alias: config)")
        .def("config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration (Pythonic alias)")
        .def("logStatus", &Brain::logStatus,
             "Log brain status (alias: log)")
        .def("log", &Brain::logStatus,
             "Log brain status (Pythonic alias)")
        .def("run_simulation", [](Brain& self, int steps, bool verbose = false) {
            for (int i = 0; i < steps; ++i) {
                self.step(i);
                if (verbose) {
                    self.logStatus();
                }
            }
        }, py::arg("steps"), py::arg("verbose") = false,
             "Run simulation for a number of steps with optional logging")
        .def("create_random_network", [](Brain& self, int neuronCount, float connectivity, const std::string& regionName) {
            // Create region
            RegionId regionId = self.addRegion(regionName);
            NeuralRegion* region = self.getRegion(regionId);
            if (region) {
                // Note: This is a simplified implementation - real implementation would use NeuralPopulation
                return regionId;
            }
            return INVALID_REGION_ID;
        }, py::arg("neuronCount"), py::arg("connectivity") = 0.1f, py::arg("regionName") = "default",
             "Create a random neural network in a specified region")
        .def("export_spikes_to_csv", [](Brain& self, const std::string& filepath) {
            // Simplified implementation - would need spike system access
            return false;
        }, py::arg("filepath"),
             "Export spike data to CSV file");
        
        // Spike system utilities
        .def("get_spike_system", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system reference")
        .def("get_stdp_system", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system reference (alias: get_stdp)")
        .def("get_stdp", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Pythonic alias for get_stdp_system")
        .def("get_plasticity_systems", [](Brain& self) {
            py::dict systems;
            if (auto* stdp = self.getSTDP()) systems["stdp"] = stdp;
            if (auto* hebbian = self.getHebbian()) systems["hebbian"] = hebbian;
            if (auto* structural = self.getStructuralPlasticity()) systems["structural"] = structural;
            return systems;
        }, "Get all plasticity system references")
        .def("get_memory_systems", [](Brain& self) {
            py::dict systems;
            if (auto* working = self.getWorkingMemory()) systems["working"] = working;
            if (auto* episodic = self.getEpisodicMemory()) systems["episodic"] = episodic;
            if (auto* associative = self.getAssociativeMemory()) systems["associative"] = associative;
            return systems;
        }, "Get all memory system references")
        .def("get_neuromodulation_systems", [](Brain& self) {
            py::dict systems;
            if (auto* dopamine = self.getDopamine()) systems["dopamine"] = dopamine;
            if (auto* curiosity = self.getCuriosity()) systems["curiosity"] = curiosity;
            if (auto* novelty = self.getNovelty()) systems["novelty"] = novelty;
            if (auto* error = self.getPredictionErrorSignal()) systems["prediction_error"] = error;
            return systems;
        }, "Get all neuromodulation system references")
        .def("get_cognition_systems", [](Brain& self) {
            py::dict systems;
            if (auto* planner = self.getPlanner()) systems["planner"] = planner;
            if (auto* concept = self.getConceptFormation()) systems["concept_formation"] = concept;
            if (auto* attention = self.getAttention()) systems["attention"] = attention;
            return systems;
        }, "Get all cognition system references")
        .def("get_development_system", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system reference (alias: get_development)")
        .def("get_development", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Pythonic alias for get_development_system")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system reference (alias: get_prediction)")
        .def("get_prediction", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Pythonic alias for get_prediction_system")
        .def("get_random_generator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator reference (alias: rng)")
        .def("rng", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Pythonic alias for get_random_generator")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count (alias: total_spikes)")
        .def("get_pending_spike_count", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events (alias: pending_spikes)")
        .def("pending_spikes", &Brain::getPendingSpikeEventCount,
             "Get pending spike count (Pythonic alias)")
        .def("update_plasticity", &Brain::updatePlasticity,
             "Update plasticity rules (alias: update_stdp)")
        .def("update_stdp", &Brain::updatePlasticity,
             "Update STDP rules (Pythonic alias)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (alias: update_development)")
        .def("update_development", &Brain::develop,
             "Apply developmental changes (Pythonic alias)")
        .def("apply_neuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signal (alias: modulate)")
        .def("modulate", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Pythonic alias for apply_neuromodulation")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Receive sensory input (alias: inject_sensory)")
        .def("inject_sensory", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Pythonic alias for receive_sensory_input")
        .def("inject_current_to_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current to neurons of specific type (Pythonic alias for inject_current_to_neurons)")
        .def("get_stats", [](Brain& self) {
            py::dict stats;
            stats["neurons"] = self.getTotalNeuronCount();
            stats["synapses"] = self.getTotalSynapseCount();
            stats["active_neuron_count"] = self.getActiveNeuronCount();
            stats["firing_neuron_count"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["region_count"] = self.getRegionCount();
            return stats;
        }, "Get brain statistics as Python dictionary")
        .def("get_version", []() {
            // Return a static version string
            return std::string("NLM 2.0.0 Python Bindings");
        }, "Get library version");
        
        // Error handling and input validation
        .def("validate_parameters", [](Brain& self, const py::dict& params) {
            bool is_valid = true;
            std::vector<std::string> errors;
            
            if (params.contains("steps")) {
                int steps = params["steps"].cast<int>();
                if (steps < 0) {
                    errors.push_back("Steps cannot be negative");
                    is_valid = false;
                }
            }
            
            if (params.contains("neuron_count")) {
                int count = params["neuron_count"].cast<int>();
                if (count <= 0) {
                    errors.push_back("Neuron count must be positive");
                    is_valid = false;
                }
            }
            
            return py::make_tuple(is_valid, errors);
        }, py::arg("params"), "Validate simulation parameters")

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Module-level functions
    m.def("getVersion", []() {
        return std::string("NLM 2.0.0 Python Bindings");
    }, "Get library version");
    
    m.def("get_version", []() {
        return std::string("NLM 2.0.0 Python Bindings");
    }, "Get library version (Pythonic alias)");
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for Python users
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        return config;
    }, "Create a default configuration with sensible defaults");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain with configuration");
    
    m.def("createBrainFromDict", [](const py::dict& config_dict) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromDict(config_dict);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config_dict"), "Create and initialize a brain from Python dictionary");
    
    m.def("createBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("filepath"), "Create and initialize a brain from configuration file");
    
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
