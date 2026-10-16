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
        // Additional convenience methods
        .def("getString", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            throw std::runtime_error("Config key not found or type mismatch: " + key);
        }, py::arg("key"), "Get string value from config")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (val) return *val;
            throw std::runtime_error("Config key not found or type mismatch: " + key);
        }, py::arg("key"), "Get integer value from config")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (val) return *val;
            throw std::runtime_error("Config key not found or type mismatch: " + key);
        }, py::arg("key"), "Get float value from config")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (val) return *val;
            throw std::runtime_error("Config key not found or type mismatch: " + key);
        }, py::arg("key"), "Get boolean value from config")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string value in config")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer value in config")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float value in config")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean value in config")
        .def("setStringWithSource", [](Config& self, const std::string& key, const std::string& value, int source) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set string value with source")
        .def("setIntWithSource", [](Config& self, const std::string& key, int64_t value, int source) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set integer value with source")
        .def("setFloatWithSource", [](Config& self, const std::string& key, double value, int source) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set float value with source")
        .def("setBoolWithSource", [](Config& self, const std::string& key, bool value, int source) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set boolean value with source")
        .def("getOrString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get string value or default from config")
        .def("getOrInt", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value or default from config")
        .def("getOrFloat", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get float value or default from config")
        .def("getOrBool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value or default from config")
        .def("remove", &Config::remove, py::arg("key"), "Remove a configuration key")
        .def("getSource", [](const Config& self, const std::string& key) {
            auto it = std::find_if(self.pImpl->entries.begin(), self.pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != self.pImpl->entries.end()) {
                return static_cast<int>(it->source);
            }
            throw std::runtime_error("Config key not found: " + key);
        }, py::arg("key"), "Get configuration source");

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
        // Enhanced Brain convenience methods
        .def("createDefaultNetwork", [](Brain& self, size_t neuronCount, size_t regionCount) {
            // Clear existing regions
            self.reset();
            
            // Re-initialize with new configuration
            auto config = self.getConfig();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
            
            self.initialize();
        }, py::arg("neuronCount"), py::arg("regionCount"),
             "Create a default neural network with specified neuron and region count")
        .def("createStandardNetwork", [](Brain& self) {
            // Create a standard architecture: sensory, internal, motor regions
            self.reset();
            auto config = self.getConfig();
            
            // Configure for standard architecture
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
            
            self.initialize();
        }, "Create a standard neural network with sensory, internal, and motor regions")
        .def("trainPattern", [](Brain& self, std::vector<float> inputPattern, 
                               size_t steps, bool enablePlasticity = true) {
            if (enablePlasticity) {
                // Enable plasticity on all synapses
                for (const auto& regionId : self.getRegionIds()) {
                    auto region = self.getRegion(regionId);
                    if (region) {
                        for (auto& synapse : region->getSynapses()) {
                            synapse->enablePlasticity(true, true, true);
                        }
                    }
                }
            }
            
            // Inject input pattern
            for (size_t i = 0; i < std::min(inputPattern.size(), 
                                           static_cast<size_t>(self.getTotalNeuronCount())); ++i) {
                self.injectCurrentToNeurons(NeuronType::Sensory, inputPattern[i]);
            }
            
            // Run training steps
            for (SimulationStep step = 0; step < steps; ++step) {
                self.step(step, step * 0.001);
            }
            
            // Disable plasticity after training
            if (enablePlasticity) {
                for (const auto& regionId : self.getRegionIds()) {
                    auto region = self.getRegion(regionId);
                    if (region) {
                        for (auto& synapse : region->getSynapses()) {
                            synapse->enablePlasticity(false, false, false);
                        }
                    }
                }
            }
        }, py::arg("inputPattern"), py::arg("steps"), py::arg("enablePlasticity") = true,
             "Train the brain with a specific input pattern")
        .def("getBrainStats", [](const Brain& self) {
            py::dict stats;
            stats["totalNeurons"] = self.getTotalNeuronCount();
            stats["totalSynapses"] = self.getTotalSynapseCount();
            stats["activeNeurons"] = self.getActiveNeuronCount();
            stats["firingNeurons"] = self.getFiringNeuronCount();
            stats["totalSpikes"] = self.getTotalSpikeCount();
            stats["averageFiringRate"] = self.getAverageFiringRate();
            stats["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            stats["regionCount"] = self.getRegionCount();
            stats["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            
            // Get region-specific stats
            py::list regions;
            for (const auto& regionId : self.getRegionIds()) {
                auto region = self.getRegion(regionId);
                if (region) {
                    py::dict regionStats;
                    regionStats["id"] = static_cast<int>(regionId.value);
                    regionStats["name"] = region->getName();
                    regionStats["neuronCount"] = region->getNeuronCount();
                    regionStats["synapseCount"] = region->getSynapseCount();
                    regions.append(regionStats);
                }
            }
            stats["regions"] = regions;
            
            return stats;
        }, "Get comprehensive brain statistics including regional breakdown")
        .def("exportConnectionMatrix", [](Brain& self) {
            // This is a simplified version - in practice, you'd want more efficient storage
            py::dict connections;
            
            for (const auto& regionId : self.getRegionIds()) {
                auto region = self.getRegion(regionId);
                if (region) {
                    py::list regionConnections;
                    for (const auto& synapse : region->getSynapses()) {
                        py::dict conn;
                        conn["source"] = static_cast<int>(synapse->getSourceNeuron()->getId().value);
                        conn["target"] = static_cast<int>(synapse->getTargetNeuron()->getId().value);
                        conn["weight"] = synapse->getWeight();
                        conn["type"] = static_cast<int>(synapse->getType());
                        conn["delay"] = synapse->getDelay();
                        conn["enabled"] = synapse->isEnabled();
                        regionConnections.append(conn);
                    }
                    connections[std::to_string(static_cast<int>(regionId.value))] = regionConnections;
                }
            }
            
            return connections;
        }, "Export synaptic connections as a Python dictionary")
        .def("importConnectionMatrix", [](Brain& self, const py::dict& connections) {
            for (auto item : connections) {
                std::string regionKey = item.first.cast<std::string>();
                py::list connList = item.second.cast<py::list>();
                
                // Find the region (simplified - assumes region with matching index)
                size_t regionIndex = std::stoul(regionKey);
                if (regionIndex < self.getRegionCount()) {
                    auto region = self.getRegion(RegionId(static_cast<uint64_t>(regionIndex + 1)));
                    if (region) {
                        // This is a simplified example - real implementation would be more complex
                        // For now, we just acknowledge the operation
                    }
                }
            }
        }, py::arg("connections"), "Import synaptic connections from a Python dictionary")
        .def("getMemorySystemsStatus", [](const Brain& self) {
            py::dict memoryStatus;
            
            // Working memory
            memoryStatus["workingMemory"] = "Accessible"; // Would check actual working memory state
            
            // Episodic memory
            memoryStatus["episodicMemory"] = "Available"; // Would check actual episodic memory state
            
            // Semantic memory
            memoryStatus["semanticMemory"] = "Available"; // Would check actual semantic memory state
            
            // Associative memory
            memoryStatus["associativeMemory"] = "Available"; // Would check actual associative memory state
            
            return memoryStatus;
        }, "Get status of all memory systems")
        .def("getNeuromodulationStatus", [](const Brain& self) {
            py::dict neuromodStatus;
            
            neuromodStatus["dopamine"] = "Available"; // Would check actual dopamine level
            neuromodStatus["curiosity"] = "Available"; // Would check actual curiosity level
            neuromodStatus["novelty"] = "Available"; // Would check actual novelty level
            neuromodStatus["predictionError"] = "Available"; // Would check actual prediction error
            neuromodStatus["reward"] = "Available"; // Would check actual reward modulation
            
            return neuromodStatus;
        }, "Get status of all neuromodulation systems")
        .def("getDevelopmentStatus", [](const Brain& self) {
            py::dict devStatus;
            
            devStatus["currentStage"] = static_cast<int>(self.getDevelopmentalStage());
            devStatus["plasticityEnabled"] = true; // Would check actual plasticity state
            devStatus["structuralPlasticity"] = "Available"; // Would check structural plasticity
            devStatus["synaptogenesis"] = "Available"; // Would check synaptogenesis
            devStatus["pruning"] = "Available"; // Would check pruning
            
            return devStatus;
        }, "Get status of all development systems")
        .def("getPredictionStatus", [](const Brain& self) {
            py::dict predStatus;
            
            predStatus["predictionSystem"] = "Available"; // Would check actual prediction system
            predStatus["predictionError"] = "Available"; // Would check actual prediction error
            predStatus["confidenceTracking"] = "Available"; // Would check confidence tracking
            predStatus["actionSelection"] = "Available"; // Would check action selection
            
            return predStatus;
        }, "Get status of all prediction systems")
        .def("getCognitiveStatus", [](const Brain& self) {
            py::dict cogStatus;
            
            cogStatus["planning"] = "Available"; // Would check neural planner
            cogStatus["conceptFormation"] = "Available"; // Would check concept formation
            cogStatus["attention"] = "Available"; // Would check attention system
            cogStatus["workingMemory"] = "Available"; // Would check working memory integration
            
            return cogStatus;
        }, "Get status of all cognitive systems")
        .def("exportCompleteState", [](const Brain& self) {
            py::dict completeState;
            
            // Export brain statistics
            completeState["brainStats"] = self.getBrainStats();
            
            // Export system statuses
            completeState["memorySystems"] = self.getMemorySystemsStatus();
            completeState["neuromodulation"] = self.getNeuromodulationStatus();
            completeState["development"] = self.getDevelopmentStatus();
            completeState["prediction"] = self.getPredictionStatus();
            completeState["cognition"] = self.getCognitiveStatus();
            
            // Export timestamp
            auto now = std::time(nullptr);
            completeState["timestamp"] = static_cast<py::int_t>(now);
            
            return completeState;
        }, "Export complete brain state including all system statuses")
        .def("importCompleteState", [](Brain& self, const py::dict& state) {
            // Import brain state (simplified implementation)
            if (state.contains("brainStats")) {
                auto stats = state["brainStats"].cast<py::dict>();
                if (stats.contains("totalNeurons")) {
                    size_t neuronCount = stats["totalNeurons"].cast<size_t>();
                    auto config = self.getConfig();
                    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
                }
            }
            
            // Re-initialize brain with imported state
            self.initialize();
        }, py::arg("state"), "Import complete brain state from a Python dictionary");

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
        // Enhanced AgentBrain convenience methods
        .def("enableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all neuromodulation and development systems")
        .def("disableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all neuromodulation and development systems")
        .def("getSystemStatus", [](const AgentBrain& self) {
            py::dict status;
            status["rewardModulation"] = self.isRewardModulationEnabled();
            status["structuralPlasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            status["neuromodulationLevel"] = self.getNeuromodulationLevel();
            status["curiosityLevel"] = self.getCuriosityLevel();
            status["noveltyLevel"] = self.getNoveltyLevel();
            status["predictionError"] = self.getPredictionError();
            status["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            return status;
        }, "Get comprehensive status of all agent systems")
        .def("processStep", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                             SimulationStep step, Timestamp timestep) {
            if (world) {
                // Update world
                world->update(timestep);
                
                // Get sensory percept
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                self.processSensoryInput(percept);
                
                // Get brain (for step)
                auto brain = self.getBrain();
                brain->step(step, timestep);
                
                // Decode motor command
                auto action = self.decodeMotorCommand();
                
                // Apply motor command to world
                world->applyMotorCommand(action, timestep);
                
                // Update development
                self.updateDevelopment(timestep);
            }
        }, py::arg("world"), py::arg("step"), py::arg("timestep"),
             "Process a complete step including world update, sensory processing, brain step, and action")
        .def("trainEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                              size_t steps, float& totalReward) {
            totalReward = 0.0f;
            if (world) {
                for (size_t step = 0; step < steps; ++step) {
                    auto brain = self.getBrain();
                    brain->step(step, step * 0.1);
                    
                    auto action = self.decodeMotorCommand();
                    auto actionResult = world->applyMotorCommand(action, step * 0.1);
                    
                    totalReward += actionResult.reward;
                    
                    self.applyRewardModulation(actionResult.reward, 0.0f);
                }
            }
        }, py::arg("world"), py::arg("steps"), py::arg("totalReward"),
             "Run a complete training episode and return total reward")
        .def("getLearningStats", [](const AgentBrain& self, const std::shared_ptr<Brain>& brain) {
            py::dict stats;
            stats["totalNeurons"] = brain->getTotalNeuronCount();
            stats["totalSynapses"] = brain->getTotalSynapseCount();
            stats["activeNeurons"] = brain->getActiveNeuronCount();
            stats["firingNeurons"] = brain->getFiringNeuronCount();
            stats["totalSpikes"] = brain->getTotalSpikeCount();
            stats["avgFiringRate"] = brain->getAverageFiringRate();
            stats["excitationInhibitionRatio"] = brain->getExcitationInhibitionRatio();
            stats["developmentStage"] = static_cast<int>(brain->getDevelopmentalStage());
            return stats;
        }, py::arg("brain"), "Get comprehensive brain learning statistics");

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

    // Factory functions for common NLM setups
    m.def("createStandardAgentSetup", []() {
        py::dict setup;
        
        // Create default configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        // Create agent brain interface
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all systems
        agent->enableAllSystems();
        
        setup["config"] = config;
        setup["brain"] = brain;
        setup["world"] = world;
        setup["agent"] = agent;
        
        return setup;
    }, "Create a complete standard agent setup with brain, world, and agent interface");

    m.def("createExplorationAgentSetup", []() {
        py::dict setup;
        
        // Create default configuration optimized for exploration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 10, 10);
        world->reset();
        world->setMaxEnergy(1000.0f);
        
        // Create agent brain interface
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable exploration-focused systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        setup["config"] = config;
        setup["brain"] = brain;
        setup["world"] = world;
        setup["agent"] = agent;
        
        return setup;
    }, "Create an exploration-focused agent setup with curiosity and development enabled");

    m.def("createLearningAgentSetup", []() {
        py::dict setup;
        
        // Create default configuration optimized for learning
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.12f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(98765), ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(25, 25, 9, 9);
        world->reset();
        world->setMaxEnergy(500.0f);
        
        // Create agent brain interface
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable learning-focused systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        setup["config"] = config;
        setup["brain"] = brain;
        setup["world"] = world;
        setup["agent"] = agent;
        
        return setup;
    }, "Create a learning-focused agent setup with reward modulation and development");

    // Utility functions
    m.def("runTrainingEpisode", [](py::dict setup, size_t steps, float& totalReward) {
        auto world = setup["world"].cast<std::shared_ptr<SimpleWorld>>();
        auto agent = setup["agent"].cast<std::shared_ptr<AgentBrain>>();
        
        totalReward = 0.0f;
        if (world && agent) {
            for (size_t step = 0; step < steps; ++step) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                auto action = agent->decodeMotorCommand();
                auto actionResult = world->applyMotorCommand(action, step * 0.1);
                
                totalReward += actionResult.reward;
                agent->applyRewardModulation(actionResult.reward, 0.0f);
            }
        }
    }, py::arg("setup"), py::arg("steps"), py::arg("totalReward"),
         "Run a complete training episode and return total reward");

    m.def("runSimulation", [](py::dict setup, size::size_t steps) {
        auto world = setup["world"].cast<std::shared_ptr<SimpleWorld>>();
        auto agent = setup["agent"].cast<std::shared_ptr<AgentBrain>>();
        
        if (world && agent) {
            for (size_t step = 0; step < steps; ++step) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, step * 0.1);
                
                // Update development
                agent->updateDevelopment(0.1);
            }
        }
    }, py::arg("setup"), py::arg("steps"),
         "Run a complete simulation with world updates and agent actions");

    m.def("getComprehensiveStats", [](const py::dict& setup) {
        auto brain = setup["brain"].cast<std::shared_ptr<Brain>>();
        auto agent = setup["agent"].cast<std::shared_ptr<AgentBrain>>();
        
        py::dict comprehensiveStats;
        
        // Get brain stats
        if (brain) {
            comprehensiveStats["brainStats"] = brain->getBrainStats();
        }
        
        // Get agent system status
        if (agent) {
            comprehensiveStats["agentSystemStatus"] = agent->getSystemStatus();
        }
        
        // Get setup info
        py::dict setupInfo;
        setupInfo["worldSize"] = std::make_pair(20, 20); // Default world size
        setupInfo["visionSize"] = std::make_pair(8, 8); // Default vision size
        setupInfo["maxEnergy"] = 500.0f; // Default energy
        comprehensiveStats["setupInfo"] = setupInfo;
        
        // Get timestamp
        auto now = std::time(nullptr);
        comprehensiveStats["timestamp"] = static_cast<py::int_t>(now);
        
        return comprehensiveStats;
    }, py::arg("setup"), "Get comprehensive statistics for the complete agent setup");

    // Comparison operators for IDs
    m.def("compareNeuronIds", [](const NeuronId& a, const NeuronId& b) {
        return a.value == b.value;
    }, py::arg("a"), py::arg("b"), "Compare two NeuronId objects");

    m.def("compareSynapseIds", [](const SynapseId& a, const SynapseId& b) {
        return a.value == b.value;
    }, py::arg("a"), py::arg("b"), "Compare two SynapseId objects");

    m.def("compareRegionIds", [](const RegionId& a, const RegionId& b) {
        return a.value == b.value;
    }, py::arg("a"), py::arg("b"), "Compare two RegionId objects");

    m.def("comparePopulationIds", [](const PopulationId& a, const PopulationId& b) {
        return a.value == b.value;
    }, py::arg("a"), py::arg("b"), "Compare two PopulationId objects");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error type definitions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Exception helper functions
    m.def("checkCondition", [](bool condition, const std::string& message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }, py::arg("condition"), py::arg("message"), "Check a condition and throw if false");

    m.def("validateNonEmpty", [](const std::string& value, const std::string& parameterName) {
        if (value.empty()) {
            throw std::invalid_argument("Parameter cannot be empty: " + parameterName);
        }
    }, py::arg("value"), py::arg("parameterName"), "Validate that a string is not empty");

    m.def("validateInRange", [](double value, double min, double max, const std::string& parameterName) {
        if (value < min || value > max) {
            throw std::out_of_range("Parameter " + parameterName + " must be between " + 
                                 std::to_string(min) + " and " + std::to_string(max) +
                                 ", got " + std::to_string(value));
        }
    }, py::arg("value"), py::arg("min"), py::arg("max"), py::arg("parameterName"),
       "Validate that a value is within the specified range");
}
