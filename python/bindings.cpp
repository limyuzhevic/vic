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
        .def("validate", &Config::validate,
             "Validate configuration values and return error messages if any")
        .def("validateOrThrow", [](const Config& self) {
            auto errors = self.validate();
            if (!errors.empty()) {
                std::string errorMsg = "Configuration validation failed:\n";
                for (const auto& error : errors) {
                    errorMsg += "  - " + error + "\n";
                }
                throw std::runtime_error(errorMsg);
            }
        }, "Validate configuration and throw exception if invalid")
        .def("set", [](Config& self, const std::string& key, const py::object& value, ConfigSource source = ConfigSource::Runtime) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, py::cast<int64_t>(value), source);
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, py::cast<float>(value), source);
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, py::cast<bool>(value), source);
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, py::cast<std::string>(value), source);
            } else if (py::isinstance<py::list>(value)) {
                auto list = value.cast<std::vector<float>>();
                self.set(key, ConfigValue(list), source);
            } else {
                throw std::runtime_error("Unsupported configuration value type");
            }
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with type validation")
        .def("get", [](const Config& self, const std::string& key) {
            if (self.has(key)) {
                if (self.get<int64_t>(key).has_value()) {
                    return py::cast(self.get<int64_t>(key).value());
                } else if (self.get<double>(key).has_value()) {
                    return py::cast(self.get<double>(key).value());
                } else if (self.get<float>(key).has_value()) {
                    return py::cast(self.get<float>(key).value());
                } else if (self.get<bool>(key).has_value()) {
                    return py::cast(self.get<bool>(key).value());
                } else if (self.get<std::string>(key).has_value()) {
                    return py::cast(self.get<std::string>(key).value());
                }
            }
            throw std::runtime_error("Key not found: " + key);
        }, py::arg("key"), "Get configuration value with type inference")
        .def("getOr", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            if (py::isinstance<py::int_>(defaultValue)) {
                return py::cast(self.getOr<int64_t>(key, py::cast<int64_t>(defaultValue)));
            } else if (py::isinstance<py::float_>(defaultValue)) {
                return py::cast(self.getOr<double>(key, py::cast<double>(defaultValue)));
            } else if (py::isinstance<py::bool_>(defaultValue)) {
                return py::cast(self.getOr<bool>(key, py::cast<bool>(defaultValue)));
            } else if (py::isinstance<py::str>(defaultValue)) {
                return py::cast(self.getOr<std::string>(key, py::cast<std::string>(defaultValue)));
            }
            throw std::runtime_error("Unsupported default value type");
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default and type inference")
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
        .def("getDuration", &Action::getDuration)
        .def("setDuration", &Action::setDuration, py::arg("duration"))
        .def("getEnergyCost", &Action::getEnergyCost)
        .def("setEnergyCost", &Action::setEnergyCost, py::arg("cost"))
        .def("getTimestamp", &Action::getTimestamp)
        .def("setTimestamp", &Action::setTimestamp, py::arg("timestamp"))
        .def("isValid", &Action::isValid)
        .def("toString", [](const Action& a) {
            std::stringstream ss;
            ss << "Action[Type=" << a.getName() 
               << ", Duration=" << a.getDuration()
               << ", Energy=" << a.getEnergyCost() << "]";
            return ss.str();
        }, "Get string representation");

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
        .def("batchStep", [](Brain& self, size_t numSteps, bool logProgress = false) {
            for (size_t i = 0; i < numSteps; ++i) {
                self.step(i);
                if (logProgress && (i % 100 == 0)) {
                    std::cout << "Step " << i << ": " 
                              << self.getFiringNeuronCount() << " firing neurons\n";
                }
            }
        }, py::arg("numSteps"), py::arg("logProgress") = false,
             "Perform multiple steps in batch")
        .def("batchInjectCurrent", [](Brain& self, const std::vector<NeuronId>& neuronIds, float current) {
            for (const auto& id : neuronIds) {
                self.injectCurrent(id, current);
            }
        }, py::arg("neuronIds"), py::arg("current"),
             "Inject current to multiple neurons")
        .def("getNeuronStats", [](const Brain& self) {
            return std::make_tuple(self.getTotalNeuronCount(),
                                   self.getActiveNeuronCount(),
                                   self.getFiringNeuronCount(),
                                   self.getTotalSpikeCount());
        }, "Get comprehensive neuron statistics")
        .def("enableSystem", [](Brain& self, const std::string& systemName, bool enable) {
            auto config = self.getConfig();
            std::string key = "system." + systemName + ".enabled";
            config->set(key, enable);
        }, py::arg("systemName"), py::arg("enable"),
             "Enable/disable a system")
        .def("getSystemStatus", [](const Brain& self) {
            auto config = self.getConfig();
            std::map<std::string, bool> status;
            for (const auto& key : config->getKeys()) {
                if (key.find("system.") == 0 && key.find(".enabled") != std::string::npos) {
                    std::string systemName = key.substr(7, key.find(".enabled") - 8);
                    status[systemName] = config->getOr<bool>(key, false);
                }
            }
            return status;
        }, "Get status of all systems")
        .def("getPerformanceMetrics", [](const Brain& self) {
            std::map<std::string, double> metrics;
            metrics["neuron_count"] = self.getTotalNeuronCount();
            metrics["firing_rate"] = self.getAverageFiringRate();
            metrics["e_i_ratio"] = self.getExcitationInhibitionRatio();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            metrics["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get performance metrics");

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
        .def("processBatchSensoryInput", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                self.processSensoryInput(percept);
            }
        }, py::arg("percepts"),
             "Process multiple sensory inputs in batch")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("getActions", [](AgentBrain& self, size_t numActions) {
            std::vector<Action> actions;
            for (size_t i = 0; i < numActions; ++i) {
                actions.push_back(self.decodeMotorCommand());
            }
            return actions;
        }, py::arg("numActions"),
             "Generate multiple actions in batch")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyBatchRewardModulation", [](AgentBrain& self, const std::vector<std::pair<float, float>>& rewards) {
            for (const auto& reward : rewards) {
                self.applyRewardModulation(reward.first, reward.second);
            }
        }, py::arg("rewards"),
             "Apply multiple reward modulations in batch")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("updateBatchDevelopment", [](AgentBrain& self, const std::vector<double>& timesteps) {
            for (double t : timesteps) {
                self.updateDevelopment(t);
            }
        }, py::arg("timesteps"),
             "Update development system for multiple timesteps")
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
        .def("getLearningStats", [](AgentBrain& self) {
            std::map<std::string, double> stats;
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive learning statistics")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("resetBatch", [](AgentBrain& self, const std::vector<size_t>& episodeCounts) {
            for (size_t count : episodeCounts) {
                self.reset();
                for (size_t i = 0; i < count; ++i) {
                    self.reset();
                }
            }
        }, py::arg("episodeCounts"),
             "Reset agent for multiple episodes")
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
        .def("enableAllSystems", [](AgentBrain& self, bool enable) {
            self.enableRewardModulation(enable);
            self.enableStructuralPlasticity(enable);
            self.enableDevelopment(enable);
            self.enableCuriosity(enable);
        }, py::arg("enable"),
             "Enable/disable all systems")
        .def("getAllSystemStates", [](AgentBrain& self) {
            std::map<std::string, bool> states;
            states["reward_modulation"] = self.isRewardModulationEnabled();
            states["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            states["development"] = self.isDevelopmentEnabled();
            states["curiosity"] = self.isCuriosityEnabled();
            return states;
        }, "Get state of all systems")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Create configuration from file")
    .def("createConfigFromDict", [](const py::dict& dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto it : dict) {
            std::string key = py::cast<std::string>(it.first);
            auto value = it.second;
            if (py::isinstance<py::int_>(value)) {
                config->set(key, py::cast<int64_t>(value));
            } else if (py::isinstance<py::float_>(value)) {
                config->set(key, py::cast<float>(value));
            } else if (py::isinstance<py::bool_>(value)) {
                config->set(key, py::cast<bool>(value));
            } else if (py::isinstance<py::str>(value)) {
                config->set(key, py::cast<std::string>(value));
            }
        }
        return config;
    }, py::arg("dict"), "Create configuration from dictionary");

    m.def("create_experiment_config", [](const std::string& type) {
        auto config = std::make_shared<Config>();
        
        if (type == "learning") {
            config->set("brain.neuron_count", 500);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
        } else if (type == "structural") {
            config->set("brain.neuron_count", 200);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.stdp.enable", true);
            config->set("development.enable", true);
        } else if (type == "prediction") {
            config->set("brain.neuron_count", 300);
            config->set("plasticity.stdp.enable", true);
            config->set("prediction.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else {
            config->set("brain.neuron_count", 100);
        }
        
        return config;
    }, py::arg("type"), "Create experiment configuration of specified type");

    // Constants for Python API
    m.attr("DEFAULT_CONFIG") = py::dict({
        {"brain.neuron_count", 100},
        {"brain.synapse_density", 0.1},
        {"plasticity.stdp.enable", true},
        {"plasticity.hebbian.enable", true},
        {"neuromod.curiosity.enable", true},
        {"neuromod.novelty.enable", true}
    });

    m.attr("VERSION") = "1.0.0";
    m.attr("BUILD_DATE") = __DATE__;
    m.attr("BUILD_TIME") = __TIME__;

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
