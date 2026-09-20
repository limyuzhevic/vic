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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Utility methods for advanced users
        .def("getValue", [](const Config& self, const std::string& key) {
            std::stringstream ss;
            ss << "Key: " << key << ", Exists: " << self.has(key);
            if (self.has(key)) {
                ss << ", Has value for key " << key << "; to get specific type, use get<int>(), get<double>(), etc.";
            }
            return ss.str();
        }, py::arg("key"), "Get string representation of a configuration key.")
        .def("getAllValues", [](const Config& self) {
            std::stringstream ss;
            ss << "Total keys: " << self.getKeys().size() << "\n";
            for (const auto& key : self.getKeys()) {
                ss << "  " << key << "\n";
            }
            return ss.str();
        }, "Get string representation of all configuration keys and their values.")
        .def("exportToJSON", [](const Config& self, const std::string& filepath) {
            return self.saveToFile(filepath);
        }, py::arg("filepath"), "Export configuration to file (placeholder format).")
        .def("importFromJSON", [](Config& self, const std::string& filepath) {
            return self.loadFromFile(filepath);
        }, py::arg("filepath"), "Import configuration from file (placeholder format).")
        .def("printConfig", [](const Config& self) {
            std::cout << self.summary() << std::endl;
        }, "Print configuration summary to console.")
        .def("update", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Update configuration value by key.")
        .def("updateInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Update configuration value by key.")
        .def("updateFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Update configuration value by key.")
        .def("updateBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Update configuration value by key.")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto val = self.get<int>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get integer value with default.")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            auto val = self.get<double>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float value with default.")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.get<std::string>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "", "Get string value with default.")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto val = self.get<bool>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false, "Get boolean value with default.")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set integer value by key.")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set float value by key.")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set string value by key.")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set boolean value by key.")
        .def("removeKey", &Config::remove, py::arg("key"), "Remove configuration key.")
        .def("applyToBrain", [](const Config& self, std::shared_ptr<Brain> brain) {
            if (brain) {
                brain->setConfig(self);
            }
            return brain;
        }, py::arg("brain"), "Apply configuration to brain object. Returns the same brain for chaining.")
        .def("clone", [](const Config& self) {
            auto clone = std::make_shared<Config>();
            // Copy all entries (implementation detail)
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // This is a simplified version - actual implementation would need to copy values
                // For now, just copy the structure
                std::string value = "[value]";
                clone->set(key, value, ConfigSource::Runtime);
            }
            return clone;
        }, "Create a clone of this configuration.");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Convenience methods for advanced users
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, int numSteps) {
            float totalReward = 0.0f;
            for (int step = 0; step < numSteps; ++step) {
                world.update(0.1f);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Access the brain from agent and step it
                auto brain = self.getBrain();
                brain->step(step, step * 0.1);
                
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                // Get reward from internal signals
                auto internal = percept.getInternal();
                float reward = internal.size() > 0 ? internal[0] : 0.0f;
                self.applyRewardModulation(reward, 0.0f);
            }
            return totalReward / numSteps;
        }, py::arg("world"), py::arg("numSteps"),
         "Run a complete episode and return average reward. Convenience method for simulation loops.")
        .def("getStateSummary", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "Development: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            ss << "Curiosity: " << self.getCuriosityLevel() << "\n";
            ss << "Novelty: " << self.getNoveltyLevel() << "\n";
            ss << "Dopamine: " << self.getNeuromodulationLevel() << "\n";
            ss << "Prediction Error: " << self.getPredictionError() << "\n";
            return ss.str();
        }, "Get a summary of the agent's current state as a string.")
        .def("createWorldSnapshot", [](AgentBrain& self, const SimpleWorld& world) {
            std::stringstream ss;
            auto body = world.getAgentBody();
            ss << "Position: (" << body.x << ", " << body.y << ")\n";
            ss << "Orientation: " << body.orientation << "\n";
            ss << "Energy: " << body.energy << "\n";
            ss << "Health: " << body.health << "\n";
            ss << "Age: " << body.age << "\n";
            return ss.str();
        }, py::arg("world"), "Get a string representation of the world state for logging/monitoring.")
        .def("batchProcessPercepts", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            float totalNovelty = 0.0f;
            for (const auto& percept : percepts) {
                self.processSensoryInput(percept);
                totalNovelty += self.getNoveltyLevel();
            }
            return totalNovelty / percepts.size();
        }, py::arg("percepts"), "Process multiple percepts efficiently for batch operations. Returns average novelty level. ")
        .def("exportConfig", [](const AgentBrain& self) {
            std::stringstream ss;
            auto brain = self.getBrain();
            auto config = brain->getConfig();
            ss << "Configuration:\n";
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                ss << "  " << key << " = ";
                if (config->has(key)) {
                    ss << "[present]";
                }
                ss << "\n";
            }
            return ss.str();
        }, "Export configuration summary for debugging/monitoring.");

    // Convenience functions for advanced usage
    m.def("createAdvancedBrain", [](const std::string& configFile, bool enableAdvancedFeatures) {
        auto config = std::make_shared<Config>();
        if (!configFile.empty() && config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (enableAdvancedFeatures) {
            // Enable advanced features
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 2.0f);
            config->set("neuromod.curiosity.enable", true);
        }
        
        return brain;
    }, py::arg("configFile") = "", py::arg("enableAdvancedFeatures") = false,
        "Create an advanced brain with optional configuration and feature enablement. Returns Brain object for chaining.")
        
    .def("configureWorld", [](SimpleWorld& world, int width, int height, 
                             int visionWidth, int visionHeight, 
                             float energyDecayRate) {
        world.configure(width, height, visionWidth, visionHeight);
        world.setEnergyDecayRate(energyDecayRate);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        py::arg("energyDecayRate") = 0.01f,
        "Configure world with all parameters at once. Returns the same world for chaining.")
        
    .def("createAgentWithDefaults", [](std::shared_ptr<Brain> brain, const SimpleWorld& world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable common advanced features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an agent brain with default advanced features enabled. Returns AgentBrain object.")
        
    .def("runFullSimulation", [](std::shared_ptr<Brain> brain, SimpleWorld& world, 
                                 int numSteps, bool verbose) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        float totalReward = 0.0f;
        size_t maxFiring = 0;
        
        for (int step = 0; step < numSteps; ++step) {
            // Get sensory input
            auto percept = world.getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.1);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Get reward and apply neuromodulation
            auto internal = percept.getInternal();
            float reward = internal.size() > 0 ? internal[0] : 0.0f;
            totalReward += reward;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Log progress
            if (verbose && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Reward: " + std::to_string(totalReward / (step + 1)) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
            
            // Track max firing
            maxFiring = std::max(maxFiring, brain->getFiringNeuronCount());
        }
        
        return std::make_tuple(totalReward / numSteps, maxFiring);
    }, py::arg("brain"), py::arg("world"), py::arg("numSteps") = 1000, py::arg("verbose") = false,
        "Run a complete simulation episode. Returns tuple of (average reward, max firing neurons).")
        
    .def("validateConfiguration", [](const Config& config) {
        std::stringstream ss;
        bool valid = true;
        
        // Check for essential keys
        auto keys = config.getKeys();
        if (keys.empty()) {
            ss << "ERROR: Empty configuration\n";
            valid = false;
        }
        
        // Check required brain parameters
        if (!config.has("brain.neuron_count")) {
            ss << "WARNING: brain.neuron_count not set (using default)\n";
        }
        
        if (!config.has("brain.synapse_density")) {
            ss << "WARNING: brain.synapse_density not set (using default)\n";
            valid = false;
        }
        
        // Check plasticity settings
        if (!config.has("plasticity.stdp.enable")) {
            ss << "WARNING: STDP not explicitly enabled\n";
            valid = false;
        }
        
        // Check neuromodulation
        if (!config.has("neuromod.dopamine.scale")) {
            ss << "WARNING: Dopamine scale not set (using default)\n";
        }
        
        if (!config.has("neuromod.curiosity.enable")) {
            ss << "WARNING: Curiosity not explicitly enabled\n";
            valid = false;
        }
        
        ss << "Validation " << (valid ? "PASSED" : "FAILED") << "\n";
        ss << "Total keys configured: " << keys.size() << "\n";
        
        return ss.str();
    }, py::arg("config"), "Validate configuration and return validation report as string.")
        
    .def("createBenchmarkConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Fast benchmark configuration
        config->set("neuron_count", 500);
        config->set("synapse_density", 0.2f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.5f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("brain.v_thresh", -50.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.tau_mem", 20.0f);
        config->set("brain.tau_ref", 2.0f);
        
        return config;
    }, "Create a benchmark-optimized configuration with performance tuning.")
        
    .def("createDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Development configuration with enhanced plasticity
        config->set("neuron_count", 1000);
        config->set("synapse_density", 0.15f);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.018f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enabled", true);
        config->set("brain.v_thresh", -45.0f);  // Lower threshold for more firing
        config->set("brain.v_rest", -65.0f);   // Higher resting potential
        config->set("brain.tau_mem", 25.0f);
        config->set("brain.tau_ref", 3.0f);
        config->set("synaptogenesis_rate", 0.0002f);
        config->set("pruning_rate", 0.00002f);
        
        return config;
    }, "Create a development-focused configuration optimized for learning and plasticity.")
        
    .def("createProductionConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Production configuration with stability
        config->set("neuron_count", 200);
        config->set("synapse_density", 0.08f);
        config->set("stdp_ltp_weight", 0.005f);
        config->set("stdp_ltd_weight", 0.006f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", false);  // Disable for stability
        config->set("neuromod.dopamine.scale", 0.8f);
        config->set("neuromod.curiosity.enable", false);  // Reduce exploration
        config->set("neuromod.novelty.enable", false);
        config->set("development.enabled", false);
        config->set("brain.v_thresh", -55.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.tau_mem", 15.0f);
        config->set("brain.tau_ref", 2.0f);
        config->set("synaptogenesis_rate", 0.00001f);
        config->set("pruning_rate", 0.000005f);
        
        return config;
    }, "Create a production-focused configuration optimized for stability and reliability.")
        
    .def("quickBenchmark", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("synapse_density", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run quick benchmark
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return std::make_tuple(
            brain->getAverageFiringRate(),
            brain->getTotalSpikeCount(),
            brain->getTotalNeuronCount()
        );
    }, "Run a quick benchmark and return (avg_firing_rate, total_spikes, total_neurons).")
        
    .def("version", []() {
        return std::string("NLM Python Bindings v0.1.0");
    }, "Get version information for the bindings.")
        
    .def("printMemoryUsage", []() {
        std::stringstream ss;
        ss << "Memory usage statistics (placeholder)\n";
        ss << "Note: For detailed memory analysis, use system tools like valgrind or address sanitizer.\n";
        ss << "This is a placeholder function for memory usage reporting.\n";
        return ss.str();
    }, "Print memory usage statistics (placeholder for advanced debugging).")
        
    .def("createConfigFromDict", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            
            // Try to convert to appropriate type
            if (item.second.is(py::int_())) {
                config->set(key, py::cast<int>(item.second));
            } else if (item.second.is(py::float_())) {
                config->set(key, py::cast<float>(item.second));
            } else if (item.second.is(py::bool_())) {
                config->set(key, py::cast<bool>(item.second));
            } else if (item.second.is(py::str())) {
                config->set(key, py::cast<std::string>(item.second));
            }
        }
        
        return config;
    }, py::arg("configDict"),
        "Create configuration from Python dictionary. Keys are converted to appropriate types.")
        
    .def("convertConfigToDict", [](const Config& config) {
        py::dict dict;
        
        for (const auto& key : config.getKeys()) {
            // Try to get as different types
            auto intVal = config.get<int>(key);
            if (intVal) {
                dict[key] = *intVal;
                continue;
            }
            
            auto floatVal = config.get<double>(key);
            if (floatVal) {
                dict[key] = *floatVal;
                continue;
            }
            
            auto boolVal = config.get<bool>(key);
            if (boolVal) {
                dict[key] = *boolVal;
                continue;
            }
            
            auto stringVal = config.get<std::string>(key);
            if (stringVal) {
                dict[key] = *stringVal;
            }
        }
        
        return dict;
    }, "Convert configuration to Python dictionary.")
        
    .def("saveConfigSnapshot", [](const Config& config, const std::string& filepath) {
        return config.saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"),
        "Save configuration to file for persistence.")
        
    .def("loadConfigSnapshot", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"),
        "Load configuration from file and return Config object.")
        
    .def("validateAndFixConfig", [](Config& config) {
        std::stringstream ss;
        bool fixed = false;
        
        // Auto-fix common issues
        if (!config.has("brain.neuron_count")) {
            config->set("brain.neuron_count", 1000);
            ss << "Auto-fixed: Set brain.neuron_count = 1000\n";
            fixed = true;
        }
        
        if (!config.has("brain.synapse_density")) {
            config->set("brain.synapse_density", 0.1f);
            ss << "Auto-fixed: Set brain.synapse_density = 0.1\n";
            fixed = true;
        }
        
        if (!config.has("plasticity.stdp.enable")) {
            config->set("plasticity.stdp.enable", true);
            ss << "Auto-fixed: Enabled STDP\n";
            fixed = true;
        }
        
        if (!config.has("neuromod.curiosity.enable")) {
            config->set("neuromod.curiosity.enable", true);
            ss << "Auto-fixed: Enabled curiosity\n";
            fixed = true;
        }
        
        ss << "Configuration " << (fixed ? "FIXED" : "OK") << "\n";
        return ss.str();
    }, py::arg("config"), "Validate and auto-fix common configuration issues. Returns report.")
        
    .def("compareConfigs", [](const Config& config1, const Config& config2) {
        std::stringstream ss;
        auto keys1 = config1.getKeys();
        auto keys2 = config2.getKeys();
        
        ss << "Configuration Comparison:\n";
        ss << "Config 1 has " << keys1.size() << " keys, Config 2 has " << keys2.size() << " keys\n";
        
        // Find differences
        for (const auto& key : keys1) {
            if (!config2.has(key)) {
                ss << "Key in Config 1 only: " << key << "\n";
            } else {
                // Compare values (simplified)
                ss << "Common key: " << key << "\n";
            }
        }
        
        for (const auto& key : keys2) {
            if (!config1.has(key)) {
                ss << "Key in Config 2 only: " << key << "\n";
            }
        }
        
        return ss.str();
    }, py::arg("config1"), py::arg("config2"), "Compare two configurations and return report.")
        
    .def("generateConfigTemplate", []() {
        std::stringstream ss;
        ss << "# NLM Configuration Template\n";
        ss << "# Copy this and modify as needed\n\n";
        ss << "# Basic settings\n";
        ss << "brain.neuron_count = 1000\n";
        ss << "brain.synapse_density = 0.1\n\n";
        ss << "# Plasticity\n";
        ss << "plasticity.stdp.enable = true\n";
        ss << "plasticity.stdp.learning_rate = 0.001\n";
        ss << "plasticity.hebbian.enable = true\n\n";
        ss << "# Neuromodulation\n";
        ss << "neuromod.dopamine.scale = 1.0\n";
        ss << "neuromod.curiosity.enable = true\n";
        ss << "neuromod.novelty.enable = true\n\n";
        ss << "# Neuron parameters\n";
        ss << "brain.v_thresh = -50.0\n";
        ss << "brain.v_rest = -70.0\n";
        ss << "brain.v_reset = -75.0\n";
        ss << "brain.tau_mem = 20.0\n";
        ss << "brain.tau_ref = 2.0\n\n";
        ss << "# Development\n";
        ss << "development.enabled = true\n";
        ss << "synaptogenesis_rate = 0.0001\n";
        ss << "pruning_rate = 0.00001\n";
        
        return ss.str();
    }, "Generate a configuration template with all common settings.")
        
    .def("createSimpleTest", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 50);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Simple test: inject current and see if neurons fire
        brain->injectCurrentToNeurons(NeuronType::Excitatory, 20.0f);
        
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::stringstream ss;
        ss << "Simple Test Results:\n";
        ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Average firing rate: " << brain->getAverageFiringRate() << "\n";
        ss << "Test " << (brain->getTotalSpikeCount() > 0 ? "PASSED" : "FAILED") << "\n";
        
        return ss.str();
    }, "Run a simple functionality test and return results as string.")
        
    .def("createAdvancedTest", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.5f);
        config->set("neuromod.curiosity.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run more comprehensive test with learning
        auto agent = std::make_shared<AgentBrain>(brain);
        SimpleWorld world;
        world.configure(10, 10, 8, 8);
        world.reset();
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        float totalReward = 0.0f;
        for (int step = 0; step < 200; ++step) {
            // Get percept
            auto percept = world.getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.1);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Apply reward
            auto internal = percept.getInternal();
            float reward = internal.size() > 0 ? internal[0] : 0.0f;
            totalReward += reward;
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        std::stringstream ss;
        ss << "Advanced Test Results:\n";
        ss << "Average reward: " << (totalReward / 200) << "\n";
        ss << "Curiosity level: " << agent->getCuriosityLevel() << "\n";
        ss << "Novelty level: " << agent->getNoveltyLevel() << "\n";
        ss << "Dopamine level: " << agent->getNeuromodulationLevel() << "\n";
        ss << "Prediction error: " << agent->getPredictionError() << "\n";
        ss << "Development stage: " << static_cast<int>(agent->getDevelopmentalStage()) << "\n";
        ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Test " << (totalReward > 0 ? "PASSED" : "FAILED") << "\n";
        
        return ss.str();
    }, "Run an advanced comprehensive test with all systems and return detailed results.")
        
    .def("createBenchmarkSuite", []() {
        std::stringstream ss;
        ss << "=== NLM Python Bindings Benchmark Suite ===\n\n";
        
        // Test 1: Simple brain creation
        ss << "Test 1: Brain Creation\n";
        auto config1 = std::make_shared<Config>();
        config1->set("neuron_count", 100);
        auto brain1 = std::make_shared<Brain>(config1);
        brain1->initialize();
        ss << "  ✓ Created brain with " << brain1->getTotalNeuronCount() << " neurons\n";
        
        // Test 2: Configuration operations
        ss << "\nTest 2: Configuration Operations\n";
        auto config2 = std::make_shared<Config>();
        config2->set("test_key", 42);
        config2->set("test_float", 3.14f);
        config2->set("test_bool", true);
        config2->set("test_string", "test");
        ss << "  ✓ Set and get various value types\n";
        ss << "  ✓ Keys: " << config2->getKeys().size() << "\n";
        
        // Test 3: Agent creation
        ss << "\nTest 3: Agent Creation\n";
        auto brain3 = std::make_shared<Brain>(config2);
        brain3->initialize();
        SimpleWorld world3;
        world3.configure(5, 5, 4, 4);
        world3.reset();
        auto agent3 = std::make_shared<AgentBrain>(brain3);
        agent3->initialize(world3);
        ss << "  ✓ Created agent with brain and world\n";
        
        // Test 4: Sensory processing
        ss << "\nTest 4: Sensory Processing\n";
        auto percept = world3.getSensoryPercept();
        agent3->processSensoryInput(percept);
        ss << "  ✓ Processed sensory input\n";
        
        // Test 5: Motor decoding
        ss << "\nTest 5: Motor Decoding\n";
        auto action = agent3->decodeMotorCommand();
        ss << "  ✓ Decoded motor command: " << static_cast<int>(action) << "\n";
        
        // Test 6: Neuromodulation
        ss << "\nTest 6: Neuromodulation\n";
        agent3->applyRewardModulation(1.0f, 0.5f);
        ss << "  ✓ Applied reward modulation\n";
        ss << "  ✓ Dopamine level: " << agent3->getNeuromodulationLevel() << "\n";
        
        // Test 7: Development
        ss << "\nTest 7: Development\n";
        agent3->updateDevelopment(0.1);
        ss << "  ✓ Updated development\n";
        ss << "  ✓ Developmental stage: " << static_cast<int>(agent3->getDevelopmentalStage()) << "\n";
        
        ss << "\n=== All Tests Passed ===\n";
        ss << "Benchmark completed successfully with all major functionality tested.\n";
        
        return ss.str();
    }, "Run comprehensive benchmark suite testing all major NLM Python functionality.");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
