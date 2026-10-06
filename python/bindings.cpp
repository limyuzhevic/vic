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
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            bool success = self.loadFromFile(filepath);
            if (!success) {
                throw std::runtime_error("Failed to load configuration from file: " + filepath);
            }
            return success;
        }, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            bool success = self.loadFromArgs(argc, argv);
            if (!success) {
                throw std::runtime_error("Failed to load configuration from command line arguments");
            }
            return success;
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            bool success = self.saveToFile(filepath);
            if (!success) {
                throw std::runtime_error("Failed to save configuration to file: " + filepath);
            }
            return success;
        }, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<py::object>(key);
            if (!value.has_value()) {
                throw std::runtime_error("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             "Get configuration value by key")
        .def("getOr", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            try {
                auto value = self.get<py::object>(key);
                return value.value_or(defaultValue);
            } catch (...) {
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value by key with default")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            // Handle different types
            if (value.is_none()) {
                throw std::runtime_error("Cannot set None value in configuration");
            }
            if (value.is<bool>()) {
                self.set(key, value.cast<bool>());
            } else if (value.is<int>()) {
                self.set(key, value.cast<int>());
            } else if (value.is<double>()) {
                self.set(key, value.cast<double>());
            } else if (value.is<std::string>()) {
                self.set(key, value.cast<std::string>());
            } else if (value.is<std::vector<int>>()) {
                self.set(key, value.cast<std::vector<int>>());
            } else if (value.is<std::vector<double>>()) {
                self.set(key, value.cast<std::vector<double>>());
            } else if (value.is<std::vector<std::string>>()) {
                self.set(key, value.cast<std::vector<std::string>>());
            } else {
                throw std::runtime_error("Unsupported configuration value type");
            }
        }, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getEntries", &Config::getEntries,
             "Get all configuration entries")
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
        .def("clone", &Action::clone)
        .def("validate", [](const Action& self) {
            // Basic validation - ensure parameters are within reasonable bounds
            auto params = self.getParameters();
            for (float param : params) {
                if (std::isnan(param) || std::isinf(param)) {
                    throw std::invalid_argument("Action parameters contain NaN or infinity values");
                }
            }
            return true;
        }, "Validate action parameters");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set object position")
        .def("setType", [](WorldObject& self, WorldObjectType type) {
            self.type = type;
        }, py::arg("type"),
             "Set object type")
        .def("setValue", [](WorldObject& self, float value) {
            self.value = value;
        }, py::arg("value"),
             "Set object value (reward or damage)")
        .def("setRadius", [](WorldObject& self, float radius) {
            self.radius = radius;
        }, py::arg("radius"),
             "Set object radius")
        .def("isValid", [](const WorldObject& self) {
            if (self.x < 0 || self.y < 0) return false;
            if (self.radius <= 0) return false;
            if (self.type == WorldObjectType::Empty) return true; // Empty objects are valid
            if (std::isnan(self.value) || std::isinf(self.value)) return false;
            return true;
        }, "Check if world object is valid")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set agent position")
        .def("setOrientation", [](AgentBody& self, float orientation) {
            if (std::isnan(orientation) || std::isinf(orientation)) {
                throw std::invalid_argument("Orientation cannot be NaN or infinity");
            }
            self.orientation = orientation;
        }, py::arg("orientation"),
             "Set agent orientation")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            if (std::isnan(vx) || std::isinf(vx) || std::isnan(vy) || std::isinf(vy)) {
                throw std::invalid_argument("Velocity components cannot be NaN or infinity");
            }
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"),
             "Set agent velocity")
        .def("setEnergy", [](AgentBody& self, float energy) {
            if (energy < 0) throw std::invalid_argument("Energy cannot be negative");
            self.energy = energy;
        }, py::arg("energy"),
             "Set agent energy")
        .def("setHealth", [](AgentBody& self, float health) {
            if (health < 0 || health > 1) throw std::invalid_argument("Health must be between 0 and 1");
            self.health = health;
        }, py::arg("health"),
             "Set agent health")
        .def("setAge", [](AgentBody& self, float age) {
            if (age < 0) throw std::invalid_argument("Age cannot be negative");
            self.age = age;
        }, py::arg("age"),
             "Set agent age")
        .def("validate", [](const AgentBody& self) {
            if (std::isnan(self.x) || std::isinf(self.x)) throw std::invalid_argument("Invalid X position");
            if (std::isnan(self.y) || std::isinf(self.y)) throw std::invalid_argument("Invalid Y position");
            if (std::isnan(self.orientation) || std::isinf(self.orientation)) throw std::invalid_argument("Invalid orientation");
            if (std::isnan(self.velocityX) || std::isinf(self.velocityX)) throw std::invalid_argument("Invalid velocity X");
            if (std::isnan(self.velocityY) || std::isinf(self.velocityY)) throw std::invalid_argument("Invalid velocity Y");
            if (self.energy < 0) throw std::invalid_argument("Invalid energy");
            if (self.health < 0 || self.health > 1) throw std::invalid_argument("Invalid health");
            if (self.age < 0) throw std::invalid_argument("Invalid age");
            return true;
        }, "Validate agent body state")
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
        .def("configure", [](SimpleWorld& self, float width, float height, size_t visionWidth, size_t visionHeight) {
            if (width <= 0) throw std::invalid_argument("World width must be positive");
            if (height <= 0) throw std::invalid_argument("World height must be positive");
            if (visionWidth == 0) throw std::invalid_argument("Vision width must be positive");
            if (visionHeight == 0) throw std::invalid_argument("Vision height must be positive");
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
        .def("receiveSensoryInputs", [](Brain& self, const std::vector<class SensoryInput>& inputs) {
            for (const auto& input : inputs) {
                self.receiveSensoryInput(input);
            }
        }, py::arg("inputs"),
             "Receive multiple sensory inputs at once")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("injectCurrents", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
            for (const auto& inj : injections) {
                self.injectCurrent(inj.first, inj.second);
            }
        }, py::arg("injections"),
             "Inject current into multiple neurons at once")
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
        .def("getConfigValue", [](const Brain& self, const std::string& key) {
            auto value = self.getConfig()->get<py::object>(key);
            if (!value.has_value()) {
                throw std::runtime_error("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             "Get a specific configuration value")
        .def("setConfigValue", [](Brain& self, const std::string& key, const py::object& value) {
            self.getConfig()->set(key, value.cast<py::object>());
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["pending_spike_events"] = self.getPendingSpikeEventCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

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

    // Utility functions for batch operations
    m.def("batchReceiveSensoryInput", [](std::shared_ptr<Brain> brain, const std::vector<class SensoryInput>& inputs) {
        for (const auto& input : inputs) {
            brain->receiveSensoryInput(input);
        }
    }, py::arg("brain"), py::arg("inputs"),
         "Receive multiple sensory inputs into a brain at once");

    m.def("batchInjectCurrents", [](std::shared_ptr<Brain> brain, 
                                 const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
        for (const auto& inj : injections) {
            brain->injectCurrent(inj.first, inj.second);
        }
    }, py::arg("brain"), py::arg("injections"),
         "Inject current into multiple neurons at once");

    m.def("batchStepBrains", [](const std::vector<std::shared_ptr<Brain>>& brains, SimulationStep currentStep) {
        for (auto& brain : brains) {
            brain->step(currentStep);
        }
    }, py::arg("brains"), py::arg("currentStep"),
         "Step multiple brains simultaneously");

    // Statistics and analysis functions
    m.def("computeGlobalStatistics", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        py::dict globalStats;
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t totalActiveNeurons = 0;
        size_t totalFiringNeurons = 0;
        float totalFiringRate = 0.0f;
        float totalEIRatio = 0.0f;
        size_t brainCount = 0;
        
        for (const auto& brain : brains) {
            if (brain) {
                totalNeurons += brain->getTotalNeuronCount();
                totalSynapses += brain->getTotalSynapseCount();
                totalActiveNeurons += brain->getActiveNeuronCount();
                totalFiringNeurons += brain->getFiringNeuronCount();
                totalFiringRate += brain->getAverageFiringRate();
                totalEIRatio += brain->getExcitationInhibitionRatio();
                brainCount++;
            }
        }
        
        if (brainCount > 0) {
            globalStats["average_neurons"] = static_cast<double>(totalNeurons) / brainCount;
            globalStats["average_synapses"] = static_cast<double>(totalSynapses) / brainCount;
            globalStats["average_active_neurons"] = static_cast<double>(totalActiveNeurons) / brainCount;
            globalStats["average_firing_neurons"] = static_cast<double>(totalFiringNeurons) / brainCount;
            globalStats["average_firing_rate"] = totalFiringRate / brainCount;
            globalStats["average_e_i_ratio"] = totalEIRatio / brainCount;
        }
        
        return globalStats;
    }, py::arg("brains"),
         "Compute global statistics across multiple brains");

    // Validation utilities
    m.def("validateAllObjects", [](const std::vector<WorldObject>& objects) {
        std::vector<bool> validityResults;
        validityResults.reserve(objects.size());
        
        for (const auto& obj : objects) {
            validityResults.push_back(obj.isValid());
        }
        
        return validityResults;
    }, py::arg("objects"),
         "Validate multiple world objects and return validity results");

    m.def("normalizeActionParameters", [](const std::vector<float>& params, float minValue = 0.0f, float maxValue = 1.0f) {
        std::vector<float> normalizedParams;
        normalizedParams.reserve(params.size());
        
        float paramMin = *std::min_element(params.begin(), params.end());
        float paramMax = *std::max_element(params.begin(), params.end());
        
        if (paramMax > paramMin) {
            for (float param : params) {
                normalizedParams.push_back(minValue + (maxValue - minValue) * 
                                         ((param - paramMin) / (paramMax - paramMin)));
            }
        } else {
            normalizedParams = params; // All values are the same
        }
        
        return normalizedParams;
    }, py::arg("params"), py::arg("minValue") = 0.0f, py::arg("maxValue") = 1.0f,
         "Normalize action parameters to a specified range");

    // Configuration utilities
    m.def("mergeConfigs", [](const std::vector<std::shared_ptr<Config>>& configs) {
        auto mergedConfig = std::make_shared<Config>();
        
        for (const auto& config : configs) {
            if (config) {
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    auto value = config->get<py::object>(key);
                    if (value.has_value()) {
                        mergedConfig->set(key, *value);
                    }
                }
            }
        }
        
        return mergedConfig;
    }, py::arg("configs"),
         "Merge multiple configuration objects into one");

    // Debug and monitoring utilities
    m.def("getBrainHealth", [](const std::shared_ptr<Brain>& brain) {
        py::dict healthInfo;
        
        if (!brain) {
            healthInfo["status"] = "invalid";
            healthInfo["error"] = "Brain pointer is null";
            return healthInfo;
        }
        
        healthInfo["status"] = "healthy";
        healthInfo["total_neurons"] = brain->getTotalNeuronCount();
        healthInfo["total_synapses"] = brain->getTotalSynapseCount();
        healthInfo["active_neurons"] = brain->getActiveNeuronCount();
        healthInfo["firing_neurons"] = brain->getFiringNeuronCount();
        healthInfo["avg_firing_rate"] = brain->getAverageFiringRate();
        healthInfo["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        healthInfo["total_spikes"] = brain->getTotalSpikeCount();
        healthInfo["pending_spike_events"] = brain->getPendingSpikeEventCount();
        
        // Check for potential issues
        if (brain->getTotalNeuronCount() == 0) {
            healthInfo["warning"] = "No neurons in brain";
        }
        if (brain->getTotalSynapseCount() == 0) {
            healthInfo["warning"] = "No synapses in brain";
        }
        if (brain->getAverageFiringRate() > 100.0f) {
            healthInfo["warning"] = "Very high firing rate detected";
        }
        
        return healthInfo;
    }, py::arg("brain"),
         "Get comprehensive health information about a brain");

    m.def("benchmarkOperation", [](const std::string& operationName, std::function<void()> operation) {
        auto startTime = std::chrono::high_resolution_clock::now();
        operation();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        py::dict result;
        result["operation"] = operationName;
        result["duration_microseconds"] = static_cast<long long>(duration.count());
        result["duration_milliseconds"] = static_cast<double>(duration.count()) / 1000.0;
        result["success"] = true;
        
        return result;
    }, py::arg("operationName"), py::arg("operation"),
         "Benchmark an operation and return timing results");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
