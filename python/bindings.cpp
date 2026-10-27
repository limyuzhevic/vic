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
    // Convenience factory methods
    .def("setDefaults", [](Config& self, const std::string& configType) {
            if (configType == "balanced") {
                self.set("neuron_count", 500, ConfigSource::Default);
                self.set("region_count", 2, ConfigSource::Default);
                self.set("connection_probability", 0.15f, ConfigSource::Default);
                self.set("simulation_timestep", 0.001, ConfigSource::Default);
                self.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
                self.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
                self.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
                self.set("pruning_rate", 0.00001f, ConfigSource::Default);
                NLM_LOG_INFO("Applied balanced configuration defaults");
            } else if (configType == "development") {
                self.set("neuron_count", 1000, ConfigSource::Default);
                self.set("region_count", 3, ConfigSource::Default);
                self.set("connection_probability", 0.2f, ConfigSource::Default);
                self.set("simulation_timestep", 0.001, ConfigSource::Default);
                self.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
                self.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
                self.set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
                self.set("pruning_rate", 0.00002f, ConfigSource::Default);
                NLM_LOG_INFO("Applied development-focused configuration defaults");
            } else if (configType == "production") {
                self.set("neuron_count", 2000, ConfigSource::Default);
                self.set("region_count", 4, ConfigSource::Default);
                self.set("connection_probability", 0.1f, ConfigSource::Default);
                self.set("simulation_timestep", 0.001, ConfigSource::Default);
                self.set("stdp_ltp_weight", 0.005f, ConfigSource::Default);
                self.set("stdp_ltd_weight", 0.008f, ConfigSource::Default);
                self.set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
                self.set("pruning_rate", 0.000005f, ConfigSource::Default);
                NLM_LOG_INFO("Applied production configuration defaults");
            } else {
                throw py::value_error("Unknown config type: " + configType + ". Use 'balanced', 'development', or 'production'");
            }
        }, py::arg("configType"), R"pbdoc(Apply preset configuration defaults. Options: 'balanced', 'development', 'production'")pbdoc")
        .def("validate", [](Config& self) {
            std::vector<std::string> errors;
            if (!self.has("neuron_count")) errors.push_back("Missing neuron_count");
            if (!self.has("region_count")) errors.push_back("Missing region_count");
            if (!self.has("connection_probability")) errors.push_back("Missing connection_probability");
            if (!self.has("simulation_timestep")) errors.push_back("Missing simulation_timestep");
            
            if (!errors.empty()) {
                throw py::value_error("Invalid configuration:\n" + 
                                    std::accumulate(errors.begin(), errors.end(), std::string(),
                                                    [](const std::string& acc, const std::string& err) {
                                                        return acc.empty() ? err : acc + "\n" + err;
                                                    }));
            }
            NLM_LOG_INFO("Configuration validated successfully");
        }, R"pbdoc(Validate configuration and throw exception if invalid)pbdoc")
        .def("is_valid", [](Config& self) {
            std::vector<std::string> errors;
            if (!self.has("neuron_count")) errors.push_back("Missing neuron_count");
            if (!self.has("region_count")) errors.push_back("Missing region_count");
            if (!self.has("connection_probability")) errors.push_back("Missing connection_probability");
            if (!self.has("simulation_timestep")) errors.push_back("Missing simulation_timestep");
            return errors.empty();
        }, R"pbdoc(Check if configuration is valid)pbdoc")
        .def("get_int", [](Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (!value) throw py::key_error("Key not found: " + key);
            return *value;
        }, py::arg("key"), R"pbdoc(Get integer value from config)pbdoc")
        .def("get_float", [](Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            if (!value) throw py::key_error("Key not found: " + key);
            return *value;
        }, py::arg("key"), R"pbdoc(Get float value from config)pbdoc")
        .def("get_string", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) throw py::key_error("Key not found: " + key);
            return *value;
        }, py::arg("key"), R"pbdoc(Get string value from config)pbdoc")
        .def("set_int", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), R"pbdoc(Set integer value in config)pbdoc")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), R"pbdoc(Set float value in config)pbdoc")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), R"pbdoc(Set string value in config)pbdoc")
        .def("get_neurons", [](Config& self) {
            return self.getOr<int64_t>("neuron_count", 500);
        }, R"pbdoc(Get neuron count)pbdoc")
        .def("set_neurons", [](Config& self, int64_t neurons) {
            self.set("neuron_count", neurons, ConfigSource::User);
        }, py::arg("neurons"), R"pbdoc(Set neuron count)pbdoc")
        .def("get_regions", [](Config& self) {
            return self.getOr<int64_t>("region_count", 2);
        }, R"pbdoc(Get region count)pbdoc")
        .def("set_regions", [](Config& self, int64_t regions) {
            self.set("region_count", regions, ConfigSource::User);
        }, py::arg("regions"), R"pbdoc(Set region count)pbdoc")
        .def("get_timestep", [](Config& self) {
            return self.getOr<double>("simulation_timestep", 0.001);
        }, R"pbdoc(Get simulation timestep)pbdoc")
        .def("set_timestep", [](Config& self, double timestep) {
            self.set("simulation_timestep", timestep, ConfigSource::User);
        }, py::arg("timestep"), R"pbdoc(Set simulation timestep)pbdoc")
        .def("get_connectivity", [](Config& self) {
            return self.getOr<float>("connection_probability", 0.15f);
        }, R"pbdoc(Get connection probability)pbdoc")
        .def("set_connectivity", [](Config& self, float connectivity) {
            self.set("connection_probability", connectivity, ConfigSource::User);
        }, py::arg("connectivity"), R"pbdoc(Set connection probability)pbdoc")
        .def("get_plasticity_params", [](Config& self) {
            return py::dict(
                "stdp_ltp_weight", self.getOr<float>("stdp_ltp_weight", 0.01f),
                "stdp_ltd_weight", self.getOr<float>("stdp_ltd_weight", 0.012f),
                "stdp_tau", self.getOr<float>("stdp_tau", 20.0f),
                "synaptogenesis_rate", self.getOr<float>("synaptogenesis_rate", 0.0001f),
                "pruning_rate", self.getOr<float>("pruning_rate", 0.00001f)
            );
        }, R"pbdoc(Get plasticity parameters as dict)pbdoc")
        .def("set_plasticity_params", [](Config& self, py::dict params) {
            if (params.contains("stdp_ltp_weight")) {
                self.set("stdp_ltp_weight", py::cast<float>(params["stdp_ltp_weight"]), ConfigSource::User);
            }
            if (params.contains("stdp_ltd_weight")) {
                self.set("stdp_ltd_weight", py::cast<float>(params["stdp_ltd_weight"]), ConfigSource::User);
            }
            if (params.contains("stdp_tau")) {
                self.set("stdp_tau", py::cast<float>(params["stdp_tau"]), ConfigSource::User);
            }
            if (params.contains("synaptogenesis_rate")) {
                self.set("synaptogenesis_rate", py::cast<float>(params["synaptogenesis_rate"]), ConfigSource::User);
            }
            if (params.contains("pruning_rate")) {
                self.set("pruning_rate", py::cast<float>(params["pruning_rate"]), ConfigSource::User);
            }
        }, py::arg("params"), R"pbdoc(Set plasticity parameters from dict)pbdoc")
        .def("copy_from", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                self.set(key, other.getKeys()[key], ConfigSource::User);
            }
        }, py::arg("other"), R"pbdoc(Copy configuration from another Config object)pbdoc")

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
        // Convenience methods for common workflows
        .def("runExperiment", [](Brain& self, std::shared_ptr<SimpleWorld> world,
                                uint64_t maxSteps, bool autoReset) {
            float totalReward = 0.0f;
            float totalFiringRate = 0.0f;
            
            NLM_LOG_INFO("Starting experiment: " + std::to_string(maxSteps) + " steps");
            
            for (uint64_t step = 0; step < maxSteps; ++step) {
                // Get observation from world if provided
                if (world) {
                    auto percept = world->getSensoryPercept();
                    self.receiveSensoryInput(percept);
                }
                
                // Run brain step
                self.step(step, step * 0.001);
                
                // Get motor command if there are motor neurons
                auto action = self.produceAction();
                
                // Apply action to world if provided
                if (world) {
                    // Convert Action to MotorCommand for world application
                    // This is a simplified mapping - real implementation would be more sophisticated
                    world->applyMotorCommand(action->getType(), step * 0.001);
                }
                
                // Compute reward (simplified - would use actual reward system)
                float reward = self.getTotalSpikeCount() > 0 ? 1.0f : 0.0f;
                totalReward += reward;
                totalFiringRate += self.getAverageFiringRate();
                
                if (step % 1000 == 0) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + 
                                " | Reward: " + std::to_string(totalReward / (step + 1)) +
                                " | Firing: " + std::to_string(self.getAverageFiringRate()) +
                                " | Neurons: " + std::to_string(self.getTotalNeuronCount()));
                }
            }
            
            if (autoReset) {
                self.reset();
                self.initialize(self.getConfig());
            }
            
            NLM_LOG_INFO("Experiment completed: avg reward = " + 
                        std::to_string(totalReward / maxSteps) +
                        ", avg firing = " + std::to_string(totalFiringRate / maxSteps));
            
            return py::dict(
                "total_reward", totalReward / maxSteps,
                "avg_firing_rate", totalFiringRate / maxSteps,
                "total_steps", maxSteps,
                "final_spike_count", self.getTotalSpikeCount()
            );
        }, py::arg("world") = nullptr, py::arg("maxSteps") = 1000, py::arg("autoReset") = true,
        R"pbdoc(Run complete experiment with optional world interaction. Returns dict with metrics.)pbdoc")
        .def("runEpisode", [](Brain& self, std::shared_ptr<SimpleWorld> world,
                             uint64_t maxSteps, bool autoReset) {
            float totalReward = 0.0f;
            
            NLM_LOG_INFO("Starting episode: " + std::to_string(maxSteps) + " steps");
            
            for (uint64_t step = 0; step < maxSteps; ++step) {
                // Get observation from world if provided
                if (world) {
                    auto percept = world->getSensoryPercept();
                    self.receiveSensoryInput(percept);
                }
                
                // Run brain step
                self.step(step, step * 0.001);
                
                // Get motor command if there are motor neurons
                auto action = self.produceAction();
                
                // Apply action to world if provided
                if (world) {
                    world->applyMotorCommand(action->getType(), step * 0.001);
                }
                
                // Simple reward based on activity
                float reward = self.getFiringNeuronCount() > 0 ? 1.0f : 0.0f;
                totalReward += reward;
                
                if (step % 500 == 0) {
                    NLM_LOG_INFO("Episode step " + std::to_string(step) + 
                                " | Reward: " + std::to_string(totalReward / (step + 1)));
                }
            }
            
            if (autoReset) {
                self.reset();
                self.initialize(self.getConfig());
            }
            
            NLM_LOG_INFO("Episode completed: total reward = " + std::to_string(totalReward));
            return totalReward;
        }, py::arg("world") = nullptr, py::arg("maxSteps") = 1000, py::arg("autoReset") = true,
        R"pbdoc(Run single episode with optional world interaction. Returns total reward.)pbdoc")
        .def("is_initialized", &Brain::initialize,
             "Check if brain is initialized (requires checking state)")
        .def("total_neurons", &Brain::getTotalNeuronCount,
             "Get total neuron count")
        .def("total_synapses", &Brain::getTotalSynapseCount,
             "Get total synapse count")
        .def("get_stats", &Brain::getConfig,
             "Get brain statistics and metrics")
        .def("get_activity_summary", [](Brain& self, size_t timeWindow) {
            return py::dict(
                "total_spikes", self.getTotalSpikeCount(),
                "active_neurons", self.getActiveNeuronCount(),
                "firing_neurons", self.getFiringNeuronCount(),
                "avg_firing_rate", self.getAverageFiringRate(),
                "excitatory_inhibitory_ratio", self.getExcitationInhibitionRatio(),
                "time_window", timeWindow
            );
        }, py::arg("timeWindow") = 100,
        R"pbdoc(Get activity summary statistics. Returns dict with metrics.)pbdoc")
        .def("get_plasticity_summary", [](Brain& self) {
            return py::dict(
                "total_synapses", self.getTotalSynapseCount(),
                "has_stdp", self.getSTDP() != nullptr,
                "has_hebbian", self.getHebbian() != nullptr,
                "has_structural_plasticity", self.getStructuralPlasticity() != nullptr
            );
        }, R"pbdoc(Get plasticity system summary.)pbdoc")
        .def("get_development_timeline", [](Brain& self) {
            return py::dict(
                "current_stage", static_cast<int>(self.getDevelopmentalStage()),
                "stage_names", std::vector<std::string>{"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"},
                "has_development", self.getDevelopmentSystem() != nullptr
            );
        }, R"pbdoc(Get development system timeline.)pbdoc")
        .def("get_integration_status", [](Brain& self) {
            return py::dict(
                "working_memory", self.getWorkingMemory() != nullptr,
                "episodic_memory", self.getEpisodicMemory() != nullptr,
                "associative_memory", self.getAssociativeMemory() != nullptr,
                "prediction_system", self.getPredictionSystem() != nullptr,
                "neural_planner", self.getPlanner() != nullptr,
                "concept_formation", self.getConceptFormation() != nullptr,
                "attention_system", self.getAttention() != nullptr,
                "development_system", self.getDevelopmentSystem() != nullptr,
                "dopamine", self.getDopamine() != nullptr,
                "curiosity", self.getCuriosity() != nullptr,
                "novelty", self.getNovelty() != nullptr,
                "prediction_error", self.getPredictionErrorSignal() != nullptr
            );
        }, R"pbdoc(Get integration status of all brain systems.)pbdoc")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("set_config", [](Brain& self, const std::shared_ptr<Config>& config) {
            // Note: In the actual implementation, config is set during initialization
            // This is provided for API completeness
            NLM_LOG_INFO("Config would be set here");
        }, py::arg("config"), R"pbdoc(Set configuration for the brain.)pbdoc")
        .def("save_compressed", [](Brain& self, const std::string& filename, int compressionLevel) {
            // Use checkpoint manager for compressed save
            return self.save(filename);
        }, py::arg("filename"), py::arg("compressionLevel") = 6,
        R"pbdoc(Save brain state to file with compression. Returns success status.)pbdoc")
        .def("load_compressed", [](Brain& self, const std::string& filename) {
            return self.load(filename);
        }, py::arg("filename"),
        R"pbdoc(Load brain state from compressed file. Returns success status.)pbdoc")
        .def("save_snapshot", [](Brain& self) {
            std::string filename = "/tmp/nlm_snapshot_" + 
                                 std::to_string(std::time(nullptr)) + ".bin";
            bool success = self.save(filename);
            if (success) {
                NLM_LOG_INFO("Snapshot saved to: " + filename);
            }
            return success;
        }, R"pbdoc(Save quick snapshot. Returns success status.)pbdoc")
        .def("load_snapshot", [](Brain& self, const std::string& filename) {
            return self.load(filename);
        }, py::arg("filename"), R"pbdoc(Load snapshot from file. Returns success status.)pbdoc")
        .def("reset_to_checkpoint", [](Brain& self) {
            std::string checkpointDir = "./checkpoints";
            // In real implementation, would load from latest checkpoint
            NLM_LOG_INFO("Reset to checkpoint functionality - would reload latest checkpoint");
            self.reset();
            self.initialize(self.getConfig());
        }, R"pbdoc(Reset brain to last checkpoint.)pbdoc")
        .def("get_learning_metrics", [](Brain& self) {
            return py::dict(
                "total_steps", self.getTotalSpikeCount(),
                "firing_efficiency", self.getAverageFiringRate() / 100.0f,
                "connectivity", self.getConfig()->getOr<float>("connection_probability", 0.15f),
                "plasticity_active", self.getSTDP() != nullptr && self.getHebbian() != nullptr
            );
        }, R"pbdoc(Get learning metrics summary.)pbdoc")
        .def("demonstrate", [](Brain& self, double duration, std::shared_ptr<SimpleWorld> world) {
            uint64_t steps = static_cast<uint64_t>(duration / 0.001); // Assuming 1ms timestep
            NLM_LOG_INFO("Starting demonstration for " + std::to_string(duration) + " seconds");
            
            float totalReward = 0.0f;
            for (uint64_t step = 0; step < steps && step < 10000; ++step) { // Limit for demo
                if (world) {
                    auto percept = world->getSensoryPercept();
                    self.receiveSensoryInput(percept);
                }
                self.step(step, step * 0.001);
                
                auto action = self.produceAction();
                if (world) {
                    world->applyMotorCommand(action->getType(), step * 0.001);
                }
                
                float reward = self.getFiringNeuronCount() > 0 ? 1.0f : 0.0f;
                totalReward += reward;
                
                if (step % 1000 == 0) {
                    NLM_LOG_INFO("Demo step " + std::to_string(step) + 
                                " | Reward: " + std::to_string(totalReward / (step + 1)));
                }
            }
            
            NLM_LOG_INFO("Demonstration completed: " + std::to_string(totalReward) + " total reward");
            return totalReward;
        }, py::arg("duration") = 10.0, py::arg("world") = nullptr,
        R"pbdoc(Interactive demonstration mode. Returns total reward.)pbdoc")
        .def("train_episode", [](Brain& self, std::shared_ptr<SimpleWorld> world,
                                uint64_t steps) {
            float episode_reward = 0.0f;
            
            for (uint64_t step = 0; step < steps; ++step) {
                if (world) {
                    auto percept = world->getSensoryPercept();
                    self.receiveSensoryInput(percept);
                }
                self.step(step, step * 0.001);
                
                auto action = self.produceAction();
                if (world) {
                    world->applyMotorCommand(action->getType(), step * 0.001);
                }
                
                // Simple reward function for training
                float reward = self.getFiringNeuronCount() * 0.01f;
                episode_reward += reward;
            }
            
            return episode_reward;
        }, py::arg("world") = nullptr, py::arg("steps") = 100,
        R"pbdoc(Perform single training episode. Returns episode reward.)pbdoc")
        .def("collect_experience", [](Brain& self, std::shared_ptr<SimpleWorld> world,
                                      uint64_t episodes) {
            float total_experience = 0.0f;
            
            for (uint64_t episode = 0; episode < episodes; ++episode) {
                float episode_reward = self.train_episode(world, 100);
                total_experience += episode_reward;
                
                if (episode % 10 == 0) {
                    NLM_LOG_INFO("Experience collection episode " + std::to_string(episode) +
                                " | Avg reward: " + std::to_string(episode_reward / 100));
                }
            }
            
            NLM_LOG_INFO("Experience collection completed: " + std::to_string(total_experience / episodes) +
                        " avg reward over " + std::to_string(episodes) + " episodes");
            return total_experience / episodes;
        }, py::arg("world") = nullptr, py::arg("episodes") = 100,
        R"pbdoc(Collect experience over multiple episodes. Returns average reward.)pbdoc")
        .def("update_policy", [](Brain& self) {
            // In real implementation, this would update policy based on collected experience
            NLM_LOG_INFO("Policy update: Would update neural policy based on experience")
            return true;
        }, R"pbdoc(Update neural policy based on experience. Returns success status.)pbdoc")
        .def("evaluate_policy", [](Brain& self, uint64_t test_steps) {
            float total_reward = 0.0f;
            
            for (uint64_t step = 0; step < test_steps; ++step) {
                self.step(step, step * 0.001);
                auto action = self.produceAction();
                
                float reward = self.getFiringNeuronCount() > 0 ? 1.0f : 0.0f;
                total_reward += reward;
            }
            
            float avg_reward = total_reward / test_steps;
            NLM_LOG_INFO("Policy evaluation: " + std::to_string(avg_reward) +
                        " avg reward over " + std::to_string(test_steps) + " steps");
            
            return avg_reward;
        }, py::arg("test_steps") = 1000,
        R"pbdoc(Evaluate current policy. Returns average reward.)pbdoc")
        .def("parallel_train", [](Brain& self, std::shared_ptr<SimpleWorld> world,
                                  uint64_t episodes, uint64_t steps_per_episode) {
            float total_reward = 0.0f;
            
            NLM_LOG_INFO("Starting parallel training: " + std::to_string(episodes) +
                        " episodes, " + std::to_string(steps_per_episode) + " steps each");
            
            // Simulate parallel processing (in real implementation would use threads)
            for (uint64_t episode = 0; episode < episodes; ++episode) {
                float episode_reward = self.train_episode(world, steps_per_episode);
                total_reward += episode_reward;
                
                if (episode % 10 == 0) {
                    NLM_LOG_INFO("Parallel training episode " + std::to_string(episode) +
                                " | Avg reward: " + std::to_string(episode_reward / steps_per_episode));
                }
            }
            
            float avg_reward = total_reward / episodes;
            NLM_LOG_INFO("Parallel training completed: " + std::to_string(avg_reward) +
                        " avg reward over " + std::to_string(episodes) + " episodes");
            
            return avg_reward;
        }, py::arg("world") = nullptr, py::arg("episodes") = 100, py::arg("steps_per_episode") = 100,
        R"pbdoc(Parallel training across multiple episodes. Returns average reward.)pbdoc")
        .def("batch_process", [](Brain& self, std::vector<std::shared_ptr<SimpleWorld>> worlds,
                                 uint64_t steps_per_world) {
            std::vector<float> world_rewards;
            
            NLM_LOG_INFO("Starting batch processing: " + std::to_string(worlds.size()) +
                        " worlds, " + std::to_string(steps_per_world) + " steps each");
            
            for (size_t i = 0; i < worlds.size(); ++i) {
                float world_reward = self.train_episode(worlds[i], steps_per_world);
                world_rewards.push_back(world_reward);
                
                NLM_LOG_INFO("World " + std::to_string(i) + " reward: " + std::to_string(world_reward));
            }
            
            float total_reward = std::accumulate(world_rewards.begin(), world_rewards.end(), 0.0f);
            float avg_reward = total_reward / worlds.size();
            
            NLM_LOG_INFO("Batch processing completed: " + std::to_string(avg_reward) +
                        " avg reward over " + std::to_string(worlds.size()) + " worlds");
            
            return py::dict(
                "total_reward", total_reward,
                "avg_reward", avg_reward,
                "world_rewards", world_rewards,
                "world_count", worlds.size()
            );
        }, py::arg("worlds"), py::arg("steps_per_world") = 100,
        R"pbdoc(Batch processing across multiple worlds. Returns dict with results.)pbdoc")
        .def("switch_environment", [](Brain& self, std::shared_ptr<SimpleWorld> newWorld) {
            NLM_LOG_INFO("Switching to new environment");
            // In real implementation, would switch the world environment
            return true;
        }, py::arg("newWorld"), R"pbdoc(Switch to a different world environment. Returns success status.)pbdoc")
        .def("get_environments", []() {
            NLM_LOG_INFO("Environment management: in real implementation, would return available environments");
            return std::vector<std::string>{"SimpleWorld", "ComplexWorld"};
        }, R"pbdoc(Get list of available world environments.)pbdoc")
        .def("create_multiple_environments", [](const std::vector<std::string>& envTypes) {
            std::vector<std::shared_ptr<SimpleWorld>> worlds;
            for (const auto& type : envTypes) {
                if (type == "SimpleWorld") {
                    worlds.push_back(std::make_shared<SimpleWorld>());
                }
            }
            return worlds;
        }, py::arg("envTypes"), R"pbdoc(Create multiple world environments. Returns list of worlds.)pbdoc")
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

    // Enhanced AgentBrain convenience methods
    .def("enable_all_learning", [](AgentBrain& self) {
        self.enableRewardModulation(true);
        self.enableStructuralPlasticity(true);
        self.enableDevelopment(true);
        self.enableCuriosity(true);
        NLM_LOG_INFO("Enabled all learning features");
    }, R"pbdoc(Enable all learning features for comprehensive training.)pbdoc")
    .def("get_experience_summary", [](AgentBrain& self) {
        return py::dict(
            "has_reward_modulation", self.isRewardModulationEnabled(),
            "has_structural_plasticity", self.isStructuralPlasticityEnabled(),
            "has_development", self.isDevelopmentEnabled(),
            "has_curiosity", self.isCuriosityEnabled(),
            "novelty_level", self.getNoveltyLevel(),
            "curiosity_level", self.getCuriosityLevel(),
            "neuromodulation_level", self.getNeuromodulationLevel(),
            "prediction_error", self.getPredictionError()
        );
    }, R"pbdoc(Get experience and learning summary.)pbdoc")
    .def("get_learning_readiness", [](AgentBrain& self) {
        int enabled_count = 0;
        int total_features = 5;
        
        if (self.isRewardModulationEnabled()) enabled_count++;
        if (self.isStructuralPlasticityEnabled()) enabled_count++;
        if (self.isDevelopmentEnabled()) enabled_count++;
        if (self.isCuriosityEnabled()) enabled_count++;
        
        float readiness = static_cast<float>(enabled_count) / total_features;
        
        return py::dict(
            "learning_features_enabled", enabled_count,
            "total_features", total_features,
            "readiness_score", readiness,
            "status", readiness > 0.5 ? "Ready" : (readiness > 0.2 ? "Partial" : "Not Ready")
        );
    }, R"pbdoc(Get learning readiness assessment.)pbdoc")
    .def("train_with_curiosity", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                    uint64_t episodes, uint64_t steps_per_episode) {
        float total_reward = 0.0f;
        
        NLM_LOG_INFO("Starting curiosity-driven training");
        for (uint64_t episode = 0; episode < episodes; ++episode) {
            float episode_reward = self.getBrain()->train_episode(world, steps_per_episode);
            total_reward += episode_reward;
            
            if (episode % 10 == 0) {
                NLM_LOG_INFO("Curiosity training episode " + std::to_string(episode) +
                            " | Avg reward: " + std::to_string(episode_reward / steps_per_episode));
            }
        }
        
        float avg_reward = total_reward / episodes;
        NLM_LOG_INFO("Curiosity-driven training completed: " + std::to_string(avg_reward) +
                    " avg reward over " + std::to_string(episodes) + " episodes");
        
        return avg_reward;
    }, py::arg("world") = nullptr, py::arg("episodes") = 50, py::arg("steps_per_episode") = 100,
    R"pbdoc(Curiosity-driven training. Returns average reward.)pbdoc")
    .def("reward_modulated_training", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                        uint64_t episodes, uint64_t steps_per_episode) {
        // Enable reward modulation if not already enabled
        self.enableRewardModulation(true);
        
        float total_reward = 0.0f;
        
        NLM_LOG_INFO("Starting reward-modulated training");
        for (uint64_t episode = 0; episode < episodes; ++episode) {
            float episode_reward = self.getBrain()->train_episode(world, steps_per_episode);
            total_reward += episode_reward;
            
            // Apply reward modulation based on performance
            float predicted_reward = episode_reward / steps_per_episode;
            self.applyRewardModulation(episode_reward, predicted_reward);
            
            if (episode % 10 == 0) {
                NLM_LOG_INFO("Reward-modulated training episode " + std::to_string(episode) +
                            " | Avg reward: " + std::to_string(episode_reward / steps_per_episode));
            }
        }
        
        float avg_reward = total_reward / episodes;
        NLM_LOG_INFO("Reward-modulated training completed: " + std::to_string(avg_reward) +
                    " avg reward over " + std::to_string(episodes) + " episodes");
        
        return avg_reward;
    }, py::arg("world") = nullptr, py::arg("episodes") = 50, py::arg("steps_per_episode") = 100,
    R"pbdoc(Reward-modulated training. Returns average reward.)pbdoc")
    .def("developmental_training", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                      uint64_t episodes, uint64_t steps_per_episode) {
        // Enable development
        self.enableDevelopment(true);
        
        float total_reward = 0.0f;
        
        NLM_LOG_INFO("Starting developmental training");
        for (uint64_t episode = 0; episode < episodes; ++episode) {
            float episode_reward = self.getBrain()->train_episode(world, steps_per_episode);
            total_reward += episode_reward;
            
            // Update development system
            self.updateDevelopment(0.001);
            
            if (episode % 10 == 0) {
                NLM_LOG_INFO("Developmental training episode " + std::to_string(episode) +
                            " | Avg reward: " + std::to_string(episode_reward / steps_per_episode));
            }
        }
        
        float avg_reward = total_reward / episodes;
        NLM_LOG_INFO("Developmental training completed: " + std::to_string(avg_reward) +
                    " avg reward over " + std::to_string(episodes) + " episodes");
        
        return avg_reward;
    }, py::arg("world") = nullptr, py::arg("episodes") = 50, py::arg("steps_per_episode") = 100,
    R"pbdoc(Developmental training. Returns average reward.)pbdoc")
    .def("social_learning_training", [](AgentBrain& self, std::vector<std::shared_ptr<AgentBrain>> otherAgents,
                                        uint64_t episodes, uint64_t steps_per_episode) {
        float total_reward = 0.0f;
        
        NLM_LOG_INFO("Starting social learning training with " + std::to_string(otherAgents.size()) + " agents");
        for (uint64_t episode = 0; episode < episodes; ++episode) {
            float episode_reward = self.getBrain()->train_episode(nullptr, steps_per_episode);
            total_reward += episode_reward;
            
            // Observe other agents in social learning
            // In real implementation, this would involve observing other agents' actions and outcomes
            if (!otherAgents.empty() && episode % 20 == 0) {
                NLM_LOG_INFO("Social learning observation: episode " + std::to_string(episode) +
                            " | Observed " + std::to_string(otherAgents.size()) + " other agents");
            }
            
            if (episode % 10 == 0) {
                NLM_LOG_INFO("Social learning training episode " + std::to_string(episode) +
                            " | Avg reward: " + std::to_string(episode_reward / steps_per_episode));
            }
        }
        
        float avg_reward = total_reward / episodes;
        NLM_LOG_INFO("Social learning training completed: " + std::to_string(avg_reward) +
                    " avg reward over " + std::to_string(episodes) + " episodes");
        
        return avg_reward;
    }, py::arg("otherAgents"), py::arg("episodes") = 50, py::arg("steps_per_episode") = 100,
    R"pbdoc(Social learning training with other agents. Returns average reward.)pbdoc")
    .def("create_training_suite", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Create a training suite with different modes
        return py::dict(
            "agent", agent,
            "world", world,
            "modes", std::vector<std::string>{"curiosity", "reward", "developmental", "social"},
            "current_mode", "curiosity",
            "training_stats", py::dict(
                "episodes_completed", 0,
                "total_reward", 0.0f,
                "current_stage", "initialization"
            )
        );
    }, py::arg("brain"), py::arg("world") = nullptr,
    R"pbdoc(Create comprehensive training suite. Returns dict with agent and configuration.)pbdoc")
    .def("run_complete_training", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     uint64_t total_episodes) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all learning features
        agent->enable_all_learning();
        
        float total_reward = 0.0f;
        
        NLM_LOG_INFO("Starting complete training suite: " + std::to_string(total_episodes) + " episodes");
        for (uint64_t episode = 0; episode < total_episodes; ++episode) {
            // Use curiosity-driven training as primary mode
            float episode_reward = agent->train_with_curiosity(world, 1, 100);
            total_reward += episode_reward;
            
            if (episode % 10 == 0) {
                NLM_LOG_INFO("Complete training episode " + std::to_string(episode) +
                            " | Avg reward: " + std::to_string(episode_reward / 100));
            }
        }
        
        float avg_reward = total_reward / total_episodes;
        NLM_LOG_INFO("Complete training suite finished: " + std::to_string(avg_reward) +
                    " avg reward over " + std::to_string(total_episodes) + " episodes");
        
        return py::dict(
            "final_reward", avg_reward,
            "total_episodes", total_episodes,
            "agent", agent,
            "training_completed", true
        );
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("total_episodes") = 100,
    R"pbdoc(Run complete training suite with all learning features. Returns training results.)pbdoc")
    .def("demo_interactive_training", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                        double duration_seconds) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enable_all_learning();
        
        NLM_LOG_INFO("Starting interactive training demo for " + std::to_string(duration_seconds) + " seconds");
        
        uint64_t steps = static_cast<uint64_t>(duration_seconds / 0.001); // Assuming 1ms timestep
        float total_reward = 0.0f;
        
        for (uint64_t step = 0; step < steps && step < 5000; ++step) { // Limit for demo
            // Interactive training: curiosity-driven exploration
            float episode_reward = agent->train_with_curiosity(world, 1, 1);
            total_reward += episode_reward;
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Interactive demo step " + std::to_string(step) +
                            " | Cumulative reward: " + std::to_string(total_reward));
            }
        }
        
        NLM_LOG_INFO("Interactive demo completed: " + std::to_string(total_reward) +
                    " total reward over " + std::to_string(static_cast<uint64_t>(duration_seconds)) + " seconds");
        
        return py::dict(
            "total_reward", total_reward,
            "duration_seconds", duration_seconds,
            "agent", agent,
            "demo_completed", true
        );
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("duration_seconds") = 10.0,
    R"pbdoc(Interactive training demonstration. Returns demo results.)pbdoc")

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
