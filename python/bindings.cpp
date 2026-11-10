#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        This enhanced Python API provides:
        - Advanced brain simulation control
        - Memory system management
        - Neuromodulation control
        - Development system integration
        - Better error handling and validation
        - Performance monitoring
        - Export/import capabilities
        - Configuration management
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==================== Type Definitions ====================
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
        .def("__ne__", &NeuronId::operator!=)
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
        .def("__ne__", &NeuronId::operator!=)
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
        .def("__ne__", &NeuronId::operator!=);

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

    // ==================== Configuration System ====================
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Advanced configuration system for NLM)

        The Config class provides:
        - Key-value configuration storage
        - Type-safe configuration access
        - File I/O support
        - Command-line integration
        - Configuration validation
        - Default value management
        - Configuration validation
        - Profile management
        - Environment variable support
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success.")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
            "Load configuration from command line arguments.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("getString", [](const Config& self, const std::string& key) {
            auto opt = self.get<std::string>(key);
            if (opt) return *opt;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
            "Get a string configuration value. Raises KeyError if key doesn't exist.")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto opt = self.get<int64_t>(key);
            if (opt) return *opt;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
            "Get an integer configuration value. Raises KeyError if key doesn't exist.")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto opt = self.get<float>(key);
            if (opt) return *opt;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
            "Get a float configuration value. Raises KeyError if key doesn't exist.")
        .def("getDouble", [](const Config& self, const std::string& key) {
            auto opt = self.get<double>(key);
            if (opt) return *opt;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
            "Get a double configuration value. Raises KeyError if key doesn't exist.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set a string configuration value.")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set an integer configuration value.")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set a float configuration value.")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set a double configuration value.")
        .def("setIntSafe", [](Config& self, const std::string& key, int64_t value, const std::string& source) {
            ConfigSource src;
            if (source == "default") src = ConfigSource::Default;
            else if (source == "user") src = ConfigSource::User;
            else if (source == "config_file") src = ConfigSource::ConfigFile;
            else if (source == "command_line") src = ConfigSource::CommandLine;
            else src = ConfigSource::Default;
            self.set(key, value, src);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "default",
            "Set configuration value with explicit source.")
        .def("getOrString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with default. Returns default if key doesn't exist.")
        .def("getOrInt", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get integer configuration value with default.")
        .def("getOrFloat", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get float configuration value with default.")
        .def("getOrDouble", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get double configuration value with default.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("validate", &Config::validate,
             "Validate configuration consistency. Returns True if valid.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ==================== Sensory Inputs ====================
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
        }, py::arg("data"), "Set vision data")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("normalize", &Vision::normalize,
             "Normalize vision data to range [-1, 1]");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getFrequencySpectrum", &Audio::getFrequencySpectrum,
             "Get frequency spectrum of audio data");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("getSignalSum", &InternalSignals::getSignalSum,
             "Get sum of all signals")
        .def("getSignalAverage", &InternalSignals::getSignalAverage,
             "Get average of all signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals");

    // ==================== Actions ====================
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getDescription)
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("toDict", [](const Action& action) {
            py::dict result;
            result["type"] = static_cast<int>(action.getType());
            result["parameters"] = action.getParameters();
            result["name"] = action.getName();
            result["description"] = action.getDescription();
            return result;
        }, "Convert action to dictionary representation")
        .def("?", [](const Action& action, const std::string& query) {
            return action.queryMetadata(query);
        }, py::arg("query"), "Query action metadata (e.g., 'energy', 'duration')");

    // ==================== World Objects ====================
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Advanced world object representation)pbdoc")
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
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isNear", [](const WorldObject& obj, float x, float y, float threshold) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return (dx*dx + dy*dy) <= (threshold*threshold);
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if object is within distance")
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position");

    // ==================== Agent Body ====================
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Advanced agent body state)pbdoc")
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
        .def("updatePosition", [](AgentBody& body, double dt) {
            body.x += body.velocityX * dt;
            body.y += body.velocityY * dt;
            body.orientation += body.angularVelocity * dt;
        }, py::arg("dt"), "Update position based on velocity")
        .def("moveTo", [](AgentBody& body, float x, float y, double speed) {
            float dx = x - body.x;
            float dy = y - body.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance > 0) {
                body.velocityX = dx / distance * speed;
                body.velocityY = dy / distance * speed;
                body.isMoving = true;
            }
        }, py::arg("x"), py::arg("y"), py::arg("speed"), "Move toward target position")
        .def("getPosition", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get current position as (x, y) tuple")
        .def("getHeading", [](const AgentBody& body) {
            return body.orientation;
        }, "Get current heading (orientation)")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // ==================== Sensory Percept ====================
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Advanced sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("toDict", [](const SensoryPercept& percept) {
            py::dict result;
            auto vision = percept.getVision();
            if (vision) result["vision"] = *vision;
            auto touch = percept.getTouch();
            if (touch) result["touch"] = *touch;
            auto internal = percept.getInternal();
            if (internal) result["internal"] = *internal;
            auto proprioception = percept.getProprioception();
            if ( proprioception) result["proprioception"] = *proprioception;
            auto audio = percept.getAudio();
            if (audio) result["audio"] = *audio;
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert percept to dictionary representation")
        .def("fromDict", [](py::dict d) {
            SensoryPercept percept;
            if (d.contains("vision")) {
                auto vision = d["vision"].cast<Vision>();
                percept.setVision(vision);
            }
            if (d.contains("touch")) {
                auto touch = d["touch"].cast<InternalSignals>();
                percept.setTouch(touch);
            }
            if (d.contains("internal")) {
                auto internal = d["internal"].cast<InternalSignals>();
                percept.setInternal(internal);
            }
            if (d.contains("proprioception")) {
                auto proprioception = d["proprioception"].cast<InternalSignals>();
                percept.setProprioception(proprioception);
            }
            if (d.contains("audio")) {
                auto audio = d["audio"].cast<Audio>();
                percept.setAudio(audio);
            }
            if (d.contains("timestamp")) {
                auto timestamp = d["timestamp"].cast<double>();
                percept.setTimestamp(timestamp);
            }
            return percept;
        }, py::arg("dict"), "Create percept from dictionary representation");

    // ==================== Simple World ====================
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Advanced 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("findNearestObject", [](SimpleWorld& world, float x, float y, WorldObjectType type = WorldObjectType::Empty) {
            WorldObject* nearest = nullptr;
            float minDist = std::numeric_limits<float>::max();
            for (auto& obj : world.objects) {
                if (!obj.active) continue;
                if (type != WorldObjectType::Empty && obj.type != type) continue;
                float dx = obj.x - x;
                float dy = obj.y - y;
                float dist = std::sqrt(dx*dx + dy*dy);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = &obj;
                }
            }
            return nearest;
        }, py::arg("x"), py::arg("y"), py::arg("type") = WorldObjectType::Empty,
             "Find nearest object of specified type")
        .def("getObjectsByType", [](SimpleWorld& world, WorldObjectType type) {
            std::vector<WorldObject> result;
            for (auto& obj : world.objects) {
                if (obj.active && obj.type == type) {
                    result.push_back(obj);
                }
            }
            return result;
        }, py::arg("type"), "Get all objects of specified type")
        .def("getResourceCount", [](SimpleWorld& world) {
            int count = 0;
            for (auto& obj : world.objects) {
                if (obj.active && obj.type == WorldObjectType::Resource) {
                    count++;
                }
            }
            return count;
        }, "Get number of resources in world")
        .def("getHazardCount", [](SimpleWorld& world) {
            int count = 0;
            for (auto& obj : world.objects) {
                if (obj.active && obj.type == WorldObjectType::Hazard) {
                    count++;
                }
            }
            return count;
        }, "Get number of hazards in world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not colliding)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world updates")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getWorldBounds", [](SimpleWorld& world) {
            return py::make_tuple(0.0f, 0.0f, world.width, world.height);
        }, "Get world bounds as (xmin, ymin, xmax, ymax)")
        .def("getCenter", [](SimpleWorld& world) {
            return py::make_tuple(world.width / 2.0f, world.height / 2.0f);
        }, "Get world center position");

    // ==================== Brain ====================
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Advanced neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             "Save brain state to file. Returns True on success.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True on success.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get number of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("enablePlasticity", [](Brain& brain, bool hebbian, bool stdp, bool rewardModulated) {
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(hebbian, stdp, rewardModulated);
                }
            }
        }, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
            "Enable plasticity for all synapses in first region")
        .def("disablePlasticity", [](Brain& brain) {
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(false, false, false);
                }
            }
        }, "Disable all plasticity in brain")
        .def("getSynapseStats", [](Brain& brain) {
            size_t total = brain.getTotalSynapseCount();
            size_t plastic = 0;
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    auto flags = syn->getPlasticityFlags();
                    if (flags.hebbian || flags.stdp || flags.reward_modulated) {
                        plastic++;
                    }
                }
            }
            return py::make_tuple(total, plastic, plastic * 100.0f / total);
        }, "Get synapse statistics (total, plastic, plastic_percentage)")
        .def("getNeuronStats", [](Brain& brain) {
            size_t total = brain.getTotalNeuronCount();
            size_t active = brain.getActiveNeuronCount();
            size_t firing = brain.getFiringNeuronCount();
            return py::make_tuple(total, active, firing, active * 100.0f / total);
        }, "Get neuron statistics (total, active, firing, active_percentage)")
        .def("getMemoryStats", [](Brain& brain) {
            py::dict stats;
            if (auto* workingMem = brain.getSpikeSystem()) {
                // Add working memory stats if available
                stats["working_memory"] = "available";
            }
            return stats;
        }, "Get memory system statistics");

    // ==================== AgentBrain ====================
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("simulateStep", [](AgentBrain& agent, const SensoryPercept& percept, double timestep) {
            agent.processSensoryInput(percept);
            agent.updateDevelopment(0.1);
            auto action = agent.decodeMotorCommand();
            return action;
        }, py::arg("percept"), py::arg("timestep") = 0.1,
             "Simulate one step: process input, update, get output action")
        .def("learnFromReward", [](AgentBrain& agent, float reward, float predictedReward) {
            agent.applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predictedReward"),
             "Learn from reward signal");

    // ==================== Convenience Functions ====================
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with optimized NLM settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced factory functions
    m.def("createBrainWithConfig", [](py::dict configDict) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        for (auto& item : configDict) {
            std::string key = item.first.cast<std::string>();
            std::string type = item.second.get_type();
            if (type == "int") {
                config->setIntSafe(key, item.second.cast<int64_t>());
            } else if (type == "float") {
                config->setFloatSafe(key, item.second.cast<float>());
            } else if (type == "string") {
                config->setStringSafe(key, item.second.cast<std::string>());
            }
        }
        return std::make_shared<Brain>(config);
    }, py::arg("configDict"), "Create brain from Python dictionary configuration");

    m.def("createWorldFromConfig", [](const std::shared_ptr<Config>& config) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        auto width = config->getOrInt("world_width", 100);
        auto height = config->getOrInt("world_height", 100);
        auto visionWidth = config->getOrInt("vision_width", 20);
        auto visionHeight = config->getOrInt("vision_height", 20);
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("config"), "Create world from configuration");

    // Error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Utility functions
    m.def("version", []() {
        return "NLM Python API v2.0 (Enhanced)";
    }, "Get NLM Python API version");

    m.def("getDefaultConfig", []() {
        py::dict defaults;
        defaults["neuron_count"] = 1000;
        defaults["region_count"] = 1;
        defaults["connection_probability"] = 0.1f;
        defaults["simulation_timestep"] = 0.001;
        defaults["random_seed"] = 42;
        defaults["working_memory_capacity"] = 100;
        defaults["episodic_memory_max_episodes"] = 1000;
        defaults["dopamine_baseline"] = 0.1f;
        return defaults;
    }, "Get default configuration as dictionary");

    // Advanced simulation utilities
    m.def("runSimulation", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                             size_t numSteps, bool enableLearning) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->updateDevelopment(0.1);
            
            if (enableLearning) {
                float reward = 0.0f;
                float predicted = 0.0f;
                agent->applyRewardModulation(reward, predicted);
            }
        }
        return world->getAgentBody();
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("enableLearning") = true,
         "Run complete simulation loop");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return py::make_tuple(steps, duration.count(), steps * 1000.0 / duration.count());
    }, py::arg("brain"), py::arg("steps"),
         "Benchmark brain performance (steps, milliseconds, steps_per_second)");

    // Object factories
    py::class_<WorldObjectFactory>(m, "WorldObjectFactory", R"pbdoc(Factory for creating world objects)pbdoc")
        .def_static("createResource", [](float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            obj.active = true;
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
            "Create a resource object")
        .def_static("createHazard", [](float x, float y, float damage) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.value = damage;
            obj.active = true;
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 1.0f,
            "Create a hazard object")
        .def_static("createMarker", [](float x, float y, const std::string& label) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Marker;
            obj.value = 0.0f;
            obj.active = true;
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("label") = "",
            "Create a marker object");

    // Invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
