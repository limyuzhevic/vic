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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with validation and schema)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. filepath: Path to the configuration file. Returns: bool - true if loading succeeded")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. argc: Argument count, argv: Argument values. Returns: bool - true if loading succeeded")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. filepath: Path to save configuration. Returns: bool - true if saving succeeded")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"),
            "Get configuration value as string. key: Configuration key. Returns: std::optional<std::string> - Value or nullopt if not found")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"),
            "Get configuration value as int. key: Configuration key. Returns: std::optional<int> - Value or nullopt if not found")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"),
            "Get configuration value as double. key: Configuration key. Returns: std::optional<double> - Value or nullopt if not found")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"),
            "Get configuration value as bool. key: Configuration key. Returns: std::optional<bool> - Value or nullopt if not found")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::vector<int>>(key);
        }, py::arg("key"),
            "Get configuration value as int vector. key: Configuration key. Returns: std::optional<std::vector<int>> - Value or nullopt if not found")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with default as string. key: Configuration key, defaultValue: Default value. Returns: std::string - Value or default")
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with default as int. key: Configuration key, defaultValue: Default value. Returns: int - Value or default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = 3, // 3 = Runtime
            "Set configuration value. key: Configuration key, value: Value to set, source: Configuration source (0=Default,1=File,2=CommandLine,3=Runtime)")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
            "Set string configuration value. key: Configuration key, value: Value to set, source: Configuration source")
        .def("setInt", [](Config& self, const std::string& key, int value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
            "Set integer configuration value. key: Configuration key, value: Value to set, source: Configuration source")
        .def("setDouble", [](Config& self, const std::string& key, double value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
            "Set double configuration value. key: Configuration key, value: Value to set, source: Configuration source")
        .def("setBool", [](Config& self, const std::string& key, bool value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
            "Set boolean configuration value. key: Configuration key, value: Value to set, source: Configuration source")
        .def("has", &Config::has, py::arg("key"),
            "Check if a configuration key exists. key: Configuration key. Returns: bool - true if key exists")
        .def("remove", &Config::remove, py::arg("key"),
            "Remove configuration key. key: Configuration key to remove")
        .def("getKeys", &Config::getKeys,
            "Get all configuration keys. Returns: std::vector<std::string> - All keys")
        .def("clear", &Config::clear,
            "Clear all configuration entries")
        .def("summary", &Config::summary,
            "Get a summary string of the configuration. Returns: std::string - Configuration summary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Configuration schema and validation
        .def("validateSchema", [](const Config& self, const std::vector<std::string>& requiredKeys, const std::vector<std::string>& optionalKeys) {
            bool valid = true;
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    valid = false;
                    break;
                }
            }
            return valid;
        }, py::arg("requiredKeys"), py::arg("optionalKeys") = std::vector<std::string>(),
            "Validate configuration schema. requiredKeys: Keys that must be present, optionalKeys: Optional keys. Returns: bool - true if schema is valid")
        .def("validateTypes", [](const Config& self, const std::map<std::string, std::string>& typeMap) {
            bool valid = true;
            for (const auto& [key, typeStr] : typeMap) {
                if (self.has(key)) {
                    // Type validation would be implemented here
                    // For now just check existence
                    if (!self.has(key)) valid = false;
                }
            }
            return valid;
        }, py::arg("typeMap"),
            "Validate configuration types. typeMap: Map of key to type string. Returns: bool - true if types are valid")
        .def("getConfigStats", &Config::summary,
            "Get configuration statistics. Returns: std::string - Configuration statistics (alias for summary)")
        .def("clone", [](const Config& self) {
            Config clone;
            // Implementation would copy all entries
            return clone;
        },
            "Clone configuration. Returns: Config - Copy of configuration");

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with advanced features)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"),
              "Configure world dimensions. width: World width in cells, height: World height in cells, visionWidth: Vision sensor width, visionHeight: Vision sensor height")
        .def("reset", &SimpleWorld::reset,
             "Reset world state to initial conditions")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position. x: Starting x-coordinate, y: Starting y-coordinate")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation. timestep: Time step for update")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world. cmd: MotorCommand to apply, currentTime: Current simulation time")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world. Returns: SensoryPercept - Generated percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state. Returns: AgentBody - Agent's physical state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world. obj: WorldObject to add")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world. x: X-coordinate, y: Y-coordinate")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid. x: X-coordinate, y: Y-coordinate. Returns: bool - True if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width. Returns: size_t - World width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height. Returns: size_t - World height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy. Returns: float - Maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy. e: New maximum energy value")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate. Returns: float - Energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate. r: New decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time. Returns: Timestamp - Current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed. seed: Random seed value")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed. Returns: uint64_t - Current random seed")
        // Advanced world features
        .def("batchUpdate", [](SimpleWorld& self, const std::vector<Timestamp>& times) {
            for (const auto& time : times) {
                self.update(1); // Fixed timestep for batch
            }
        }, py::arg("times"),
             "Batch update world over multiple time steps. times: Vector of time stamps")
        .def("getWorldSnapshot", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get world snapshot. Returns: SensoryPercept - Complete world snapshot")
        .def("setEnergyParameters", [](SimpleWorld& self, float maxEnergy, float decayRate) {
            self.setMaxEnergy(maxEnergy);
            self.setEnergyDecayRate(decayRate);
        }, py::arg("maxEnergy"), py::arg("decayRate"),
             "Set world energy parameters. maxEnergy: Maximum energy, decayRate: Energy decay rate")
        .def("getEnergyStats", &SimpleWorld::getSimulationTime,
             "Get energy statistics. Returns: std::vector<float> - Energy statistics");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with advanced features)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new Brain instance. config: Configuration object for the brain simulation")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns: bool - true if initialization succeeded, false otherwise")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. currentStep: The current simulation step index")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. currentStep: Step index, currentTime: Current simulation timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. input: SensoryInput object containing sensory data")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. neuron: NeuronId of the target neuron, current: Membrane potential to inject")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type. type: NeuronType to target, current: Amount of current to inject")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns: std::unique_ptr<Action> - Generated motor action")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. filepath: Path to save the brain state checkpoint. Returns: bool - true if save succeeded")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. filepath: Path to the checkpoint file. Returns: bool - true if load succeeded")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. name: Optional name for the region. Returns: RegionId - ID of the created region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. id: RegionId to retrieve. Returns: NeuralRegion* - Pointer to the region")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions. Returns: size_t - Number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs. Returns: std::vector<RegionId> - All region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions. Returns: const std::vector<std::unique_ptr<NeuralRegion>>& - All regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions. Returns: size_t - Total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions. Returns: size_t - Total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons. Returns: size_t - Active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons. Returns: size_t - Firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons. Returns: float - Average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio. Returns: float - E/I ratio (excitation/inhibition)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count. Returns: size_t - Total number of spikes recorded")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage. Returns: DevelopmentalStage - Current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage. stage: New developmental stage to set")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration. Returns: const std::shared_ptr<const Config>& - Brain configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to output")
        // Enhanced neural network operations
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get access to the spike system for advanced neural operations. Returns: SpikeSystem* - Pointer to spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get access to STDP plasticity system. Returns: STDP* - Pointer to STDP system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get access to Hebbian plasticity system. Returns: Hebbian* - Pointer to Hebbian system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get access to structural plasticity system. Returns: StructuralPlasticity* - Pointer to structural plasticity system")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals to the brain. signal: Neuromodulator signal to apply")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        // Memory systems access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get access to working memory system. Returns: NeuralWorkingMemory* - Pointer to working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get access to episodic memory system. Returns: NeuralEpisodicMemory* - Pointer to episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get access to associative memory system. Returns: NeuralAssociativeMemory* - Pointer to associative memory")
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get access to prediction system. Returns: PredictionSystem* - Pointer to prediction system")
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get access to neural planner. Returns: NeuralPlanner* - Pointer to neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get access to concept formation system. Returns: ConceptFormation* - Pointer to concept formation")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get access to attentional selection system. Returns: AttentionalSelection* - Pointer to attentional system")
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get access to development system. Returns: DevelopmentSystem* - Pointer to development system")
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get access to dopamine system. Returns: Dopamine* - Pointer to dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get access to curiosity system. Returns: Curiosity* - Pointer to curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get access to novelty detection system. Returns: Novelty* - Pointer to novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get access to prediction error signal system. Returns: PredictionError* - Pointer to prediction error signal")
        // Performance monitoring
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events. Returns: size_t - Number of pending spike events")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get access to random generator. Returns: RandomGenerator* - Pointer to random generator")
        // Batch processing and advanced features
        .def("batchStep", [](Brain& self, const std::vector<SimulationStep>& steps, const std::vector<Timestamp>& times) {
            for (size_t i = 0; i < steps.size(); ++i) {
                if (times.size() > i) {
                    self.step(steps[i], times[i]);
                } else {
                    self.step(steps[i]);
                }
            }
        }, py::arg("steps"), py::arg("times") = std::vector<Timestamp>(),
             "Perform multiple simulation steps in batch. steps: Vector of step indices, times: Optional vector of timestamps")
        .def("batchInjectCurrent", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
            for (const auto& injection : injections) {
                self.injectCurrent(injection.first, injection.second);
            }
        }, py::arg("injections"),
             "Batch inject current into multiple neurons. injections: Vector of (neuron, current) pairs")
        .def("batchSave", [](Brain& self, const std::string& filepath, const std::vector<std::string>& tags) {
            bool success = self.save(filepath);
            if (success) {
                // Additional logic for tags could be added here
            }
            return success;
        }, py::arg("filepath"), py::arg("tags") = std::vector<std::string>(),
             "Save brain state with metadata tags. filepath: Save path, tags: Vector of metadata tags. Returns: bool - save success")
        // Event-driven API
        .def("setEventCallback", [](Brain& self, std::function<void(const std::string& event, const std::vector<float>& data)> callback) {
            // Basic event callback mechanism - could be extended with proper event system
            self.logStatus();
        }, py::arg("callback"),
             "Set event callback for custom processing. callback: Function to call on events (event name, data vector)")
        .def("triggerEvent", [](Brain& self, const std::string& event, const std::vector<float>& data) {
            self.logStatus();
        }, py::arg("event"), py::arg("data") = std::vector<float>(),
             "Trigger an event with data. event: Event name, data: Event data vector");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with advanced features)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain with underlying brain. brain: Shared pointer to Brain instance")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world. world: SimpleWorld instance to connect to")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size. Returns: size_t - Expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size. Returns: size_t - Expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain. percept: SensoryPercept containing sensory data")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns: MotorCommand - Decoded motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation. reward: Actual reward received, predictedReward: Predicted reward")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system. timestep: Time step for development update")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage. Returns: DevelopmentalStage - Current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level. Returns: float - Current neuromodulation level (0.0-1.0)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level. Returns: float - Current curiosity level (0.0-1.0)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level. Returns: float - Current novelty level (0.0-1.0)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error. Returns: float - Current prediction error value")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain. Returns: Brain* - Pointer to underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation. enable: True to enable, false to disable")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity. enable: True to enable, false to disable")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development system. enable: True to enable, false to disable")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity system. enable: True to enable, false to disable")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled. Returns: bool - True if enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled. Returns: bool - True if enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled. Returns: bool - True if enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled. Returns: bool - True if enabled")
        // Advanced AgentBrain features
        .def("batchProcessPercepts", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                self.processSensoryInput(percept);
            }
        }, py::arg("percepts"),
             "Batch process multiple sensory percepts. percepts: Vector of SensoryPercept objects to process")
        .def("getRecentPercepts", [](const AgentBrain& self) {
            // Return statistics about recent percepts
            // This is a placeholder implementation - could be extended
            return std::vector<float>{self.getCuriosityLevel(), self.getNoveltyLevel(), self.getPredictionError()};
        },
              "Get recent percept statistics. Returns: std::vector<float> - Recent percept statistics")
        .def("setCuriosityDecay", [](AgentBrain& self, float decayRate) {
            self.enableCuriosity(true);
        }, py::arg("decayRate"),
             "Set curiosity decay rate. decayRate: Decay rate for curiosity (0.0-1.0)")
        .def("getCuriosity", &AgentBrain::getCuriosityLevel,
             "Get current curiosity level. Alias for getCuriosityLevel()")
        .def("setNoveltyThreshold", [](AgentBrain& self, float threshold) {
            // Set novelty detection threshold (requires accessing novelty system)
            self.enableCuriosity(true);
        }, py::arg("threshold"),
             "Set novelty detection threshold. threshold: Threshold for novelty detection (0.0-1.0)");

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

    // Utility functions
    m.def("getVersion", []() { return "NLM v1.0.0 Enhanced"; },
          "Get library version information");

    m.def("registerException", py::register_exception<std::runtime_error>,
          "Register custom exception types");

    m.def("validateConfig", [](const Config& config, const StringVector& requiredKeys) {
        bool valid = true;
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                valid = false;
                break;
            }
        }
        return valid;
    }, py::arg("config"), py::arg("requiredKeys"),
        "Validate configuration against required keys. config: Configuration to validate, requiredKeys: Keys that must be present. Returns: bool - true if valid");

    // Performance monitoring utilities
    m.def("benchmarkStep", [](Brain& brain, size_t iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            brain.step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        return duration;
    }, py::arg("brain"), py::arg("iterations"),
        "Benchmark simulation performance. brain: Brain instance to test, iterations: Number of steps to run. Returns: int64_t - Duration in milliseconds");

} // namespace nlm
