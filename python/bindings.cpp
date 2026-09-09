#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

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

    // NeuronId with enhanced Python interface
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
        })
        .def_static("create", [](uint64_t val) { return NeuronId(val); });

    // SynapseId with enhanced Python interface
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
        })
        .def_static("create", [](uint64_t val) { return SynapseId(val); });

    // RegionId with enhanced Python interface
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
        })
        .def_static("create", [](uint64_t val) { return RegionId(val); });

    // PopulationId with enhanced Python interface
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
        })
        .def_static("create", [](uint64_t val) { return PopulationId(val); });

    // NeuronType enumeration with better Python integration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_sensory", [](NeuronType type) { return type == NeuronType::Sensory; })
        .def("is_motor", [](NeuronType type) { return type == NeuronType::Motor; });

    // SynapseType enumeration with better Python integration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_excitatory", [](SynapseType type) { return type == SynapseType::Excitatory; })
        .def("is_inhibitory", [](SynapseType type) { return type == SynapseType::Inhibitory; });

    // DevelopmentalStage enumeration with better Python integration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_plastic", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod;
        })
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Maturation || stage == DevelopmentalStage::Adult;
        });

    // FiringState enumeration with better Python integration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("is_active", [](FiringState state) {
            return state == FiringState::Active || state == FiringState::Refractory;
        })
        .def("is_passive", [](FiringState state) {
            return state == FiringState::Resting || state == FiringState::Inhibited;
        });

    // ActionType enumeration with better Python integration
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
        .export_values()
        .def("is_movement", [](ActionType action) {
            return action == ActionType::MoveForward || action == ActionType::MoveBackward ||
                   action == ActionType::MoveLeft || action == ActionType::MoveRight;
        })
        .def("is_rotation", [](ActionType action) {
            return action == ActionType::TurnLeft || action == ActionType::TurnRight;
        })
        .def("is_interaction", [](ActionType action) {
            return action == ActionType::Interact || action == ActionType::Eat ||
                   action == ActionType::Drink || action == ActionType::Rest;
        });

    // MotorCommand enumeration with better Python integration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("is_movement", [](MotorCommand cmd) {
            return cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward;
        })
        .def("is_rotation", [](MotorCommand cmd) {
            return cmd == MotorCommand::TurnLeft || cmd == MotorCommand::TurnRight;
        })
        .def("is_interaction", [](MotorCommand cmd) {
            return cmd == MotorCommand::Interact;
        })
        .def("is_exploratory", [](MotorCommand cmd) {
            return cmd == MotorCommand::LookLeft || cmd == MotorCommand::LookRight;
        });

    // WorldObjectType enumeration with better Python integration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("is_passive", [](WorldObjectType type) {
            return type == WorldObjectType::Empty || type == WorldObjectType::Marker;
        })
        .def("is_active", [](WorldObjectType type) {
            return type == WorldObjectType::Resource || type == WorldObjectType::Hazard;
        })
        .def("is_hazard", [](WorldObjectType type) {
            return type == WorldObjectType::Hazard;
        })
        .def("is_resource", [](WorldObjectType type) {
            return type == WorldObjectType::Resource;
        });

    // Config class with enhanced Python interface
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
        .def("get", &Config::get<uint64_t>, py::arg("key"),
             "Get a configuration value")
        .def("get", [](const Config& self, const std::string& key, float defaultValue) {
            auto opt = self.get<float>(key);
            return opt ? *opt : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get float configuration value")
        .def("get", [](const Config& self, const std::string& key, bool defaultValue) {
            auto opt = self.get<bool>(key);
            return opt ? *opt : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get bool configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("copy", [](const Config& cfg) {
            return Config(cfg); // Assuming Config has copy constructor
        }, "Create a copy of the configuration");

    // SensoryInput base class with enhanced Python interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& input) {
            return py::dict(
                "type", input.getType(),
                "data", input.getData(),
                "dimensions", input.getDimensions(),
                "timestamp", input.getTimestamp()
            );
        }, "Convert sensory input to dictionary");

    // Vision class with enhanced Python interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataWithShape", [](Vision& self, const std::vector<std::vector<float>>& data) {
            std::vector<float> flatData;
            for (const auto& row : data) {
                flatData.insert(flatData.end(), row.begin(), row.end());
            }
            self.setData(flatData);
        }, py::arg("data"), "Set data with 2D shape")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("reshape", [](Vision& self, size_t newWidth, size_t newHeight) {
            // This would require additional implementation in Vision class
            // For now, just provide a wrapper
            return py::none();
        }, py::arg("newWidth"), py::arg("newHeight"), "Reshape vision data")
        .def("normalize", [](Vision& self, float min, float max) {
            // This would require additional implementation in Vision class
            return py::none();
        }, py::arg("min"), py::arg("max"), "Normalize vision data to range [min, max]");

    // Audio class with enhanced Python interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& audio) {
            return audio.getNumSamples() / static_cast<float>(audio.getSampleRate());
        }, "Get audio duration in seconds")
        .def("resample", [](Audio& self, size_t newSampleRate) {
            // This would require additional implementation in Audio class
            return py::none();
        }, py::arg("newSampleRate"), "Resample audio to new sample rate");

    // InternalSignals class with enhanced Python interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("setSignal", [](InternalSignals& self, size_t index, float value) {
            while (self.getData().size() <= index) {
                self.addSignal(0.0f);
            }
            auto data = self.getData();
            data[index] = value;
            // This would require additional methods in InternalSignals class
            // For now, just return None
            return py::none();
        }, py::arg("index"), py::arg("value"), "Set signal value at index")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            const auto& data = self.getData();
            if (index < data.size()) {
                return data[index];
            }
            return 0.0f;
        }, py::arg("index"), "Get signal value at index")
        .def("getSignalNames", []() {
            return std::vector<std::string>{"energy", "health", "movement", "turning"};
        }, "Get standard signal names");

    // Action class with enhanced Python interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getDescription", [](const Action& action) {
            std::string typeName = std::to_string(static_cast<int>(action.getType()));
            return "Action of type " + typeName;
        }, "Get human-readable description")
        .def("clone", &Action::clone)
        .def("copy", &Action::clone, "Create a copy of this action")
        .def("is_complete", [](const Action& action) {
            return !action.getParameters().empty();
        }, "Check if action is complete with parameters")
        .def("to_dict", [](const Action& action) {
            return py::dict(
                "type", action.getType(),
                "parameters", action.getParameters(),
                "name", action.getName()
            );
        }, "Convert action to dictionary");

    // WorldObject class with enhanced Python interface
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
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("is_near", [](const WorldObject& obj, float x, float y, float distance) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return (dx*dx + dy*dy) < distance * distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if object is within distance")
        .def("get_id", []() -> uint64_t { return std::hash<WorldObject>{}; }, "Get unique ID for object")
        .def("to_dict", [](const WorldObject& obj) {
            return py::dict(
                "x", obj.x,
                "y", obj.y,
                "radius", obj.radius,
                "type", obj.type,
                "value", obj.value,
                "active", obj.active
            );
        }, "Convert world object to dictionary");

    // AgentBody class with enhanced Python interface
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
        .def("distance_to", [](const AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("angle_to", [](const AgentBody& body, float x, float y) {
            float dx = x - body.x;
            float dy = y - body.y;
            float angle = std::atan2(dy, dx);
            while (angle < -M_PI) angle += 2.0f * M_PI;
            while (angle > M_PI) angle -= 2.0f * M_PI;
            return angle;
        }, py::arg("x"), py::arg("y"), "Calculate angle to position (x, y)")
        .def("speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX*body.velocityX + body.velocityY*body.velocityY);
        }, "Get current speed")
        .def("is_alive", [](const AgentBody& body) {
            return body.health > 0.0f && body.energy > 0.0f;
        }, "Check if agent is alive and active")
        .def("to_dict", [](const AgentBody& body) {
            return py::dict(
                "x", body.x,
                "y", body.y,
                "orientation", body.orientation,
                "velocityX", body.velocityX,
                "velocityY", body.velocityY,
                "angularVelocity", body.angularVelocity,
                "energy", body.energy,
                "health", body.health,
                "age", body.age,
                "isMoving", body.isMoving,
                "isTurning", body.isTurning,
                "lastActionTime", body.lastActionTime
            );
        }, "Convert agent body to dictionary")
        .def_static("create", [](float x, float y, float orientation) {
            AgentBody body;
            body.x = x;
            body.y = y;
            body.orientation = orientation;
            return body;
        }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f, "Create agent body from parameters");

    // ActionResult class with enhanced Python interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_good", [](const ActionResult& result) {
            return result.success && result.reward >= 0.0f;
        }, "Check if action result is positive")
        .def("is_bad", [](const ActionResult& result) {
            return result.success && result.reward < 0.0f;
        }, "Check if action result is negative")
        .def("to_dict", [](const ActionResult& result) {
            return py::dict(
                "reward", result.reward,
                "success", result.success,
                "message", result.message
            );
        }, "Convert action result to dictionary");

    // SensoryPercept class with enhanced Python interface
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
        .def("add_all_signals", [](SensoryPercept& percept, const std::vector<float>& vision,
                                   const std::vector<float>& touch,
                                   const std::vector<float>& internal,
                                   const std::vector<float>& proprioception,
                                   const std::vector<float>& audio) {
            percept.setVision(vision);
            percept.setTouch(touch);
            percept.setInternal(internal);
            percept.setProprioception(proprioception);
            percept.setAudio(audio);
        }, py::arg("vision"), py::arg("touch"), py::arg("internal"),
           py::arg("proprioception"), py::arg("audio"), "Add all sensory signals at once")
        .def("is_similar", [](const SensoryPercept& percept, const SensoryPercept& other, float threshold) {
            const auto& vision1 = percept.getVision();
            const auto& vision2 = other.getVision();
            if (vision1.size() != vision2.size()) return false;
            float maxDiff = 0.0f;
            for (size_t i = 0; i < vision1.size(); ++i) {
                maxDiff = std::max(maxDiff, std::abs(vision1[i] - vision2[i]));
            }
            return maxDiff < threshold;
        }, py::arg("other"), py::arg("threshold"), "Check if percept is similar to another")
        .def("to_dict", [](const SensoryPercept& percept) {
            return py::dict(
                "vision", percept.getVision(),
                "vision_width", percept.getVisionWidth(),
                "vision_height", percept.getVisionHeight(),
                "touch", percept.getTouch(),
                "internal", percept.getInternal(),
                "proprioception", percept.getProprioception(),
                "audio", percept.getAudio(),
                "timestamp", percept.getTimestamp()
            );
        }, "Convert sensory percept to dictionary")
        .def_static("from_dict", [](py::dict d) {
            SensoryPercept percept;
            if (d.contains("vision")) percept.setVision(d["vision"].cast<std::vector<float>>());
            if (d.contains("touch")) percept.setTouch(d["touch"].cast<std::vector<float>>());
            if (d.contains("internal")) percept.setInternal(d["internal"].cast<std::vector<float>>());
            if (d.contains("proprioception")) percept.setProprioception(d["proprioception"].cast<std::vector<float>>());
            if (d.contains("audio")) percept.setAudio(d["audio"].cast<std::vector<float>>());
            if (d.contains("timestamp")) percept.setTimestamp(d["timestamp"].cast<double>());
            return percept;
        }, py::arg("d"), "Create sensory percept from dictionary");

    // SimpleWorld class with enhanced Python interface
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("is_position_valid", [](SimpleWorld& world, float x, float y) {
            return world.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("get_object_at", [](SimpleWorld& world, float x, float y) {
            return world.getObjectAt(x, y);
        }, py::arg("x"), py::arg("y"), "Get object at position (returns None if none)")
        .def("shoot_ray", [](SimpleWorld& world, float x, float y, float angle, float maxDistance) {
            // Implementation would need to be added to SimpleWorld class
            return std::vector<float>{};  // Placeholder
        }, py::arg("x"), py::arg("y"), py::arg("angle"), py::arg("maxDistance"), "Shoot a ray from position at angle")
        .def("get_vision_info", [](SimpleWorld& world) {
            return py::dict(
                "width", world.getWidth(),
                "height", world.getHeight(),
                "vision_width", static_cast<size_t>(world.getSensoryPercept().getVisionWidth()),
                "vision_height", static_cast<size_t>(world.getSensoryPercept().getVisionHeight())
            );
        }, "Get vision configuration info")
        .def("to_dict", [](SimpleWorld& world) {
            return py::dict(
                "width", world.getWidth(),
                "height", world.getHeight(),
                "agent", py::dict(
                    "x", world.getAgentBody().x,
                    "y", world.getAgentBody().y,
                    "orientation", world.getAgentBody().orientation,
                    "energy", world.getAgentBody().energy
                ),
                "simulation_time", world.getSimulationTime(),
                "max_energy", world.getMaxEnergy()
            );
        }, "Convert world to dictionary")
        .def_static("create_with_size", [](float width, float height, size_t visionWidth, size_t visionHeight) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            return world;
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Create world with specific dimensions");

    // Brain class with enhanced Python interface
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
        .def("get_working_memory", [](Brain& brain) {
            return brain.getWorkingMemory();
        }, "Get working memory system")
        .def("get_episodic_memory", [](Brain& brain) {
            return brain.getEpisodicMemory();
        }, "Get episodic memory system")
        .def("get_associative_memory", [](Brain& brain) {
            return brain.getAssociativeMemory();
        }, "Get associative memory system")
        .def("get_prediction_system", [](Brain& brain) {
            return brain.getPredictionSystem();
        }, "Get prediction system")
        .def("get_planner", [](Brain& brain) {
            return brain.getPlanner();
        }, "Get neural planner")
        .def("get_concept_formation", [](Brain& brain) {
            return brain.getConceptFormation();
        }, "Get concept formation system")
        .def("get_attention", [](Brain& brain) {
            return brain.getAttention();
        }, "Get attentional selection system")
        .def("get_development_system", [](Brain& brain) {
            return brain.getDevelopmentSystem();
        }, "Get development system")
        .def("get_dopamine", [](Brain& brain) {
            return brain.getDopamine();
        }, "Get dopamine neuromodulator")
        .def("get_curiosity", [](Brain& brain) {
            return brain.getCuriosity();
        }, "Get curiosity neuromodulator")
        .def("get_novelty", [](Brain& brain) {
            return brain.getNovelty();
        }, "Get novelty neuromodulator")
        .def("get_prediction_error", [](Brain& brain) {
            return brain.getPredictionErrorSignal();
        }, "Get prediction error signal")
        .def("get_random_generator", [](Brain& brain) {
            return brain.getRandomGenerator();
        }, "Get random number generator")
        .def("get_stdp", [](Brain& brain) {
            return brain.getSTDP();
        }, "Get STDP plasticity rule")
        .def("get_hebbian", [](Brain& brain) {
            return brain.getHebbian();
        }, "Get Hebbian plasticity rule")
        .def("get_structural_plasticity", [](Brain& brain) {
            return brain.getStructuralPlasticity();
        }, "Get structural plasticity system")
        .def("get_spike_system", [](Brain& brain) {
            return brain.getSpikeSystem();
        }, "Get spike system")
        .def("get_excitation_inhibition_ratio", [](const Brain& brain) {
            return brain.getExcitationInhibitionRatio();
        }, "Get excitation/inhibition ratio")
        .def("get_total_spike_count", [](const Brain& brain) {
            return brain.getTotalSpikeCount();
        }, "Get total spike count")
        .def("get_firing_rate", [](const Brain& brain) {
            return brain.getAverageFiringRate();
        }, "Get average firing rate")
        .def("get_neuron_count", [](const Brain& brain) {
            return brain.getTotalNeuronCount();
        }, "Get total neuron count")
        .def("get_synapse_count", [](const Brain& brain) {
            return brain.getTotalSynapseCount();
        }, "Get total synapse count")
        .def("get_active_neuron_count", [](const Brain& brain) {
            return brain.getActiveNeuronCount();
        }, "Get active neuron count")
        .def("get_region_count", [](const Brain& brain) {
            return brain.getRegionCount();
        }, "Get region count")
        .def("add_inter_region_connection", [](Brain& brain, RegionId source, RegionId target, float weight, Delay delay) {
            brain.addInterRegionConnection(source, target, weight, delay);
        }, py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add inter-region connection")
        .def("remove_inter_region_connection", [](Brain& brain, RegionId source, RegionId target) {
            brain.removeInterRegionConnection(source, target);
        }, py::arg("source"), py::arg("target"), "Remove inter-region connection")
        .def_static("create", [](std::shared_ptr<Config> config) {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration");

    // AgentBrain class with enhanced Python interface
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
        .def("set_sensory_novelty_decay", [](AgentBrain& agent, float decay) {
            // Note: This would require a setter method in AgentBrain
            // For now, just return a placeholder
            return py::none();
        }, py::arg("decay"), "Set sensory novelty decay rate")
        .def("get_plasticity_modifier", [](const AgentBrain& agent) {
            return agent.getBrain()->getSTDP()->getLTPWeight();
        }, "Get current plasticity modifier")
        .def("get_developmental_age", [](const AgentBrain& agent) {
            return agent.getDevelopmentalStage(); // Note: This returns DevelopmentalStage, not age
        }, "Get developmental age (placeholder)")
        .def_static("create", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience factory functions
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

    // Version and utility constants
    m.attr("__version__") = "0.1.0";
    m.attr("VERSION") = "0.1.0";
    m.attr("BUILD_TYPE") = "Release";
    
    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Utility functions
    m.def("generate_random_seed", []() -> uint64_t {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }, "Generate a random seed from current time");
    
    m.def("validate_config", [](const Config& config) {
        auto keys = config.getKeys();
        return !keys.empty();
    }, py::arg("config"), "Validate configuration has keys");
    
    m.def("normalize_vector", [](const std::vector<float>& vec, float min, float max) {
        std::vector<float> result = vec;
        float range = max - min;
        for (auto& val : result) {
            val = (val - min) / range;
        }
        return result;
    }, py::arg("vec"), py::arg("min"), py::arg("max"), "Normalize vector to range [0, 1]");
    
    m.def("denormalize_vector", [](const std::vector<float>& vec, float min, float max) {
        std::vector<float> result = vec;
        float range = max - min;
        for (auto& val : result) {
            val = val * range + min;
        }
        return result;
    }, py::arg("vec"), py::arg("min"), py::arg("max"), "Denormalize vector from range [0, 1]");

    // Constants for common values
    m.attr("VISION_DEFAULT_WIDTH") = 16;
    m.attr("VISION_DEFAULT_HEIGHT") = 16;
    m.attr("WORLD_DEFAULT_WIDTH") = 20.0f;
    m.attr("WORLD_DEFAULT_HEIGHT") = 20.0f;
    m.attr("DEFAULT_MOTOR_COMMANDS_COUNT") = 6;
    m.attr("DEFAULT_SENSOR_INPUT_SIZE") = 274; // 256 + 8 + 4 + 6
    m.attr("MAX_DEVELOPMENTAL_STAGE") = 4;
    m.attr("DEFAULT_RNG_SEED") = 42;

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Type conversion helpers
    m.def("to_neuron_type", [](int value) {
        return static_cast<NeuronType>(value);
    }, py::arg("value"), "Convert integer to NeuronType");

    m.def("to_action_type", [](int value) {
        return static_cast<ActionType>(value);
    }, py::arg("value"), "Convert integer to ActionType");

    m.def("to_world_object_type", [](int value) {
        return static_cast<WorldObjectType>(value);
    }, py::arg("value"), "Convert integer to WorldObjectType");

    // Serialization utilities
    m.def("serialize_to_string", [](const Config& config) {
        return config.summary();
    }, py::arg("config"), "Serialize configuration to string");

    // Performance and optimization hints
    m.def("get_system_info", []() {
        return py::dict(
            "description", "NLM - Neural Learning Machine",
            "version", "0.1.0",
            "build_type", "Release",
            "cpp_standard", "C++20",
            "python_support", "3.8+"
        );
    }, "Get system information");

    // Error handling helpers
    m.def("check_error", [](bool condition, const std::string& message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }, py::arg("condition"), py::arg("message"), "Check condition and throw error if false");

    // Memory and resource management helpers
    m.def("cleanup", []() {
        // Placeholder for cleanup operations
        return true;
    }, "Clean up resources");

    // Educational and debugging helpers
    m.def("print_version", []() {
        std::cout << "NLM Python Bindings v0.1.0" << std::endl;
        std::cout << "Built with C++20" << std::endl;
        std::cout << "For documentation, see: https://nlm.readthedocs.io" << std::endl;
    }, "Print version information");

    m.def("examples", []() {
        return py::dict(
            "minimal_brain", R"(
                import pynlm
                
                # Create a simple brain
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Run a few steps
                for i in range(10):
                    brain.step(i)
                
                print(f"Total spikes: {brain.getTotalSpikeCount()}")
            )",
            "agent_example", R"(
                import pynlm
                
                # Setup complete agent with world interaction
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                world = pynlm.createSimpleWorld()
                world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
                world.reset()
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                
                # Run agent simulation
                for step in range(100):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                print(f"Simulation complete!")
                print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
            )",
            "memory_example", R"(
                import pynlm
                
                # Create brain with memory systems
                config = pynlm.createDefaultConfig()
                config.set("brain.neuron_count", 500)
                
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Check memory system status
                working_memory = brain.getWorkingMemory()
                episodic_memory = brain.getEpisodicMemory()
                
                print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
                print(f"Working memory capacity: {working_memory.getCapacity() if working_memory else 'N/A'}")
                print(f"Episodic memory max episodes: {episodic_memory.getMaxEpisodes() if episodic_memory else 'N/A'}")
            )"
        );
    }, "Get example code snippets");

} // namespace nlm

