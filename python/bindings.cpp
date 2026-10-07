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
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Enhanced SensoryInput methods
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input")
        .def("setType", [](SensoryInput& self, int type) {
            // Note: This assumes SensoryInput has a setType method
            // If not, this would need to be adjusted based on actual implementation
        }, py::arg("type"), "Set the type of sensory input (experimental)")
        .def("setData", [](SensoryInput& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set raw data (experimental)")
        .def("get_all_signals", [](const SensoryInput& self) {
            return self.getData();
        }, "Get all signals as vector (alias for getData)");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Enhanced Vision methods
        .def("get_resolution", [](const Vision& self) {
            return py::make_tuple(self.getWidth(), self.getHeight());
        }, "Get resolution as (width, height) tuple")
        .def("get_channel_info", [](const Vision& self) {
            std::string channels = "RGB";
            if (self.getChannels() == 1) channels = "Grayscale";
            else if (self.getChannels() == 3) channels = "RGB";
            else if (self.getChannels() == 4) channels = "RGBA";
            return channels;
        }, "Get channel info as string");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Enhanced Audio methods
        .def("get_duration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("get_audio_info", [](const Audio& self) {
            py::dict info;
            info["sample_rate"] = self.getSampleRate();
            info["num_samples"] = self.getNumSamples();
            info["duration_seconds"] = self.getNumSamples() / static_cast<float>(self.getSampleRate());
            return info;
        }, "Get audio information as dictionary");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Enhanced InternalSignals methods
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"), "Add a signal (alias)")
        .def("get_signal_count", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of signals")
        .def("get_signal_values", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all signal values");

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
        // Enhanced Action methods
        .def("get_action_name", &Action::getName, "Get action name (alias)")
        .def("get_parameters", &Action::getParameters, "Get action parameters (alias)")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters (alias)")
        .def("is_valid", [](const Action& self) {
            return !self.getName().empty();
        }, "Check if action is valid")
        .def("get_action_type", &Action::getType, "Get action type (alias)")
        .def("get_action_type_name", [](const Action& self) {
            std::string type_name;
            switch (self.getType()) {
                case ActionType::MoveForward: type_name = "MoveForward"; break;
                case ActionType::MoveBackward: type_name = "MoveBackward"; break;
                case ActionType::MoveLeft: type_name = "MoveLeft"; break;
                case ActionType::MoveRight: type_name = "MoveRight"; break;
                case ActionType::TurnLeft: type_name = "TurnLeft"; break;
                case ActionType::TurnRight: type_name = "TurnRight"; break;
                case ActionType::Look: type_name = "Look"; break;
                case ActionType::LookUp: type_name = "LookUp"; break;
                case ActionType::LookDown: type_name = "LookDown"; break;
                case ActionType::Interact: type_name = "Interact"; break;
                case ActionType::Eat: type_name = "Eat"; break;
                case ActionType::Drink: type_name = "Drink"; break;
                case ActionType::Rest: type_name = "Rest"; break;
                case ActionType::Wait: type_name = "Wait"; break;
                case ActionType::Custom: type_name = "Custom"; break;
                default: type_name = "Unknown";
            }
            return type_name;
        }, "Get action type as string")
        .def("has_parameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters")
        .def("get_parameter_count", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of action parameters");

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
        // Enhanced WorldObject methods
        .def("get_type_name", [](const WorldObject& self) {
            std::string type_name;
            switch (self.type) {
                case WorldObjectType::Empty: type_name = "Empty"; break;
                case WorldObjectType::Resource: type_name = "Resource"; break;
                case WorldObjectType::Hazard: type_name = "Hazard"; break;
                case WorldObjectType::Wall: type_name = "Wall"; break;
                case WorldObjectType::Marker: type_name = "Marker"; break;
                default: type_name = "Unknown";
            }
            return type_name;
        }, "Get world object type as string")
        .def("is_interactive", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource || 
                   self.type == WorldObjectType::Marker;
        }, "Check if object is interactive")
        .def("get_object_info", [](const WorldObject& self) {
            py::dict info;
            info["x"] = self.x;
            info["y"] = self.y;
            info["radius"] = self.radius;
            info["type"] = self.get_type_name();
            info["value"] = self.value;
            info["active"] = self.active;
            return info;
        }, "Get object information as dictionary");

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
        // Enhanced AgentBody methods
        .def("get_position", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("get_velocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("get_rotation", [](const AgentBody& self) {
            return py::make_tuple(self.orientation, self.angularVelocity);
        }, "Get rotation as (orientation, angular_velocity) tuple")
        .def("get_status", [](const AgentBody& self) {
            py::dict status;
            status["x"] = self.x;
            status["y"] = self.y;
            status["orientation"] = self.orientation;
            status["velocity_x"] = self.velocityX;
            status["velocity_y"] = self.velocityY;
            status["angular_velocity"] = self.angularVelocity;
            status["energy"] = self.energy;
            status["health"] = self.health;
            status["age"] = self.age;
            status["is_moving"] = self.isMoving;
            status["is_turning"] = self.isTurning;
            status["last_action_time"] = self.lastActionTime;
            return status;
        }, "Get agent status as dictionary")
        .def("is_alive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive")
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Enhanced ActionResult methods
        .def("is_successful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("get_reward_description", [](const ActionResult& self) {
            if (self.reward > 0.0f) return "Positive reward";
            else if (self.reward < 0.0f) return "Negative reward";
            else return "No reward";
        }, "Get reward description")
        .def("get_result_info", [](const ActionResult& self) {
            py::dict info;
            info["reward"] = self.reward;
            info["success"] = self.success;
            info["message"] = self.message;
            info["is_successful"] = self.success;
            info["reward_type"] = self.reward > 0.0f ? "positive" : 
                                 (self.reward < 0.0f ? "negative" : "neutral");
            return info;
        }, "Get action result information as dictionary");

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
        // Enhanced SensoryPercept methods
        .def("get_vision", &SensoryPercept::getVision, "Get vision sensory input (alias)")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch sensory input (alias)")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal sensory input (alias)")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception input (alias)")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio sensory input (alias)")
        .def("has_vision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is available")
        .def("has_touch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if touch data is available")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal data is available")
        .def("get_signals_summary", [](const SensoryPercept& self) {
            py::dict summary;
            summary["has_vision"] = !self.getVision().empty();
            summary["has_touch"] = !self.getTouch().empty();
            summary["has_internal"] = !self.getInternal().empty();
            summary["has_audio"] = !self.getAudio().empty();
            summary["total_signals"] = self.getAllSignals().size();
            return summary;
        }, "Get sensory signals summary");

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
        // Enhanced SimpleWorld methods
        .def("run_simulation_step", &SimpleWorld::update, py::arg("timestep"), "Run a single simulation step (alias)")
        .def("execute_action", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Execute a motor command (alias)")
        .def("get_environment_summary", [](SimpleWorld& world) {
            py::dict summary;
            summary["width"] = world.getWidth();
            summary["height"] = world.getHeight();
            summary["max_energy"] = world.getMaxEnergy();
            summary["energy_decay_rate"] = world.getEnergyDecayRate();
            summary["simulation_time"] = world.getSimulationTime();
            summary["random_seed"] = world.getRandomSeed();
            return summary;
        }, "Get environment configuration summary")
        .def("get_agent_status", [](SimpleWorld& world) {
            auto body = world.getAgentBody();
            py::dict status;
            status["x"] = body->x;
            status["y"] = body->y;
            status["orientation"] = body->orientation;
            status["energy"] = body->energy;
            status["health"] = body->health;
            status["is_moving"] = body->isMoving;
            status["is_turning"] = body->isTurning;
            return status;
        }, "Get current agent status")
        .def("add_resource", [](SimpleWorld& world, float x, float y, float value) {
            py::object resource_obj = py::cast(WorldObject(x, y, WorldObjectType::Resource, value));
            world.addObject(resource_obj.cast<WorldObject&>());
        }, "Add a resource object to the world")
        .def("add_hazard", [](SimpleWorld& world, float x, float y, float value) {
            py::object hazard_obj = py::cast(WorldObject(x, y, WorldObjectType::Hazard, value));
            world.addObject(hazard_obj.cast<WorldObject&>());
        }, "Add a hazard object to the world")
        .def("get_nearby_resources", [](SimpleWorld& world, float x, float y, float radius) {
            py::list resources;
            // This is a simplified implementation - actual implementation would check world objects
            // For now, just returns empty list as the actual implementation would be more complex
            return resources;
        }, "Get resources near position (experimental)")
        .def("is_safe_position", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid/safe");

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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // System accessors
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system accessor")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity accessor")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity accessor")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity accessor")
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory accessor")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory accessor")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory accessor")
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system accessor")
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner accessor")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation accessor")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection accessor")
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system accessor")
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator accessor")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator accessor")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator accessor")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error neuromodulator accessor")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator accessor")
        // Inter-region connection management
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add connection between regions")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove connection between regions")
        // Enhanced statistics
        .def("get_system_summary", [](const Brain& brain) {
            py::dict summary;
            summary["total_neurons"] = brain.getTotalNeuronCount();
            summary["total_synapses"] = brain.getTotalSynapseCount();
            summary["active_neurons"] = brain.getActiveNeuronCount();
            summary["firing_neurons"] = brain.getFiringNeuronCount();
            summary["total_spikes"] = brain.getTotalSpikeCount();
            summary["pending_spikes"] = brain.getPendingSpikeEventCount();
            summary["firing_rate"] = brain.getAverageFiringRate();
            summary["ei_ratio"] = brain.getExcitationInhibitionRatio();
            summary["developmental_stage"] = brain.getDevelopmentalStage();
            return summary;
        }, "Get comprehensive system summary")
        .def("get_activity_summary", [](const Brain& brain) {
            py::dict activity;
            activity["total_neurons"] = brain.getTotalNeuronCount();
            activity["firing_neurons"] = brain.getFiringNeuronCount();
            activity["active_neurons"] = brain.getActiveNeuronCount();
            activity["total_spikes"] = brain.getTotalSpikeCount();
            activity["pending_spikes"] = brain.getPendingSpikeEventCount();
            activity["firing_rate"] = brain.getAverageFiringRate();
            return activity;
        }, "Get neural activity summary")
        .def("get_neuromodulation_state", [](Brain& brain) {
            py::dict neuromod;
            if (auto* dopamine = brain.getDopamine()) {
                neuromod["dopamine"] = dopamine->getModulationLevel();
            }
            if (auto* curiosity = brain.getCuriosity()) {
                neuromod["curiosity"] = curiosity->getCuriosityLevel();
                neuromod["novelty"] = curiosity->getNoveltyLevel();
            }
            if (auto* novelty = brain.getNovelty()) {
                neuromod["novelty_detection"] = novelty->isNoveltyDetected();
            }
            if (auto* pred_error = brain.getPredictionErrorSignal()) {
                neuromod["prediction_error"] = pred_error->getErrorLevel();
            }
            return neuromod;
        }, "Get current neuromodulation state")
        .def("get_development_state", [](Brain& brain) {
            py::dict dev;
            if (auto* dev_sys = brain.getDevelopmentSystem()) {
                dev["stage"] = brain.getDevelopmentalStage();
                dev["age"] = dev_sys->getDevelopmentAge();
                dev["maturation"] = dev_sys->getMaturationProgress();
                dev["critical_period"] = dev_sys->isCriticalPeriod();
            }
            return dev;
        }, "Get development system state")
        .def("get_plasticity_summary", [](Brain& brain) {
            py::dict plastic;
            if (auto* stdp = brain.getSTDP()) {
                plastic["stdp_enabled"] = stdp->isEnabled();
            }
            if (auto* hebbian = brain.getHebbian()) {
                plastic["hebbian_enabled"] = hebbian->isEnabled();
            }
            if (auto* struct_plastic = brain.getStructuralPlasticity()) {
                plastic["structural_enabled"] = struct_plastic->isEnabled();
            }
            return plastic;
        }, "Get plasticity system state");

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
        // Enhanced AgentBrain methods
        .def("get_expected_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_expected_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("get_sensory_signature", [](AgentBrain& agent) {
            return py::make_tuple(agent.getSensoryInputSize(), agent.getMotorOutputSize());
        }, "Get expected input/output sizes as tuple")
        .def("get_motor_command_names", []() {
            std::vector<std::string> names;
            for (int i = 0; i < static_cast<int>(MotorCommand::Wait); ++i) {
                names.push_back(std::to_string(static_cast<MotorCommand>(i)));
            }
            return names;
        }, "Get list of available motor command names")
        .def("get_neuromodulation_levels", [](AgentBrain& agent) {
            py::dict levels;
            levels["curiosity"] = agent.getCuriosityLevel();
            levels["novelty"] = agent.getNoveltyLevel();
            levels["prediction_error"] = agent.getPredictionError();
            levels["neuromodulation"] = agent.getNeuromodulationLevel();
            return levels;
        }, "Get all neuromodulation levels as dict")
        .def("get_development_summary", [](AgentBrain& agent) {
            py::dict dev;
            dev["stage"] = agent.getDevelopmentalStage();
            dev["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            dev["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            dev["development_enabled"] = agent.isDevelopmentEnabled();
            dev["curiosity_enabled"] = agent.isCuriosityEnabled();
            return dev;
        }, "Get development system summary")
        .def("is_ready_for_input", [](AgentBrain& agent) {
            return agent.getSensoryInputSize() > 0 && agent.getMotorOutputSize() > 0;
        }, "Check if agent is ready for sensory input");

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

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience configuration helpers
    m.def("create_brain_with_config", [](const std::unordered_map<std::string, ConfigValue>& config_map) {
        auto config = std::make_shared<Config>();
        for (const auto& pair : config_map) {
            config->set(pair.first, pair.second, ConfigSource::Runtime);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config_map"), "Create brain with configuration map");

    m.def("validate_config", [](std::shared_ptr<Config> config, const std::unordered_map<std::string, std::string>& type_hints) {
        bool is_valid = true;
        for (const auto& pair : type_hints) {
            if (config->has(pair.first)) {
                auto value = config->get(pair.first);
                // Simple type checking - could be expanded
                if (pair.second == "int" && !std::holds_alternative<int>(value)) {
                    is_valid = false;
                } else if (pair.second == "float" && !std::holds_alternative<double>(value)) {
                    is_valid = false;
                } else if (pair.second == "string" && !std::holds_alternative<std::string>(value)) {
                    is_valid = false;
                } else if (pair.second == "bool" && !std::holds_alternative<bool>(value)) {
                    is_valid = false;
                }
            }
        }
        return is_valid;
    }, "Validate configuration against type hints");

    // Configuration builders
    m.def("build_brain_config", [](int neuron_count, float connection_probability, bool enable_plasticity) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("brain.connection_probability", connection_probability, ConfigSource::Default);
        config->set("plasticity.stdp.enable", enable_plasticity, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", enable_plasticity, ConfigSource::Default);
        return config;
    }, "Build a simple brain configuration");

    m.def("build_agent_config", [](bool enable_reward_modulation, bool enable_curiosity, 
                                   bool enable_development, bool enable_structural_plasticity) {
        auto config = std::make_shared<Config>();
        config->set("neuromod.curiosity.enable", enable_curiosity, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", enable_reward_modulation ? 1.0f : 0.0f, ConfigSource::Default);
        config->set("development.enable", enable_development, ConfigSource::Default);
        config->set("plasticity.structural.enable", enable_structural_plasticity, ConfigSource::Default);
        return config;
    }, "Build an agent configuration");

    // API discovery helpers
    m.def("list_available_components", []() {
        py::list components;
        // Add major classes
        components.append("Brain");
        components.append("AgentBrain");
        components.append("SimpleWorld");
        components.append("Config");
        components.append("Vision");
        components.append("Audio");
        components.append("InternalSignals");
        components.append("SensoryInput");
        components.append("Action");
        components.append("WorldObject");
        components.append("AgentBody");
        components.append("ActionResult");
        components.append("SensoryPercept");
        return components;
    }, "List all available Python components");

    m.def("get_api_documentation", [](const std::string& component) {
        py::dict doc;
        if (component == "Brain") {
            doc["description"] = "Central neural simulation brain class";
            doc["methods"] = py::list({"initialize", "step", "receiveSensoryInput", "produceAction",
                                      "reset", "save", "load", "addRegion", "getRegion",
                                      "getTotalNeuronCount", "getTotalSynapseCount", "getFiringNeuronCount",
                                      "getAverageFiringRate", "getTotalSpikeCount", "getDevelopmentalStage"});
        } else if (component == "AgentBrain") {
            doc["description"] = "Agent brain interface connecting NLM brain to world";
            doc["methods"] = py::list({"initialize", "processSensoryInput", "decodeMotorCommand",
                                      "applyRewardModulation", "updateDevelopment", "getDevelopmentalStage"});
        } else if (component == "SimpleWorld") {
            doc["description"] = "Simple 2D world for NLM simulation";
            doc["methods"] = py::list({"configure", "reset", "update", "applyMotorCommand",
                                      "getSensoryPercept", "getAgentBody", "addObject", "removeObject"});
        }
        return doc;
    }, "Get API documentation for a component");

    m.def("show_examples", [](const std::string& pattern) {
        if (pattern == "agent") {
            return R"(
import pynlm

# Create and configure agent
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete! Firing neurons: {brain.getFiringNeuronCount()}")
            )";
        } else if (pattern == "basic") {
            return R"(
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate for 100 steps
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
            )";
        }
        return "No examples found for pattern: " + pattern;
    }, "Show example code for common patterns");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
