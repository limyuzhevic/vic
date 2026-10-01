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
        .def("has", py::overload_cast<const std::string&>(&Config::has), py::arg("key"),
             "Check if a configuration key exists")
        .def("has", py::overload_cast<const char*>(&Config::has), py::arg("key"),
             "Check if a configuration key exists (C string)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            // Try to get int
            if (auto val = self.get<int64_t>(key)) {
                return py::cast(*val);
            }
            // Try to get float
            if (auto val = self.get<float>(key)) {
                return py::cast(*val);
            }
            // Try to get string
            if (auto val = self.get<std::string>(key)) {
                return py::cast(*val);
            }
            // Try to get bool
            if (auto val = self.get<bool>(key)) {
                return py::cast(*val);
            }
            // Return None if not found
            return py::none();
        }, py::arg("key"), "Get configuration value (auto-detects type)")
        .def("setInt", &Config::set<int64_t>, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("setFloat", &Config::set<float>, py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("setString", &Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set a string configuration value")
        .def("setBool", &Config::set<bool>, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("setDouble", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set a double configuration value")
        .def("getNeuronCount", [](Config& self) -> int {
            return self.getOr<int>("neuron_count", 1000);
        }, "Get the number of neurons")
        .def("setNeuronCount", [](Config& self, int count) {
            self.set("neuron_count", count);
        }, py::arg("count"), "Set the number of neurons")
        .def("getSynapseDensity", [](Config& self) -> float {
            return self.getOr<float>("synapse_density", 0.1f);
        }, "Get the synapse density")
        .def("setSynapseDensity", [](Config& self, float density) {
            self.set("synapse_density", density);
        }, py::arg("density"), "Set the synapse density")
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
        .def("printInfo", [](const SensoryInput& self) {
            std::cout << "SensoryInput type: " << static_cast<int>(self.getType())
                      << " dims: " << self.getDimensions()
                      << " timestamp: " << self.getTimestamp() << std::endl;
        }, "Print sensory input information");

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
        .def("clone", &Action::clone)
        .def("is", [](const Action& self, ActionType type) {
            return self.getType() == type;
        }, py::arg("type"), "Check if action is of specified type")
        .def("getNameForType", [](ActionType type) {
            switch (type) {
                case ActionType::MoveForward: return "MoveForward";
                case ActionType::MoveBackward: return "MoveBackward";
                case ActionType::MoveLeft: return "MoveLeft";
                case ActionType::MoveRight: return "MoveRight";
                case ActionType::TurnLeft: return "TurnLeft";
                case ActionType::TurnRight: return "TurnRight";
                case ActionType::Look: return "Look";
                case ActionType::LookUp: return "LookUp";
                case ActionType::LookDown: return "LookDown";
                case ActionType::Interact: return "Interact";
                case ActionType::Eat: return "Eat";
                case ActionType::Drink: return "Drink";
                case ActionType::Rest: return "Rest";
                case ActionType::Wait: return "Wait";
                case ActionType::Custom: return "Custom";
                default: return "Unknown";
            }
        }, py::arg("type"), "Get name for action type");

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
        .def("isResource", [](WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if world object is a resource")
        .def("isHazard", [](WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if world object is a hazard")
        .def("isWall", [](WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if world object is a wall")
        .def("getDistanceTo", [](WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to point")
        .def("canInteract", [](WorldObject& self, float x, float y, float range) {
            float dx = self.x - x;
            float dy = self.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            return dist <= (self.radius + range);
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is within interaction range")
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName;
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
                default: typeName = "Unknown";
            }
            return "<WorldObject: " + typeName + " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ") value=" + std::to_string(obj.value) + ">";
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
        .def("getPosition", [](AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getSpeed", [](AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get agent speed")
        .def("getHeading", [](AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent heading in radians")
        .def("isAlive", [](AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive")
        .def("getMaxEnergy", [](AgentBody& self, float maxEnergy) {
            // Use maxEnergy as a parameter since AgentBody doesn't store max
            self.energy = std::min(self.energy, maxEnergy);
        }, py::arg("maxEnergy"), "Cap agent energy to maximum")
        .def("applyDamage", [](AgentBody& self, float damage) {
            self.health = std::max(0.0f, self.health - damage);
        }, py::arg("damage"), "Apply damage to agent")
        .def("heal", [](AgentBody& self, float amount) {
            self.health = std::min(100.0f, self.health + amount);
        }, py::arg("amount"), "Heal agent");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("getFormattedReward", [](const ActionResult& self) {
            std::stringstream ss;
            ss << (self.reward >= 0 ? "+" : "") << std::fixed << std::setprecision(2) << self.reward;
            return ss.str();
        }, "Get formatted reward string")
        .def("toString", [](const ActionResult& self) {
            std::stringstream ss;
            ss << "ActionResult: " << (self.success ? "SUCCESS" : "FAILURE") << std::endl;
            ss << "  Reward: " << self.reward << std::endl;
            ss << "  Message: " << self.message << std::endl;
            return ss.str();
        }, "Get string representation of action result");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")

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
        .def("getVisionWidth", &SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &SimpleWorld::getVisionHeight)
        .def("getAgent", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("setObjectDensity", [](SimpleWorld& self, float resourceDensity, float hazardDensity) {
            // This would require modifying the world - for now just documentation
            // In practice, you'd need to implement object generation with configurable densities
        }, py::arg("resourceDensity"), py::arg("hazardDensity"), "Set object density (conceptual)")
        .def("getObjectCount", [](SimpleWorld& self) {
            return static_cast<int>(self.getObjectCount());
        }, "Get number of objects in world (conceptual)")
        .def("toString", [](SimpleWorld& self) {
            std::stringstream ss;
            ss << "SimpleWorld:" << std::endl;
            ss << "  Dimensions: " << self.getWidth() << " x " << self.getHeight() << std::endl;
            ss << "  Agent position: (" << self.getAgent().x << ", " << self.getAgent().y << ")" << std::endl;
            ss << "  Simulation time: " << self.getSimulationTime() << std::endl;
            ss << "  Max energy: " << self.getMaxEnergy() << std::endl;
            ss << "  Energy decay rate: " << self.getEnergyDecayRate() << std::endl;
            return ss.str();
        }, "Get world information as string");

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
        .def("isInitialized", [](Brain& self) {
            return self.getTotalNeuronCount() > 0;
        }, "Check if brain is initialized")
        .def("getNeuronDensity", [](Brain& self) {
            float totalNeurons = static_cast<float>(self.getTotalNeuronCount());
            return totalNeurons / 1000.0f; // Normalized density
        }, "Get normalized neuron density")
        .def("getSynapticComplexity", [](Brain& self) {
            float totalSynapses = static_cast<float>(self.getTotalSynapseCount());
            float totalNeurons = static_cast<float>(self.getTotalNeuronCount());
            return totalNeurons > 0 ? totalSynapses / totalNeurons : 0.0f;
        }, "Get synaptic complexity (synapses per neuron)")
        .def("getActivityLevel", [](Brain& self) {
            return self.getAverageFiringRate() / 100.0f; // Normalized activity
        }, "Get normalized activity level")
        .def("printState", &Brain::logStatus,
             "Print brain state summary (alias for logStatus)")
        .def("__str__", [](Brain& self) {
            std::stringstream ss;
            ss << "Brain State:" << std::endl;
            ss << "  Neurons: " << self.getTotalNeuronCount() << std::endl;
            ss << "  Synapses: " << self.getTotalSynapseCount() << std::endl;
            ss << "  Active neurons: " << self.getActiveNeuronCount() << std::endl;
            ss << "  Firing neurons: " << self.getFiringNeuronCount() << std::endl;
            ss << "  Total spikes: " << self.getTotalSpikeCount() << std::endl;
            ss << "  Developmental stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            return ss.str();
        }, "Get string representation of brain state");

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
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning features")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning features")
        .def("getFeatureStatus", [](AgentBrain& self) {
            return py::make_dict(
                "rewardModulation", self.isRewardModulationEnabled(),
                "structuralPlasticity", self.isStructuralPlasticityEnabled(),
                "development", self.isDevelopmentEnabled(),
                "curiosity", self.isCuriosityEnabled()
            );
        }, "Get dictionary of feature status")
        .def("getNeuromodulationStatus", [](AgentBrain& self) {
            return py::make_dict(
                "dopamine", self.getNeuromodulationLevel(),
                "curiosity", self.getCuriosityLevel(),
                "novelty", self.getNoveltyLevel(),
                "predictionError", self.getPredictionError()
            );
        }, "Get dictionary of neuromodulation levels")
        .def("getDevelopmentStatus", [](AgentBrain& self) {
            return py::make_dict(
                "stage", static_cast<int>(self.getDevelopmentalStage()),
                "stageName", []() {
                    // Map developmental stage to string
                    py::dict stageNames = {
                        {static_cast<int>(DevelopmentalStage::Initial), "Initial"},
                        {static_cast<int>(DevelopmentalStage::CriticalPeriod), "CriticalPeriod"},
                        {static_cast<int>(DevelopmentalStage::Maturation), "Maturation"},
                        {static_cast<int>(DevelopmentalStage::Adult), "Adult"},
                        {static_cast<int>(DevelopmentalStage::Aging), "Aging"}
                    };
                    return stageNames;
                }(),
                "description", "Brain developmental stage"
            );
        }, "Get dictionary of development status")
        .def("getSensoryInfo", [](AgentBrain& self) {
            return py::make_dict(
                "inputSize", self.getSensoryInputSize(),
                "description", "Expected size of sensory input vectors"
            );
        }, "Get sensory system information")
        .def("getMotorInfo", [](AgentBrain& self) {
            return py::make_dict(
                "outputSize", self.getMotorOutputSize(),
                "possibleCommands", [
                    MotorCommand::MoveForward,
                    MotorCommand::MoveBackward,
                    MotorCommand::TurnLeft,
                    MotorCommand::TurnRight,
                    MotorCommand::LookLeft,
                    MotorCommand::LookRight,
                    MotorCommand::Interact,
                    MotorCommand::Wait
                ],
                "description", "Motor command output size and available commands"
            );
        }, "Get motor system information")
        .def("processVisionInput", [](AgentBrain& self, const std::vector<float>& vision) {
            // Create a temporary Vision object (conceptual - would need actual implementation)
            // This is a convenience method for common vision processing
            if (self.getSensoryInputSize() >= 256) {
                // Assume vision is the first 256 values of input
                SensoryPercept percept;
                percept.setVision(vision);
                self.processSensoryInput(percept);
                return true;
            }
            return false;
        }, py::arg("vision"), "Process vision input (convenience method)")
        .def("processTouchInput", [](AgentBrain& self, const std::vector<float>& touch) {
            // Process touch input (convenience method)
            SensoryPercept percept;
            percept.setTouch(touch);
            self.processSensoryInput(percept);
        }, py::arg("touch"), "Process touch input (convenience method)")
        .def("processInternalInput", [](AgentBrain& self, const std::vector<float>& internal) {
            // Process internal input (convenience method)
            SensoryPercept percept;
            percept.setInternal(internal);
            self.processSensoryInput(percept);
        }, py::arg("internal"), "Process internal input (convenience method)")
        .def("isReady", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            return self.getSensoryInputSize() > 0 && world != nullptr;
        }, py::arg("world"), "Check if agent brain is ready for operation");

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
