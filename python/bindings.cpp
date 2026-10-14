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
        // Power user configuration utilities
        .def("getOrInt", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer value or default")
        .def("getOrFloat", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float value or default")
        .def("getOrString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get string value or default")
        .def("getOrBool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get boolean value or default")
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set integer value")
        .def("setFloat", [](Config& self, const std::string& key, float value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set float value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set string value")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set boolean value")
        .def("export", [](const Config& self) {
            std::map<std::string, std::string> result;
            for (const auto& key : self.getKeys()) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[key] = value.value();
                }
            }
            return result;
        }, "Export configuration as map")
        .def("import", [](Config& self, const std::map<std::string, std::string>& configMap) {
            self.clear();
            for (const auto& pair : configMap) {
                self.set(pair.first, pair.second, ConfigSource::Runtime);
            }
        }, py::arg("configMap"), "Import configuration from map")
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                auto value = other.get<std::string>(key);
                if (value.has_value()) {
                    self.set(key, value.value(), ConfigSource::Runtime);
                }
            }
        }, py::arg("other"), "Merge another configuration into this one")
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys
            std::vector<std::string> errors;
            if (!self.has("brain.neuron_count")) {
                errors.push_back("Missing required key: brain.neuron_count");
            }
            if (!self.has("brain.synapse_density")) {
                errors.push_back("Missing recommended key: brain.synapse_density");
            }
            return errors;
        }, "Validate configuration and return list of errors")

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, py::return_value_policy::copy,
             "Get the raw data as a vector")
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
        .def("getChannels", &Vision::getChannels)
        .def("getData", &Vision::getData, py::return_value_policy::copy,
             "Get vision data as vector");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getData", &Audio::getData, py::return_value_policy::copy,
             "Get audio data as vector");

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
        .def("getAllSignals", &SensoryPercept::getAllSignals, py::return_value_policy::copy,
             "Get all sensory signals as vector")
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get world object at position (returns nullptr if none)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Power user utilities
        .def("getWorldObjects", [](const SimpleWorld& self) {
            return self.objects_;
        }, "Get all objects in world")
        .def("getObjectsByType", [](const SimpleWorld& self, WorldObjectType type) {
            std::vector<WorldObject> result;
            for (const auto& obj : self.objects_) {
                if (obj.type == type) {
                    result.push_back(obj);
                }
            }
            return result;
        }, py::arg("type"), "Get all objects of a specific type")
        .def("countObjects", [](const SimpleWorld& self) {
            return self.objects_.size();
        }, "Get total number of objects in world")
        .def("getObjectsInRange", [](const SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> result;
            for (const auto& obj : self.objects_) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx*dx + dy*dy <= radius*radius) {
                    result.push_back(obj);
                }
            }
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects within a radius")
        .def("getAllSensoryData", [](const SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return std::map<std::string, std::vector<float>>({
                {"vision", percept.getVision()},
                {"touch", percept.getTouch()},
                {"internal", percept.getInternal()},
                {"proprioception", percept.getProprioception()},
                {"audio", percept.getAudio()}
            });
        }, "Get all sensory data as a map")
        .def("getAgentState", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::map<std::string, float>({
                {"x", body.x},
                {"y", body.y},
                {"orientation", body.orientation},
                {"velocity_x", body.velocityX},
                {"velocity_y", body.velocityY},
                {"angular_velocity", body.angularVelocity},
                {"energy", body.energy},
                {"health", body.health},
                {"age", body.age},
                {"is_moving", body.isMoving},
                {"is_turning", body.isTurning}
            });
        }, "Get agent state as a map")
        .def("getWorldStats", [](const SimpleWorld& self) {
            return std::map<std::string, double>({
                {"width", self.getWidth()},
                {"height", self.getHeight()},
                {"max_energy", self.getMaxEnergy()},
                {"energy_decay_rate", self.getEnergyDecayRate()},
                {"simulation_time", self.getSimulationTime()},
                {"random_seed", self.getRandomSeed()}
            });
        }, "Get world statistics as a map")
        .def("exportObjects", [](const SimpleWorld& self) {
            std::vector<std::vector<float>> result;
            for (const auto& obj : self.objects_) {
                result.push_back({obj.x, obj.y, obj.radius, static_cast<float>(obj.type), obj.value});
            }
            return result;
        }, "Export all objects as list of vectors for data analysis")
        .def("importObjects", [](SimpleWorld& self, const std::vector<std::vector<float>>& objects) {
            self.objects_.clear();
            for (const auto& objData : objects) {
                if (objData.size() >= 3) {
                    WorldObject obj(objData[0], objData[1], static_cast<WorldObjectType>(static_cast<int>(objData[2])),
                                   objData.size() >= 4 ? objData[3] : 0.0f,
                                   objData.size() >= 5 ? objData[4] : 0.5f);
                    self.objects_.push_back(obj);
                }
            }
        }, py::arg("objects"), "Import objects from list of vectors")

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
        // Power user utilities
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        .def("getNeuromodulationLevel", &Brain::getNeuromodulationLevel,
             "Get current neuromodulation level (deprecated - use AgentBrain)")
        .def("getTotalPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals directly")

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
        // Power user utilities
        .def("getAllMetrics", [](const AgentBrain& self) {
            return std::map<std::string, float>({
                {"neuromodulation", self.getNeuromodulationLevel()},
                {"curiosity", self.getCuriosityLevel()},
                {"novelty", self.getNoveltyLevel()},
                {"prediction_error", self.getPredictionError()},
                {"development_stage", static_cast<float>(self.getDevelopmentalStage())}
            });
        }, "Get all agent metrics as a map")
        .def("getWorldSensors", [](const AgentBrain& self, const SimpleWorld& world) {
            auto percept = world.getSensoryPercept();
            return std::map<std::string, std::vector<float>>({
                {"vision", percept.getVision()},
                {"touch", percept.getTouch()},
                {"internal", percept.getInternal()},
                {"proprioception", percept.getProprioception()},
                {"audio", percept.getAudio()}
            });
        }, py::arg("world"), "Get all world sensory data as a map")
        .def("getWorldInfo", [](const AgentBrain& self, const SimpleWorld& world) {
            auto body = world.getAgentBody();
            auto pos = world.getWidth();
            return std::map<std::string, float>({
                {"x", body.x},
                {"y", body.y},
                {"orientation", body.orientation},
                {"velocity_x", body.velocityX},
                {"velocity_y", body.velocityY},
                {"energy", body.energy},
                {"health", body.health},
                {"age", body.age},
                {"world_width", pos},
                {"world_height", world.getHeight()}
            });
        }, py::arg("world"), "Get world state information as a map")

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

    // Advanced factory functions for power users
    m.def("createComplexConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set up a complex default configuration for power users
        config->set("brain.neuron_count", 2000, ConfigSource::Default);
        config->set("brain.synapse_density", 0.15f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.5f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("development.synaptogenesis_rate", 0.002f, ConfigSource::Default);
        config->set("development.pruning_rate", 0.0002f, ConfigSource::Default);
        return config;
    }, "Create a complex configuration with advanced settings");

    // Utility functions for power users
    m.def("createAgentBrainWithWorld", [](std::shared_ptr<Config> config, 
                                        std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("config"), py::arg("world"), 
       "Create and initialize an agent brain with a world in one call");

    m.def("createCompleteSetup", [](std::shared_ptr<Config> config,
                                   std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("world"),
       "Create a complete agent setup with all subsystems enabled. Returns tuple of (brain, agent, world)");

    // Configuration validation and setup helpers
    m.def("validateConfig", [](const Config& config) {
        // Basic validation with detailed error reporting
        std::map<std::string, std::vector<std::string>> errors;
        
        if (!config.has("brain.neuron_count")) {
            errors["missing_required"].push_back("brain.neuron_count is required");
        } else {
            auto neurons = config.getOr<int>("brain.neuron_count", 1000);
            if (neurons < 100) {
                errors["warning"].push_back("brain.neuron_count is very low (recommended: 1000+)");
            }
        }
        
        if (!config.has("brain.synapse_density")) {
            errors["warning"].push_back("brain.synapse_density is not set (recommended: 0.1)");
        }
        
        if (!config.has("random_seed")) {
            errors["warning"].push_back("random_seed is not set (recommended for reproducibility)");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return errors/warnings");

    // Serialization helpers
    m.def("serializeBrain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("deserializeBrain", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        // This would need a factory or global registry in a real implementation
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"), "Load brain state from file");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version and build info
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__build_info__") = "Enhanced Python API with power user utilities";
    
    m.def("getBuildInfo", []() {
        return std::map<std::string, std::string>({
            {"version", "0.1.0-enhanced"},
            {"build_date", "2026-01-01"},
            {"api_enhanced", "true"},
            {"features", "power_user_utilities,advanced_configuration,serialization_helpers"}
        });
    }, "Get build and version information");
}

} // namespace nlm
