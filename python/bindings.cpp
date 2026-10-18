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
        .def("batch_set", [](Config& self, py::dict config_dict) {
            for (auto item : config_dict) {
                std::string key = item.first.cast<std::string>();
                py::object value_obj = item.second;
                
                // Convert Python types to ConfigValue
                if (value_obj.is_none()) {
                    self.set(key, ConfigValue());
                } else if (value_obj.is(py::int_())) {
                    int64_t val = value_obj.cast<int64_t>();
                    self.set(key, val);
                } else if (value_obj.is(py::float_())) {
                    double val = value_obj.cast<double>();
                    self.set(key, val);
                } else if (value_obj.is(py::bool_())) {
                    bool val = value_obj.cast<bool>();
                    self.set(key, val);
                } else if (value_obj.is(py::str())) {
                    std::string val = value_obj.cast<std::string>();
                    self.set(key, val);
                } else if (value_obj.is(py::list())) {
                    auto py_list = value_obj.cast<std::vector<double>>();
                    self.set(key, py_list);
                } else {
                    throw std::runtime_error("Unsupported value type in config dictionary for key: " + key);
                }
            }
        }, py::arg("config_dict"),
            "Set multiple configuration values from a Python dictionary")
        .def("getAsDict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value_opt = self.get<std::string>(key);
                if (value_opt) {
                    result[key.c_str()] = *value_opt;
                } else {
                    auto int_opt = self.get<int64_t>(key);
                    if (int_opt) {
                        result[key.c_str()] = *int_opt;
                    } else {
                        auto double_opt = self.get<double>(key);
                        if (double_opt) {
                            result[key.c_str()] = *double_opt;
                        } else {
                            auto bool_opt = self.get<bool>(key);
                            if (bool_opt) {
                                result[key.c_str()] = *bool_opt;
                            }
                        }
                    }
                }
            }
            return result;
        }, "Get configuration as Python dictionary")
        .def("validate", [](const Config& self) {
            auto keys = self.getKeys();
            std::vector<std::string> errors;
            
            // Check for required basic settings
            if (!self.has("brain.neuron_count") && !self.has("neuron_count")) {
                errors.push_back("Missing required configuration: neuron_count");
            }
            if (!self.has("brain.synapse_density") && !self.has("connection_probability")) {
                errors.push_back("Missing configuration: connection_probability");
            }
            
            return errors;
        }, "Validate configuration and return list of errors (empty if valid)")
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
        .def("configure", [](SimpleWorld& self, py::kwargs kwargs) {
            float width = kwargs.contains("width") ? kwargs["width"].cast<float>() : 10.0f;
            float height = kwargs.contains("height") ? kwargs["height"].cast<float>() : 10.0f;
            size_t visionWidth = kwargs.contains("visionWidth") ? kwargs["visionWidth"].cast<size_t>() : 8;
            size_t visionHeight = kwargs.contains("visionHeight") ? kwargs["visionHeight"].cast<size_t>() : 8;
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("kwargs") = py::kwargs(),
             "Configure world with keyword arguments (width, height, visionWidth, visionHeight)")
        .def("setup", [](SimpleWorld& self, py::kwargs kwargs) {
            // Convenience method with sensible defaults
            float width = kwargs.contains("width") ? kwargs["width"].cast<float>() : 20.0f;
            float height = kwargs.contains("height") ? kwargs["height"].cast<float>() : 20.0f;
            size_t visionWidth = kwargs.contains("visionWidth") ? kwargs["visionWidth"].cast<size_t>() : 8;
            size_t visionHeight = kwargs.contains("visionHeight") ? kwargs["visionHeight"].cast<size_t>() : 8;
            
            self.configure(width, height, visionWidth, visionHeight);
            
            // Set reasonable defaults
            self.setMaxEnergy(kwargs.contains("maxEnergy") ? kwargs["maxEnergy"].cast<float>() : 100.0f);
            self.setEnergyDecayRate(kwargs.contains("energyDecay") ? kwargs["energyDecay"].cast<float>() : 0.01f);
            
            // Set agent start position if provided
            if (kwargs.contains("startX") && kwargs.contains("startY")) {
                self.setAgentStart(kwargs["startX"].cast<float>(), kwargs["startY"].cast<float>());
            } else {
                self.setAgentStart(width/2.0f, height/2.0f);
            }
            
            // Set random seed if provided
            if (kwargs.contains("seed")) {
                self.setRandomSeed(kwargs["seed"].cast<uint64_t>());
            }
            
            return self;
        }, py::arg("kwargs") = py::kwargs(),
             "Configure world with keyword arguments and set up agent")
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
        .def("describe", [](const SimpleWorld& self) {
            std::stringstream ss;
            ss << "World State:" << std::endl;
            ss << "  Dimensions: " << self.getWidth() << " x " << self.getHeight() << std::endl;
            ss << "  Vision: " << self.getVisionWidth() << " x " << self.getVisionHeight() << std::endl;
            ss << "  Simulation Time: " << self.getSimulationTime() << std::endl;
            ss << "  Max Energy: " << self.getMaxEnergy() << std::endl;
            ss << "  Energy Decay: " << self.getEnergyDecayRate() << std::endl;
            ss << "  Random Seed: " << self.getRandomSeed() << std::endl;
            ss << "  Agent Position: (" << self.getAgentBody().x << ", " << self.getAgentBody().y << ")" << std::endl;
            ss << "  Agent Energy: " << self.getAgentBody().energy << "/" << self.getMaxEnergy() << std::endl;
            return ss.str();
        }, "Return human-readable description of world state")
        .def("getVisionWidth", &SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &SimpleWorld::getVisionHeight);

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
        // Pythonic convenience methods
        .def("enableLearning", [](Brain& self) {
            auto config = self.getConfig();
            if (config) {
                config->set("plasticity.stdp.enable", true);
                config->set("plasticity.hebbian.enable", true);
                config->set("neuromod.dopamine.scale", 1.0f);
                config->set("neuromod.curiosity.enable", true);
                config->set("neuromod.novelty.enable", true);
            }
        }, "Enable all learning subsystems")
        .def("getStats", [](const Brain& self) {
            py::dict stats;
            stats["neurons"] = self.getTotalNeuronCount();
            stats["synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get brain statistics as Python dictionary")
        .def("describe", [](const Brain& self) {
            std::stringstream ss;
            ss << "Brain State:" << std::endl;
            ss << "  Total Neurons: " << self.getTotalNeuronCount() << std::endl;
            ss << "  Total Synapses: " << self.getTotalSynapseCount() << std::endl;
            ss << "  Active Neurons: " << self.getActiveNeuronCount() << std::endl;
            ss << "  Firing Neurons: " << self.getFiringNeuronCount() << std::endl;
            ss << "  Total Spikes: " << self.getTotalSpikeCount() << std::endl;
            ss << "  Average Firing Rate: " << self.getAverageFiringRate() << " Hz" << std::endl;
            ss << "  Excitation/Inhibition Ratio: " << self.getExcitationInhibitionRatio() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            ss << "  Region Count: " << self.getRegionCount() << std::endl;
            return ss.str();
        }, "Return human-readable description of brain state")
        .def("getNeuroplasticityStatus", [](const Brain& self) {
            py::dict status;
            auto config = self.getConfig();
            if (config) {
                status["stdp_enabled"] = config->getOr<bool>("plasticity.stdp.enable", true);
                status["hebbian_enabled"] = config->getOr<bool>("plasticity.hebbian.enable", true);
                status["structural_enabled"] = config->getOr<bool>("plasticity.structural.enable", true);
                status["dopamine_scale"] = config->getOr<float>("neuromod.dopamine.scale", 1.0f);
            }
            return status;
        }, "Get neuroplasticity system status");

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
        .def("setup", [](AgentBrain& self, const SimpleWorld& world, py::kwargs kwargs) {
            self.initialize(world);
            
            // Enable subsystems if requested
            if (kwargs.contains("enable_reward_modulation")) {
                self.enableRewardModulation(kwargs["enable_reward_modulation"].cast<bool>());
            }
            if (kwargs.contains("enable_structural_plasticity")) {
                self.enableStructuralPlasticity(kwargs["enable_structural_plasticity"].cast<bool>());
            }
            if (kwargs.contains("enable_development")) {
                self.enableDevelopment(kwargs["enable_development"].cast<bool>());
            }
            if (kwargs.contains("enable_curiosity")) {
                self.enableCuriosity(kwargs["enable_curiosity"].cast<bool>());
            }
            
            return self;
        }, py::arg("world"), py::arg("kwargs") = py::kwargs(),
             "Initialize agent with world and optionally enable subsystems")
        .def("enableAll", [](AgentBrain& self, bool enable) {
            self.enableRewardModulation(enable);
            self.enableStructuralPlasticity(enable);
            self.enableDevelopment(enable);
            self.enableCuriosity(enable);
        }, py::arg("enable"),
             "Enable or disable all neuromodulation subsystems at once")
        .def("getMonitoring", [](const AgentBrain& self) {
            py::dict monitoring;
            monitoring["neuromodulation_level"] = self.getNeuromodulationLevel();
            monitoring["curiosity_level"] = self.getCuriosityLevel();
            monitoring["novelty_level"] = self.getNoveltyLevel();
            monitoring["prediction_error"] = self.getPredictionError();
            monitoring["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            monitoring["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            monitoring["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            monitoring["development_enabled"] = self.isDevelopmentEnabled();
            monitoring["curiosity_enabled"] = self.isCuriosityEnabled();
            return monitoring;
        }, "Get all neuromodulation and monitoring data as Python dictionary")
        .def("processAndAct", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            // Note: This doesn't produce action, just processes input
            return self;
        }, py::arg("percept"),
             "Process sensory input (convenience method)")
        .def("getBrainStats", [](AgentBrain& self) {
            if (auto brain = self.getBrain()) {
                return brain->getStats();
            }
            return py::dict();
        }, "Get brain statistics through agent interface");

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

    // Pythonic convenience factory functions
    m.def("createQuickAgent", [](py::kwargs kwargs) {
        // Extract parameters with defaults
        size_t neuronCount = kwargs.contains("neuron_count") ? 
            kwargs["neuron_count"].cast<size_t>() : 1000;
        size_t steps = kwargs.contains("steps") ? 
            kwargs["steps"].cast<size_t>() : 100;
        bool enableLearning = kwargs.contains("enable_learning") ? 
            kwargs["enable_learning"].cast<bool>() : true;
        bool enableRewardMod = kwargs.contains("enable_reward_modulation") ? 
            kwargs["enable_reward_modulation"].cast<bool>() : true;
        bool enableCuriosity = kwargs.contains("enable_curiosity") ? 
            kwargs["enable_curiosity"].cast<bool>() : true;
        bool enableDevelopment = kwargs.contains("enable_development") ? 
            kwargs["enable_development"].cast<bool>() : true;
        bool enableStructuralPlasticity = kwargs.contains("enable_structural_plasticity") ? 
            kwargs["enable_structural_plasticity"].cast<bool>() : true;
        
        // Create components
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->setup(
            width = 20.0f, height = 20.0f, visionWidth = 8, visionHeight = 8,
            maxEnergy = 100.0f, energyDecay = 0.01f
        );
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enableLearning) {
            brain->enableLearning();
        }
        agent->enableRewardModulation(enableRewardMod);
        agent->enableCuriosity(enableCuriosity);
        agent->enableDevelopment(enableDevelopment);
        agent->enableStructuralPlasticity(enableStructuralPlasticity);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("kwargs") = py::kwargs(),
         "Create a quick agent setup with reasonable defaults (returns tuple of brain, world, agent)")

    m.def("createExplorationWorld", [](py::kwargs kwargs) {
        auto world = std::make_shared<SimpleWorld>();
        
        float width = kwargs.contains("width") ? kwargs["width"].cast<float>() : 10.0f;
        float height = kwargs.contains("height") ? kwargs["height"].cast<float>() : 10.0f;
        size_t visionWidth = kwargs.contains("visionWidth") ? kwargs["visionWidth"].cast<size_t>() : 4;
        size_t visionHeight = kwargs.contains("visionHeight") ? kwargs["visionHeight"].cast<size_t>() : 4;
        
        world->setup(width, height, visionWidth, visionHeight);
        
        return world;
    }, py::arg("kwargs") = py::kwargs(),
         "Create a simple exploration world with specified dimensions")

    m.def("createDemoWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->setup(width=15.0f, height=15.0f, visionWidth=6, visionHeight=6);
        world->setAgentStart(7.5f, 7.5f);
        world->setRandomSeed(42);
        return world;
    }, "Create a demo world with agent at center and fixed seed for reproducibility");

    m.def("runDemoSimulation", [](size_t steps) {
        // Create quick agent setup
        auto result = createQuickAgent();
        auto brain = std::get<0>(result);
        auto world = std::get<1>(result);
        auto agent = std::get<2>(result);
        
        // Enable all learning
        brain->enableLearning();
        agent->enableAll(true);
        
        // Run simulation
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(i);
            // Note: AgentBrain doesn't have decodeMotorCommand in current bindings
            // This would need to be added for full simulation
        }
        
        return brain->getStats();
    }, py::arg("steps") = 100,
         "Run a demo simulation with all learning enabled (returns brain stats)")

    m.def("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.def("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.def("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.def("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add exception classes for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
}

} // namespace nlm
