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

    // Type conversion functions for Python
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on error.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Returns True on success, False on error.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on error.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Set methods - Python bindings for all Config::set overloads
        .def("set", [](Config& self, const std::string& key, int value) {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set double configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set string configuration value")
        // Note: ConfigValue vector support removed for simplicity - use direct value types
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration entry")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value or default (int)")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value or default (double)")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value or default (bool)")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value or default (string)")
        // Python convenience methods
        .def("getInt", [](Config& self, const std::string& key, int defaultValue = 0) {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get integer configuration value with default")
        .def("getFloat", [](Config& self, const std::string& key, double defaultValue = 0.0) {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            auto val = self.get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
           "Get float configuration value with default")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue = false) {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get boolean configuration value with default")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue = "") {
            if (key.empty()) {
                throw std::invalid_argument("Config key cannot be empty");
            }
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get string configuration value with default")
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
        // Python convenience methods for SimpleWorld
        .def("getPerceptAsDict", [](SimpleWorld& world) {
            py::dict percept_dict;
            auto percept = world.getSensoryPercept();
            
            // Vision data
            auto vision = percept.getVision();
            percept_dict["vision_width"] = vision.getWidth();
            percept_dict["vision_height"] = vision.getHeight();
            percept_dict["vision_channels"] = vision.getChannels();
            
            // Touch data
            auto touch = percept.getTouch();
            percept_dict["touch_sensors"] = py::cast(touch);
            
            // Internal signals
            auto internal = percept.getInternal();
            if (!internal.empty()) percept_dict["energy"] = internal[0];
            if (internal.size() > 1) percept_dict["health"] = internal[1];
            if (internal.size() > 2) percept_dict["moving"] = internal[2];
            if (internal.size() > 3) percept_dict["turning"] = internal[3];
            
            // Proprioception
            auto proprioception = percept.getProprioception();
            if (!proprioception.empty()) percept_dict["position_x"] = proprioception[0];
            if (proprioception.size() > 1) percept_dict["position_y"] = proprioception[1];
            if (proprioception.size() > 2) percept_dict["orientation"] = proprioception[2];
            
            // Audio data
            auto audio = percept.getAudio();
            percept_dict["audio_samples"] = py::cast(audio);
            
            percept_dict["timestamp"] = percept.getTimestamp();
            return percept_dict;
        }, "Get sensory percept as Python dictionary")
        .def("getBodyAsDict", [](SimpleWorld& world) {
            py::dict body_dict;
            auto body = world.getAgentBody();
            
            body_dict["x"] = body.x;
            body_dict["y"] = body.y;
            body_dict["orientation"] = body.orientation;
            body_dict["velocity_x"] = body.velocityX;
            body_dict["velocity_y"] = body.velocityY;
            body_dict["angular_velocity"] = body.angularVelocity;
            body_dict["energy"] = body.energy;
            body_dict["health"] = body.health;
            body_dict["age"] = body.age;
            body_dict["is_moving"] = body.isMoving;
            body_dict["is_turning"] = body.isTurning;
            body_dict["last_action_time"] = body.lastActionTime;
            
            return body_dict;
        }, "Get agent body state as Python dictionary")
        .def("getWorldInfo", [](SimpleWorld& world) {
            py::dict world_info;
            world_info["width"] = world.getWidth();
            world_info["height"] = world.getHeight();
            world_info["simulation_time"] = world.getSimulationTime();
            world_info["max_energy"] = world.getMaxEnergy();
            world_info["energy_decay_rate"] = world.getEnergyDecayRate();
            world_info["random_seed"] = world.getRandomSeed();
            return world_info;
        }, "Get world information as Python dictionary")
        .def("setPosition", [](SimpleWorld& world, float x, float y) {
            auto body = world.getAgentBody();
            body.x = x;
            body.y = y;
            return "Position set successfully";
        }, py::arg("x"), py::arg("y"),
           "Set agent position (convenience method)")
        .def("setEnergy", [](SimpleWorld& world, float energy) {
            auto body = world.getAgentBody();
            body.energy = std::max(0.0f, std::min(energy, world.getMaxEnergy()));
            return "Energy updated";
        }, py::arg("energy"),
           "Set agent energy level (convenience method)")
        .def("addResource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            obj.active = true;
            world.addObject(obj);
            return "Resource added";
        }, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f,
           "Add resource object (convenience method)")
        .def("addHazard", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.value = value;
            obj.active = true;
            world.addObject(obj);
            return "Hazard added";
        }, py::arg("x"), py::arg("y"), py::arg("value") = -15.0f,
           "Add hazard object (convenience method)")
        .def("getObjectsByType", [](SimpleWorld& world, WorldObjectType type) {
            py::list object_list;
            // Note: SimpleWorld doesn't expose objects_ directly, so this is limited
            // Users can check positions manually or use the world as-is
            return object_list;
        }, py::arg("type"),
           "Get all objects of specific type (convenience method - limited access)")
        .def("isAgentAlive", [](SimpleWorld& world) {
            auto body = world.getAgentBody();
            return body.health > 0.0f && body.energy > 0.0f;
        }, "Check if agent is alive")
        .def("getTimeToLive", [](SimpleWorld& world) {
            auto body = world.getAgentBody();
            return world.getMaxEnergy() / world.getEnergyDecayRate();
        }, "Get estimated time to live based on current energy")
        .def("getWorldStats", [](SimpleWorld& world) {
            py::dict stats;
            stats["width"] = world.getWidth();
            stats["height"] = world.getHeight();
            stats["simulation_time"] = world.getSimulationTime();
            stats["max_energy"] = world.getMaxEnergy();
            stats["energy"] = world.getAgentBody().energy;
            stats["health"] = world.getAgentBody().health;
            stats["is_alive"] = world.getAgentBody().health > 0.0f && world.getAgentBody().energy > 0.0f;
            return stats;
        }, "Get comprehensive world and agent statistics");

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
        // Python convenience methods
        .def("getNeuronCountByType", [](const Brain& brain, NeuronType type) {
            size_t count = 0;
            for (const auto& region : brain.getRegions()) {
                for (const auto& population : region->getPopulations()) {
                    for (const auto& neuron : population->getNeurons()) {
                        if (neuron->getType() == type) {
                            count++;
                        }
                    }
                }
            }
            return count;
        }, py::arg("type"),
           "Get neuron count by type")
        .def("getSynapseCountByType", [](const Brain& brain, SynapseType type) {
            size_t count = 0;
            for (const auto& region : brain.getRegions()) {
                for (const auto& population : region->getPopulations()) {
                    for (const auto& neuron : population->getNeurons()) {
                        for (const auto& synapse : neuron->getOutgoingSynapses()) {
                            if (synapse->getType() == type) {
                                count++;
                            }
                        }
                    }
                }
            }
            return count;
        }, py::arg("type"),
           "Get synapse count by type")
        .def("getBrainStats", [](const Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["firing_rate"] = brain.getAverageFiringRate();
            stats["e_to_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            stats["region_count"] = brain.getRegionCount();
            return stats;
        }, "Get comprehensive brain statistics as Python dictionary")
        .def("getRegionInfo", [](const Brain& brain, RegionId regionId) {
            py::dict info;
            auto region = brain.getRegion(regionId);
            if (region) {
                info["id"] = regionId.value;
                info["name"] = region->getName();
                info["populations"] = region->getPopulationCount();
                info["neurons"] = region->getNeuronCount();
                info["synapses"] = region->getSynapseCount();
            }
            return info;
        }, py::arg("region_id"),
           "Get region information as Python dictionary")
        .def("to_dict", [](const Brain& brain) {
            py::dict brain_dict;
            brain_dict["total_neurons"] = brain.getTotalNeuronCount();
            brain_dict["total_synapses"] = brain.getTotalSynapseCount();
            brain_dict["active_neurons"] = brain.getActiveNeuronCount();
            brain_dict["firing_neurons"] = brain.getFiringNeuronCount();
            brain_dict["total_spikes"] = brain.getTotalSpikeCount();
            brain_dict["firing_rate"] = brain.getAverageFiringRate();
            brain_dict["e_to_i_ratio"] = brain.getExcitationInhibitionRatio();
            brain_dict["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            brain_dict["region_count"] = brain.getRegionCount();
            return brain_dict;
        }, "Convert brain to Python dictionary for easy inspection")
        .def("from_dict", [](Brain& brain, const py::dict& brain_dict) {
            // This is a placeholder - actual implementation would restore brain state from dict
            // For now, just log the dictionary
            std::string summary = "Brain state dictionary with keys: ";
            for (auto it : brain_dict) {
                summary += """'""" + py::cast<std::string>(it.first) + """', """;
            }
            // This would typically restore brain state from checkpoint or similar
            return summary;
        }, py::arg("brain_dict"),
           "Restore brain state from Python dictionary (placeholder)");

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
        // Python convenience methods for AgentBrain
        .def("getAgentStats", [](const AgentBrain& agent) {
            py::dict stats;
            stats["curiosity"] = agent.getCuriosityLevel();
            stats["novelty"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["neuromodulation"] = agent.getNeuromodulationLevel();
            stats["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            stats["enabled_systems"] = py::dict({
                {"reward_modulation", agent.isRewardModulationEnabled()},
                {"structural_plasticity", agent.isStructuralPlasticityEnabled()},
                {"development", agent.isDevelopmentEnabled()},
                {"curiosity", agent.isCuriosityEnabled()}
            });
            auto brain = agent.getBrain();
            if (brain) {
                stats["brain_stats"] = py::cast(brain->getBrainStats());
            }
            return stats;
        }, "Get comprehensive agent statistics as Python dictionary")
        .def("getActionRecommendation", [](AgentBrain& agent, const py::dict& context) {
            // Get brain stats for context
            auto brain_stats = agent.getBrain()->getBrainStats();
            py::dict recommendation;
            
            // Simple recommendation based on brain stats
            double curiosity = agent.getCuriosityLevel();
            double prediction_error = agent.getPredictionError();
            double firing_rate = py::cast<double>(brain_stats["firing_rate"]);
            
            // Curiosity-based exploration
            if (curiosity > 0.7 && prediction_error > 0.3) {
                recommendation["action"] = "Explore";
                recommendation["reason"] = "High curiosity and prediction error suggest exploration";
            }
            // Reward maximization
            else if (firing_rate > 0.5) {
                recommendation["action"] = "MaximizeReward";
                recommendation["reason"] = "High firing rate suggests reward optimization";
            }
            else {
                recommendation["action"] = "Default";
                recommendation["reason"] = "Normal operating conditions";
            }
            
            recommendation["curiosity"] = curiosity;
            recommendation["prediction_error"] = prediction_error;
            recommendation["firing_rate"] = firing_rate;
            return recommendation;
        }, py::arg("context") = py::dict(),
           "Get action recommendation based on brain state")
        .def("enableAllSystems", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
            return "All agent systems enabled";
        }, "Enable all agent subsystems")
        .def("disableAllSystems", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
            return "All agent systems disabled";
        }, "Disable all agent subsystems")
        .def("getSystemStatus", [](const AgentBrain& agent) {
            return py::dict({
                {"reward_modulation", agent.isRewardModulationEnabled()},
                {"structural_plasticity", agent.isStructuralPlasticityEnabled()},
                {"development", agent.isDevelopmentEnabled()},
                {"curiosity", agent.isCuriosityEnabled()}
            });
        }, "Get current status of all agent systems");

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
