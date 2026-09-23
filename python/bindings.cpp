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
        // Pythonic property access
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.getValue(key);
            if (!value) {
                throw std::runtime_error("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get configuration value by key")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("getInt", &Config::getOr<int64_t>, py::arg("key"), py::arg("default"),
             "Get integer value from config")
        .def("getFloat", &Config::getOr<float>, py::arg("key"), py::arg("default"),
             "Get float value from config")
        .def("getString", &Config::getOr<std::string>, py::arg("key"), py::arg("default"),
             "Get string value from config")
        .def("getBool", [](Config& self, const std::string& key) {
            auto value = self.getOr<bool>(key, false);
            return value;
        }, py::arg("key"), "Get boolean value from config")
        // Convenience methods for common configuration patterns
        .def("setBrainParams", [](Config& self, int neuron_count, float connection_prob) {
            self.set("brain.neuron_count", static_cast<int64_t>(neuron_count));
            self.set("brain.connection_probability", connection_prob);
        }, py::arg("neuron_count"), py::arg("connection_prob"),
           "Set common brain parameters")
        .def("setPlasticityParams", [](Config& self, float stdp_rate, float hebbian_rate) {
            self.set("plasticity.stdp.learning_rate", stdp_rate);
            self.set("plasticity.hebbian.learning_rate", hebbian_rate);
        }, py::arg("stdp_rate"), py::arg("hebbian_rate"),
           "Set plasticity parameters")
        .def("getBrainInfo", [](const Config& self) {
            std::map<std::string, std::string> info;
            
            if (auto neuron_count = self.getOr<size_t>("brain.neuron_count")) {
                info["neuron_count"] = std::to_string(*neuron_count);
            }
            if (auto region_count = self.getOr<size_t>("brain.region_count")) {
                info["region_count"] = std::to_string(*region_count);
            }
            if (auto connection_prob = self.getOr<float>("brain.connection_probability")) {
                info["connection_probability"] = std::to_string(*connection_prob);
            }
            
            return info;
        }, "Get brain configuration summary as dictionary")
        .def("validate", [](const Config& self) {
            std::vector<std::string> errors;
            
            // Check required keys
            auto keys = self.getKeys();
            if (!self.has("brain.neuron_count")) {
                errors.push_back("Missing 'brain.neuron_count'");
            }
            if (!self.has("brain.connection_probability")) {
                errors.push_back("Missing 'brain.connection_probability'");
            }
            
            // Validate ranges
            if (auto neuron_count = self.getOr<size_t>("brain.neuron_count")) {
                if (*neuron_count == 0) {
                    errors.push_back("'brain.neuron_count' must be positive");
                }
            }
            if (auto connection_prob = self.getOr<float>("brain.connection_probability")) {
                if (*connection_prob < 0.0f || *connection_prob > 1.0f) {
                    errors.push_back("'brain.connection_probability' must be between 0.0 and 1.0");
                }
            }
            
            return errors;
        }, "Validate configuration and return list of errors (empty if valid)");

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def("getReward", &ActionResult::reward, "Get reward value")
        .def("setReward", &ActionResult::reward, py::arg("reward"), "Set reward value")
        .def("getSuccess", &ActionResult::success, "Get success status")
        .def("setSuccess", &ActionResult::success, py::arg("success"), "Set success status")
        .def("getMessage", &ActionResult::message, "Get message text")
        .def("setMessage", &ActionResult::message, py::arg("message"), "Set message text")
        .def("isSuccessful", &ActionResult::success, "Check if action was successful")
        .def("toDict", [](const ActionResult& result) {
            std::map<std::string, std::string> dict;
            dict["reward"] = std::to_string(result.reward);
            dict["success"] = result.success ? "true" : "false";
            dict["message"] = result.message;
            return dict;
        }, "Convert ActionResult to dictionary")
        .def("__repr__", [](const ActionResult& result) {
            std::string success_str = result.success ? "SUCCESS" : "FAILURE";
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   ", success=" + success_str +
                   ", message='" + result.message + "'>";
        });

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
        // Pythonic convenience methods
        .def("getPosition", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getVelocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("setVelocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, py::arg("velocityX"), py::arg("velocityY"), "Set agent velocity")
        .def("getStats", [](const AgentBody& body) {
            std::map<std::string, double> stats;
            stats["x"] = body.x;
            stats["y"] = body.y;
            stats["orientation"] = body.orientation;
            stats["velocityX"] = body.velocityX;
            stats["velocityY"] = body.velocityY;
            stats["energy"] = body.energy;
            stats["health"] = body.health;
            stats["age"] = body.age;
            stats["isMoving"] = body.isMoving;
            stats["isTurning"] = body.isTurning;
            return stats;
        }, "Get agent body statistics as dictionary")
        .def("updatePosition", [](AgentBody& body, float dx, float dy, float dt) {
            body.x += dx * dt;
            body.y += dy * dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Update position based on velocity and time")
        .def("updateVelocity", [](AgentBody& body, float ax, float ay, float dt) {
            body.velocityX += ax * dt;
            body.velocityY += ay * dt;
        }, py::arg("ax"), py::arg("ay"), py::arg("dt"), "Update velocity based on acceleration and time")
        .def("isMovingFast", [](const AgentBody& body, float threshold) {
            float speed = std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
            return speed > threshold;
        }, py::arg("threshold"), "Check if agent is moving faster than threshold speed")
        .def("getDistanceTo", [](const AgentBody& body1, const AgentBody& body2) {
            float dx = body2.x - body1.x;
            float dy = body2.y - body1.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Get distance to another agent body");

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
        // Pythonic convenience methods
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position (Pythonic alias)")
        .def("configure_world", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions (Pythonic alias)")
        .def("apply_action", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("time"),
             "Apply motor command (Pythonic alias)")
        .def("get_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept (Pythonic alias)")
        .def("get_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state (Pythonic alias)")
        .def("add_world_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world (Pythonic alias)")
        .def("remove_world_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (Pythonic alias)")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (Pythonic alias)")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy (Pythonic alias)")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("energy"),
             "Set maximum energy (Pythonic alias)")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate (Pythonic alias)")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("rate"),
             "Set energy decay rate (Pythonic alias)")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get simulation time (Pythonic alias)")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed (Pythonic alias)")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed (Pythonic alias)")
        // World state convenience methods
        .def("get_world_stats", [](const SimpleWorld& world) {
            std::map<std::string, double> stats;
            stats["width"] = world.getWidth();
            stats["height"] = world.getHeight();
            stats["max_energy"] = world.getMaxEnergy();
            stats["energy_decay_rate"] = world.getEnergyDecayRate();
            stats["simulation_time"] = world.getSimulationTime();
            auto& body = world.getAgentBody();
            stats["agent_x"] = body.x;
            stats["agent_y"] = body.y;
            stats["agent_orientation"] = body.orientation;
            stats["agent_energy"] = body.energy;
            return stats;
        }, "Get world statistics as dictionary")
        .def("get_world_status", [](SimpleWorld& world) {
            std::map<std::string, std::string> status;
            status["dimensions"] = std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight());
            status["max_energy"] = std::to_string(world.getMaxEnergy());
            status["energy_decay_rate"] = std::to_string(world.getEnergyDecayRate());
            status["simulation_time"] = std::to_string(world.getSimulationTime());
            auto& body = world.getAgentBody();
            status["agent_position"] = "(" + std::to_string(body.x) + ", " + std::to_string(body.y) + ")";
            status["agent_orientation"] = std::to_string(body.orientation);
            return status;
        }, "Get world status as dictionary")
        .def("create_resource", [](SimpleWorld& world, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Create a resource object")
        .def("create_hazard", [](SimpleWorld& world, float x, float y, float damage = -1.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = -1.0f, "Create a hazard object")
        .def("create_wall", [](SimpleWorld& world, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create a wall object")
        .def("remove_objects_by_type", [](SimpleWorld& world, WorldObjectType type) {
            for (auto it = world.getObjects().begin(); it != world.getObjects().end(); ) {
                if (it->type == type) {
                    it = world.getObjects().erase(it);
                } else {
                    ++it;
                }
            }
        }, py::arg("type"), "Remove all objects of given type")
        .def("get_objects_by_type", [](const SimpleWorld& world, WorldObjectType type) {
            std::vector<WorldObject> objects;
            for (const auto& obj : world.getObjects()) {
                if (obj.type == type) {
                    objects.push_back(obj);
                }
            }
            return objects;
        }, py::arg("type"), "Get all objects of given type")
        .def("get_objects_near", [](const SimpleWorld& world, float x, float y, float radius) {
            std::vector<WorldObject> nearby;
            for (const auto& obj : world.getObjects()) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx * dx + dy * dy <= radius * radius) {
                    nearby.push_back(obj);
                }
            }
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects within radius of position")

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
        // Pythonic convenience methods
        .def("runSteps", [](Brain& brain, size_t steps) {
            for (SimulationStep i = 0; i < steps; ++i) {
                brain.step(i);
            }
        }, py::arg("steps"), "Run multiple steps of simulation")
        .def("runStepsWithTime", [](Brain& brain, size_t steps, double timestep) {
            for (SimulationStep i = 0; i < steps; ++i) {
                brain.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep"), "Run multiple steps with specified timestep")
        .def("injectCurrentToType", [](Brain& brain, NeuronType type, float current) {
            brain.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"), "Inject current into all neurons of given type")
        .def("getBrainStats", [](const Brain& brain) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = static_cast<double>(brain.getTotalNeuronCount());
            stats["total_synapses"] = static_cast<double>(brain.getTotalSynapseCount());
            stats["active_neurons"] = static_cast<double>(brain.getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<double>(brain.getFiringNeuronCount());
            stats["total_spikes"] = static_cast<double>(brain.getTotalSpikeCount());
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("getRegionStats", [](const Brain& brain) {
            std::map<std::string, std::map<std::string, double>> stats;
            for (const auto& region : brain.getRegions()) {
                std::map<std::string, double> region_data;
                region_data["neuron_count"] = static_cast<double>(region->getTotalNeuronCount());
                region_data["synapse_count"] = static_cast<double>(region->getSynapseCount());
                region_data["average_weight"] = region->getAverageSynapticWeight();
                stats["Region_" + std::to_string(region->getId().index())] = region_data;
            }
            return stats;
        }, "Get statistics for all regions")
        .def("createLearningSession", [](std::shared_ptr<Brain> brain) {
            // Create a learning session helper for easy tracking
            struct LearningSession {
                std::shared_ptr<Brain> brain;
                size_t initial_neuron_count = 0;
                size_t initial_synapse_count = 0;
                float initial_ei_ratio = 0.0f;
                float final_ei_ratio = 0.0f;
                size_t steps_completed = 0;
                
                LearningSession(std::shared_ptr<Brain> b) : brain(b) {
                    if (brain) {
                        initial_neuron_count = brain->getTotalNeuronCount();
                        initial_synapse_count = brain->getTotalSynapseCount();
                        initial_ei_ratio = brain->getExcitationInhibitionRatio();
                    }
                }
                
                float getProgressRatio() const {
                    if (steps_completed == 0) return 0.0f;
                    return static_cast<float>(steps_completed) / 1000.0f; // Normalized to 1000 steps
                }
                
                std::string getStatus() const {
                    return "Learning: " + std::to_string(steps_completed) + " steps, " +
                           std::to_string(initial_neuron_count) + " neurons, " +
                           std::to_string(initial_synapse_count) + " synapses";
                }
            };
            return std::make_shared<LearningSession>(brain);
        }, "Create a learning session helper object for tracking progress")
        .def("enableAllPlasticity", [](Brain& brain) {
            auto* stdp = brain.getSTDP();
            auto* hebbian = brain.getHebbian();
            auto* structural = brain.getStructuralPlasticity();
            
            if (stdp) {
                stdp->setLTPWeight(0.01f);
                stdp->setLTDWeight(0.012f);
            }
            if (hebbian) {
                hebbian->setLearningRate(0.001f);
            }
            if (structural) {
                structural->setSynaptogenesisRate(0.0001f);
                structural->setPruningRate(0.00001f);
            }
        }, "Enable all plasticity mechanisms with default rates")
        .def("disableAllPlasticity", [](Brain& brain) {
            auto* stdp = brain.getSTDP();
            auto* hebbian = brain.getHebbian();
            auto* structural = brain.getStructuralPlasticity();
            
            if (stdp) {
                stdp->setLTPWeight(0.0f);
                stdp->setLTDWeight(0.0f);
            }
            if (hebbian) {
                hebbian->setLearningRate(0.0f);
            }
            if (structural) {
                structural->setSynaptogenesisRate(0.0f);
                structural->setPruningRate(0.0f);
            }
        }, "Disable all plasticity mechanisms")
        .def("setDevelopmentStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getDevelopmentStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("updateDevelopment", [](Brain& brain, float age) {
            // Map age to developmental stage
            if (age < 60.0f) {  // ~1 minute
                brain.setDevelopmentalStage(DevelopmentalStage::Initial);
            } else if (age < 300.0f) {  // ~5 minutes
                brain.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            } else if (age < 900.0f) {  // ~15 minutes
                brain.setDevelopmentalStage(DevelopmentalStage::Maturation);
            } else {
                brain.setDevelopmentalStage(DevelopmentalStage::Adult);
            }
        }, py::arg("age"), "Update developmental stage based on age (time)")
        .def("getMemoryStatus", [](const Brain& brain) {
            std::map<std::string, std::string> status;
            auto* wm = brain.getWorkingMemory();
            auto* em = brain.getEpisodicMemory();
            
            if (wm) {
                status["working_memory_traces"] = std::to_string(wm->getActiveTraces());
            } else {
                status["working_memory"] = "disabled";
            }
            
            if (em) {
                status["episodic_memory_episodes"] = std::to_string(em->getEpisodeCount());
            } else {
                status["episodic_memory"] = "disabled";
            }
            
            return status;
        }, "Get memory system status")

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
        // Pythonic convenience methods
        .def("enableAllLearning", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all learning mechanisms")
        .def("disableAllLearning", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all learning mechanisms")
        .def("getAgentStats", [](const AgentBrain& agent) {
            std::map<std::string, double> stats;
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["development_stage"] = static_cast<double>(agent.getDevelopmentalStage());
            return stats;
        }, "Get agent statistics as dictionary")
        .def("createAgentSession", [](std::shared_ptr<AgentBrain> agent) {
            // Create a session helper for tracking agent learning
            struct AgentSession {
                std::shared_ptr<AgentBrain> agent;
                size_t steps_completed = 0;
                float total_reward = 0.0f;
                float max_curiosity = 0.0f;
                float max_novelty = 0.0f;
                
                AgentSession(std::shared_ptr<AgentBrain> a) : agent(a) {}
                
                void recordStep(float reward, float curiosity, float novelty) {
                    steps_completed++;
                    total_reward += reward;
                    max_curiosity = std::max(max_curiosity, curiosity);
                    max_novelty = std::max(max_novelty, novelty);
                }
                
                float getAverageReward() const {
                    return steps_completed > 0 ? total_reward / steps_completed : 0.0f;
                }
                
                std::string getProgress() const {
                    return "Steps: " + std::to_string(steps_completed) +
                           ", Avg Reward: " + std::to_string(getAverageReward()) +
                           ", Max Curiosity: " + std::to_string(max_curiosity);
                }
            };
            return std::make_shared<AgentSession>(agent);
        }, "Create an agent session helper for tracking learning progress")
        .def("runEpisode", [](AgentBrain& agent, SimpleWorld& world, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                // Process sensory input
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                
                // This would normally be done by the brain in a real simulation
                // For this helper, we just return the state
                if (i % 100 == 0) {
                    agent.decodeMotorCommand();
                }
            }
        }, py::arg("world"), py::arg("steps"), "Run an episode of agent learning")
        .def("getNeuromodulationState", [](const AgentBrain& agent) {
            std::map<std::string, double> state;
            state["dopamine_level"] = agent.getNeuromodulationLevel();
            state["curiosity_level"] = agent.getCuriosityLevel();
            state["novelty_level"] = agent.getNoveltyLevel();
            state["prediction_error"] = agent.getPredictionError();
            return state;
        }, "Get complete neuromodulation state as dictionary")
        .def("resetLearningState", [](AgentBrain& agent) {
            agent.reset();
            // Reset all neuromodulation levels
            agent.enableRewardModulation(true);
            agent.enableCuriosity(true);
        }, "Reset agent learning state")

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
    
    // Add Pythonic snake_case aliases for existing methods
    m.def("runSimulation", [](std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           std::shared_ptr<Brain> brain,
                           double timestep) {
        // Update world
        world->update(timestep);
        
        // Get sensory percept and process
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(brain->getTotalSpikeCount(), timestep);
        
        // Get motor command and apply
        MotorCommand cmd = agent->decodeMotorCommand();
        world->applyMotorCommand(cmd, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = percept.getInternal()[0];
        if (world->getAgentBody().energy < 0.1f) {
            reward += 1.0f;  // Penalize low energy
        }
        agent->applyRewardModulation(reward, 0.0f);
        
        return reward;
    }, py::arg("world"), py::arg("agent"), py::arg("brain"), py::arg("timestep") = 0.1,
       R"pbdoc(
        Run a complete simulation step with world, agent, and brain.
        
        Updates the world, processes sensory input, steps the brain,
        decodes motor command, applies action to world, and applies reward modulation.
        
        Returns the reward for this step.
        "pbdoc")
    );
    
    // Add helper for creating world with obstacles
    m.def("createWorldWithObstacles", [](float width, float height, size_t vision_width, size_t vision_height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->reset();
        
        // Add random obstacles
        auto rng = std::make_shared<RandomGenerator>(42);
        
        for (int i = 0; i < 5; ++i) {
            float x = rng->uniformReal(2.0f, width - 2.0f);
            float y = rng->uniformReal(2.0f, height - 2.0f);
            
            if (rng->uniformReal(0.0f, 1.0f) < 0.6f) {
                // Wall
                WorldObject wall(x, y, WorldObjectType::Wall, 0.0f, 0.5f);
                world->addObject(wall);
            } else {
                // Hazard
                float damage = -rng->uniformReal(5.0f, 15.0f);
                WorldObject hazard(x, y, WorldObjectType::Hazard, damage);
                world->addObject(hazard);
            }
        }
        
        return world;
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f, 
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       R"pbdoc(
        Create a world with randomly placed obstacles.
        
        Returns a SimpleWorld with walls and hazards placed randomly.
        "pbdoc")
    );
    
    // Add helper for loading/saving agent brains
    m.def("saveAgentBrain", [](std::shared_ptr<AgentBrain> agent, const std::string& filepath) {
        auto brain = agent->getBrain();
        return brain->save(filepath);
    }, py::arg("agent"), py::arg("filepath"),
       R"pbdoc(
        Save agent brain and all connected systems to file.
        
        Includes the brain, its memory systems, and configuration.
        "pbdoc")
    );
    
    m.def("loadAgentBrain", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (brain->load(filepath)) {
            auto agent = std::make_shared<AgentBrain>(brain);
            return agent;
        } else {
            throw std::runtime_error("Failed to load agent brain from " + filepath);
        }
    }, py::arg("filepath"),
       R"pbdoc(
        Load agent brain and all connected systems from file.
        
        Returns an AgentBrain with loaded brain state and all systems.
        
        Raises RuntimeError if loading fails.
        "pbdoc")
    );
    
    // Add quick start helper for basic simulations
    m.def("quickStartSimulation", [](size_t steps = 100) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15.0f, 15.0f, 8, 8);
        world->reset();
        world->setAgentStart(7.5f, 7.5f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableAllLearning();
        
        std::vector<float> rewards;
        for (size_t i = 0; i < steps; ++i) {
            double timestep = 0.1;
            world->update(timestep);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(i, timestep * i);
            
            MotorCommand cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, world->getSimulationTime());
            
            float reward = percept.getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            rewards.push_back(reward);
            
            if (i % 20 == 0) {
                // Log progress
                std::cout << "Step " << i << ": "
                          << "Reward: " << reward << ", "
                          << "Firing neurons: " << brain->getFiringNeuronCount() << ", "
                          << "Curiosity: " << agent->getCuriosityLevel() << std::endl;
            }
        }
        
        return rewards;
    }, py::arg("steps") = 100,
       R"pbdoc(
        Quick start helper for running a complete simulation.
        
        Creates a default brain, world, and agent with all learning enabled,
        then runs the simulation for the specified number of steps.
        
        Returns a list of rewards collected during the simulation.
        "pbdoc")
    );

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

(End of file - total 884 lines)
