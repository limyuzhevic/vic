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
             "Load configuration from a JSON/YAML/simple file. Supports JSON, YAML, and key=value formats")
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
        // Pythonic property setters
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set float/double configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value")
        // Getters with Pythonic naming
        .def("get_int", &Config::getOr<int>, py::arg("key"), py::arg("default") = 0,
             "Get integer value, returns default if not found")
        .def("get_float", &Config::getOr<double>, py::arg("key"), py::arg("default") = 0.0,
             "Get float value, returns default if not found")
        .def("get_bool", &Config::getOr<bool>, py::arg("key"), py::arg("default") = false,
             "Get boolean value, returns default if not found")
        .def("get_string", &Config::getOr<std::string>, py::arg("key"), py::arg("default") = "",
             "Get string value, returns default if not found")
        // Advanced configuration methods
        .def("validate", &Config::validate, py::arg("schema"),
             "Validate configuration against schema (JSON schema)")
        .def("get_type", &Config::getType, py::arg("key"),
             "Get configuration value type (int, float, string, bool, array)")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("get_stats", &Config::getStats,
             "Get configuration statistics and metadata")
        // Context manager for automatic cleanup
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config& self, py::object, py::object, py::object) {
            self.clear();
        })
        // Pythonic dictionary-like interface
        .def("update", [](Config& self, const py::dict& values) {
            for (auto item : values) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance(item.second, py::int_())) {
                    self.set(key, py::cast<int>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::float_())) {
                    self.set(key, py::cast<double>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::bool_())) {
                    self.set(key, py::cast<bool>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::str_())) {
                    self.set(key, py::cast<std::string>(item.second), ConfigSource::Runtime);
                }
            }
        }, py::arg("values"),
             "Update configuration from Python dictionary (Pythonic)")
        // String representation
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("to_dict", [](const Config& cfg) {
            py::dict result;
            for (const auto& key : cfg.getKeys()) {
                auto valueOpt = cfg.get<std::string>(key);
                if (valueOpt) {
                    result[key] = valueOpt.value();
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& values) {
            self.clear();
            for (auto item : values) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance(item.second, py::int_())) {
                    self.set(key, py::cast<int>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::float_())) {
                    self.set(key, py::cast<double>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::bool_())) {
                    self.set(key, py::cast<bool>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::str_())) {
                    self.set(key, py::cast<std::string>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::list_())) {
                    // Convert list to string representation
                    py::list pyList = py::cast<py::list>(item.second);
                    std::vector<double> vec;
                    for (auto it : pyList) {
                        vec.push_back(py::cast<double>(it));
                    }
                    std::ostringstream oss;
                    oss << "[";
                    for (size_t i = 0; i < vec.size(); ++i) {
                        if (i > 0) oss << ",";
                        oss << vec[i];
                    }
                    oss << "]";
                    self.set(key, oss.str(), ConfigSource::Runtime);
                }
            }
            return &self; // Return self for method chaining
        }, py::arg("values"),
             "Update configuration from Python dictionary (returns self for chaining)")
        // Configuration builder pattern
        .def("builder", []() -> std::unique_ptr<py::dict> {
            auto builder = std::make_unique<py::dict>();
            return builder;
        }, "Create a configuration builder dictionary for fluent API")
        // Convenience factory methods
        .def("neural_config", []() -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            // Set sensible defaults for neural simulation
            config->set("neuron_count", 1000);
            config->set("region_count", 1);
            config->set("simulation_timestep", 0.001);
            config->set("random_seed", 42);
            config->set("stdp_ltp_weight", 0.02f);
            config->set("stdp_ltd_weight", 0.015f);
            config->set("stdp_tau", 20.0f);
            config->set("connection_probability", 0.15f);
            return config;
        }, "Create a neural simulation configuration with sensible defaults")
        .def("world_config", []() -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            // Set defaults for world simulation
            config->set("world_width", 100.0);
            config->set("world_height", 100.0);
            config->set("max_energy", 100.0);
            config->set("energy_decay_rate", 0.01);
            return config;
        }, "Create a world simulation configuration with sensible defaults")
        .def("agent_config", []() -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            // Set defaults for agent behavior
            config->set("enable_curiosity", true);
            config->set("enable_reward_modulation", true);
            config->set("enable_development", true);
            config->set("curiosity_sensitivity", 0.5);
            config->set("reward_decay_rate", 0.1);
            return config;
        }, "Create an agent configuration with sensible defaults");


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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration. Use nlm.createBrain() for simpler API")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        // Step methods with timestamps
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        // Sensory input methods
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        // Current injection methods
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        // Motor action
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        // State management
        .def("reset", &Brain::reset,
             "Reset brain state to initial configuration")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name")
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
        // Statistics and monitoring
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
        // Configuration access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        // Logging and status
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics")
        // Context manager for automatic cleanup
        .def("__enter__", [](Brain& self) { return &self; })
        .def("__exit__", [](Brain& self, py::object, py::object, py::object) {
            self.reset();
        })
        // Pythonic iteration over regions
        .def("regions", [](Brain& self) {
            py::list regions;
            for (const auto& regionPtr : self.getRegions()) {
                regions.append(regionPtr.get());
            }
            return regions;
        }, "Get list of all regions (Pythonic)")
        // Convenience simulation methods
        .def("run_simulation", [](Brain& self, int steps, double timestep = 0.001) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001,
             "Run simulation for specified number of steps")
        // Advanced configuration methods
        .def("configure_simulation", [](Brain& self, const py::dict& params) {
            auto config = self.getConfig();
            for (auto item : params) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance(item.second, py::int_())) {
                    config->set(key, py::cast<int>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::float_())) {
                    config->set(key, py::cast<double>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::bool_())) {
                    config->set(key, py::cast<bool>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::str_())) {
                    config->set(key, py::cast<std::string>(item.second), ConfigSource::Runtime);
                }
            }
            // Re-initialize with new config
            self.reset();
            self.initialize();
            return &self; // Return self for method chaining
        }, py::arg("params"),
             "Configure brain simulation with Python dictionary (returns self for chaining)")
        // Simulation statistics
        .def("get_simulation_stats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["firing_rate"] = self.getAverageFiringRate();
            stats["excitatory_inhibitory_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get simulation statistics as Python dictionary")
        // Neural activity analysis
        .def("get_neural_activity_summary", [](Brain& self) {
            py::dict summary;
            summary["total_spikes"] = self.getTotalSpikeCount();
            summary["firing_rate"] = self.getAverageFiringRate();
            summary["active_ratio"] = 
                static_cast<double>(self.getActiveNeuronCount()) / 
                std::max(1.0, static_cast<double>(self.getTotalNeuronCount()));
            return summary;
        }, "Get neural activity summary for analysis")
        // Factory methods (these are in the module-level functions, but we can add more)
        .def("create_with_config", [](const py::dict& config_dict) {
            auto config = std::make_shared<Config>();
            for (auto item : config_dict) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance(item.second, py::int_())) {
                    config->set(key, py::cast<int>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::float_())) {
                    config->set(key, py::cast<double>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::bool_())) {
                    config->set(key, py::cast<bool>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::str_())) {
                    config->set(key, py::cast<std::string>(item.second), ConfigSource::Runtime);
                }
            }
            return nlm::createBrain(config);
        }, py::arg("config_dict"),
             "Create brain from Python dictionary configuration (convenience factory)")
        // Error handling improvements
        .def("safe_step", [](Brain& self, int step, double time) {
            try {
                self.step(step, time);
                return true;
            } catch (const std::exception& e) {
                py::print("Error during simulation step:", e.what());
                return false;
            }
        }, py::arg("step"), py::arg("time"),
             "Safe step method with error handling (returns success boolean)")
        .def("is_initialized", [](const Brain& self) {
            // This would need to check internal state, but for now we'll assume
            // that if getConfig() works, the brain is initialized
            return !self.getConfig()->getKeys().empty();
        }, "Check if brain is initialized and ready for simulation");


    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain with underlying neural brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world interface")
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
        // Pythonic interface improvements
        .def("enable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning systems (convenience method)")
        .def("disable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning systems (convenience method)")
        .def("is_learning_enabled", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get learning system status as Python dictionary")
        .def("get_agent_stats", [](const AgentBrain& self) {
            py::dict stats;
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get agent statistics and state information")
        .def("create_episode", [](AgentBrain& self) {
            self.reset();
            return &self; // Return self for method chaining
        }, "Reset and prepare for new episode (returns self for chaining)")
        .def("update_episode", [](AgentBrain& self, double timestep) {
            self.updateDevelopment(timestep);
            return &self; // Return self for method chaining
        }, "Update agent development state (returns self for chaining)")
        .def("process_episode_input", [](AgentBrain& self, const py::dict& sensory_data) {
            // Create a sensory percept from dictionary
            auto percept = std::make_shared<SensoryPercept>();
            
            if (sensory_data.contains("vision")) {
                auto vision_data = py::cast<py::list>(sensory_data["vision"]);
                std::vector<float> vision_vec;
                for (auto item : vision_data) {
                    vision_vec.push_back(py::cast<float>(item));
                }
                percept->setVision(Vision(vision_vec.size() / 3, vision_vec.size() / 3, 3));
                percept->getVision().setData(vision_vec);
            }
            
            if (sensory_data.contains("audio")) {
                auto audio_data = py::cast<py::list>(sensory_data["audio"]);
                std::vector<float> audio_vec;
                for (auto item : audio_data) {
                    audio_vec.push_back(py::cast<float>(item));
                }
                auto audio = Audio(44100, audio_vec.size());
                audio.setData(audio_vec);
                percept->setAudio(audio);
            }
            
            self.processSensoryInput(percept);
            return &self; // Return self for method chaining
        }, py::arg("sensory_data"),
             "Process sensory input from Python dictionary (returns self for chaining)")
        // Context manager
        .def("__enter__", [](AgentBrain& self) { return &self; })
        .def("__exit__", [](AgentBrain& self, py::object, py::object, py::object) {
            self.reset();
        });


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
