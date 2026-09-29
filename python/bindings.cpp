#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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
        
        This binding provides comprehensive access to the NLM neural simulation framework,
        enabling both beginners and advanced users to create and experiment with artificial brains.
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

    // Enhanced Config class with more Python-friendly methods
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
        // Enhanced Python-specific methods
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Python);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Python);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Python);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Python);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value with default")
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value with default")
        .def("getOr", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get float value with default")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get string value with default")
        .def("getBool", [](const Config& self, const std::string& key) {
            return self.getOr<bool>(key, false);
        }, py::arg("key"), "Get boolean value")
        .def("getInt", [](const Config& self, const std::string& key) {
            return self.getOr<int>(key, 0);
        }, py::arg("key"), "Get integer value")
        .def("getFloat", [](const Config& self, const std::string& key) {
            return self.getOr<float>(key, 0.0f);
        }, py::arg("key"), "Get float value")
        .def("getString", [](const Config& self, const std::string& key) {
            return self.getOr<std::string>(key, "");
        }, py::arg("key"), "Get string value")
        // Preset configuration methods
        .def("setDefaults", [](Config& self) {
            // Set sensible defaults for beginners
            self.set("random_seed", uint64_t(42), ConfigSource::Default);
            self.set("simulation_timestep", 0.001, ConfigSource::Default);
            self.set("neuron_count", size_t(500), ConfigSource::Default);
            self.set("region_count", size_t(1), ConfigSource::Default);
            self.set("connection_probability", 0.1f, ConfigSource::Default);
            self.set("enable_reward_modulation", true, ConfigSource::Default);
            self.set("enable_structural_plasticity", false, ConfigSource::Default);
            self.set("enable_development", true, ConfigSource::Default);
            self.set("enable_curiosity", true, ConfigSource::Default);
        }, "Set default configuration for beginners")
        .def("setBeginnerDefaults", [](Config& self) {
            // Very simple configuration for absolute beginners
            self.set("neuron_count", size_t(100), ConfigSource::Default);
            self.set("region_count", size_t(1), ConfigSource::Default);
            self.set("simulation_timestep", 0.01, ConfigSource::Default);
            self.set("random_seed", uint64_t(42), ConfigSource::Default);
        }, "Set beginner-friendly configuration")
        .def("setAdvancedDefaults", [](Config& self) {
            // More sophisticated configuration for advanced users
            self.set("neuron_count", size_t(2000), ConfigSource::Default);
            self.set("region_count", size_t(2), ConfigSource::Default);
            self.set("connection_probability", 0.15f, ConfigSource::Default);
            self.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
            self.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
            self.set("stdp_tau", 20.0f, ConfigSource::Default);
            self.set("enable_reward_modulation", true, ConfigSource::Default);
            self.set("enable_structural_plasticity", true, ConfigSource::Default);
            self.set("enable_development", true, ConfigSource::Default);
            self.set("enable_curiosity", true, ConfigSource::Default);
            self.set("enable_novelty", true, ConfigSource::Default);
        }, "Set advanced configuration for experienced users")
        .def("validate", [](const Config& self) {
            // Simple validation
            bool isValid = true;
            if (self.has("neuron_count")) {
                size_t neurons = self.getOr<size_t>("neuron_count", 0);
                if (neurons == 0) {
                    std::cerr << "Warning: neuron_count should be greater than 0" << std::endl;
                    isValid = false;
                }
            }
            if (self.has("region_count")) {
                size_t regions = self.getOr<size_t>("region_count", 0);
                if (regions == 0) {
                    std::cerr << "Warning: region_count should be greater than 0" << std::endl;
                    isValid = false;
                }
            }
            if (self.has("simulation_timestep")) {
                double timestep = self.getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0) {
                    std::cerr << "Warning: simulation_timestep should be positive" << std::endl;
                    isValid = false;
                }
            }
            return isValid;
        }, "Validate configuration settings")
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
        // World convenience methods
        .def("addResource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f, "Add a resource at position")
        .def("addHazard", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 5.0f, "Add a hazard at position")
        .def("addWall", [](SimpleWorld& world, float x, float y) {
            WorldObject obj(x, y, WorldObjectType::Wall);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), "Add a wall at position")
        .def("findNearestResource", [](SimpleWorld& world, float x, float y) {
            float minDist = 1000.0f;
            WorldObject* nearest = nullptr;
            for (const auto& obj : world.getObjects()) {
                if (obj.type == WorldObjectType::Resource) {
                    float dist = std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
                    if (dist < minDist) {
                        minDist = dist;
                        nearest = &const_cast<WorldObject&>(obj);
                    }
                }
            }
            if (nearest) {
                return std::make_pair(nearest->x, nearest->y);
            }
            return std::make_pair(0.0f, 0.0f);
        }, py::arg("x"), py::arg("y"), "Find nearest resource");

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
        // Brain convenience methods
        .def("getMostActiveNeurons", [](Brain& self, size_t count) {
            return self.getSpikeSystem()->getMostActiveNeurons(count);
        }, py::arg("count") = 10, "Get most active neurons")
        .def("simulateEpisode", [](Brain& self, size_t steps, double timestep = 0.001) {
            for (size_t step = 0; step < steps; ++step) {
                self.step(step, step * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001, "Simulate multiple steps")
        .def("hasMemorySystems", [](Brain& self) {
            return self.getWorkingMemory() != nullptr &&
                   self.getEpisodicMemory() != nullptr &&
                   self.getAssociativeMemory() != nullptr;
        }, "Check if memory systems are available")
        .def("hasPredictionSystem", [](Brain& self) {
            return self.getPredictionSystem() != nullptr;
        }, "Check if prediction system is available")
        .def("hasNeuromodulation", [](Brain& self) {
            return self.getDopamine() != nullptr &&
                   self.getCuriosity() != nullptr &&
                   self.getNovelty() != nullptr &&
                   self.getPredictionErrorSignal() != nullptr;
        }, "Check if neuromodulation systems are available");

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
        // Agent brain convenience methods
        .def("enableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning systems")
        .def("disableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning systems")
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, size_t steps) {
            float totalReward = 0.0f;
            for (size_t step = 0; step < steps; ++step) {
                world.update(0.01);
                const SensoryPercept& percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                self.getBrain()->step(step);
                MotorCommand cmd = self.decodeMotorCommand();
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                self.applyRewardModulation(result.reward, 0.0f);
                totalReward += result.reward;
            }
            return totalReward;
        }, py::arg("world"), py::arg("steps"), "Run an episode and return total reward")
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

    // Global constants and utility functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Python-specific utility functions
    m.def("runQuickDemo", []() {
        auto config = std::make_shared<Config>();
        config->setDefaults();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        // Run a few steps
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        return brain;
    }, "Run a quick demo and return the brain");

    m.def("createDevelopmentExperiment", []() {
        // Create a simple development experiment setup
        auto config = std::make_shared<Config>();
        config->set("neuron_count", size_t(200));
        config->set("simulation_timestep", 0.001);
        config->set("enable_development", true);
        config->set("enable_structural_plasticity", true);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->setRandomSeed(42);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        agentBrain->enableAllSystems();
        
        return std::make_tuple(config, world, brain, agentBrain);
    }, "Create a development experiment setup");

    m.def("validateConfig", [](const Config& config) {
        return config.validate();
    }, py::arg("config"), "Validate configuration settings");

    // Example experiment functions
    m.def("exampleBeginnersExperiment", []() {
        // A simple example for beginners
        auto config = std::make_shared<Config>();
        config->setBeginnerDefaults();
        
        if (!config->validate()) {
            throw std::runtime_error("Invalid beginner configuration");
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run some steps
        for (size_t i = 0; i < 50; ++i) {
            brain->step(i);
        }
        
        return std::make_pair(config, brain);
    }, "Run a beginner example experiment");

    m.def("exampleAdvancedExperiment", []() {
        // A more complex example for advanced users
        auto config = std::make_shared<Config>();
        config->setAdvancedDefaults();
        
        if (!config->validate()) {
            throw std::runtime_error("Invalid advanced configuration");
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run more steps with developmental changes
        for (size_t i = 0; i < 200; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return std::make_pair(config, brain);
    }, "Run an advanced example experiment");
}

} // namespace nlm
