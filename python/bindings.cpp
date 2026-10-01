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
        // Additional convenience methods for configuration management
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            auto value = self.getOr<float>(key, defaultValue);
            return value;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a float configuration value with default")
        .def("getInt", [](const Config& self, const std::string& key, int64_t defaultValue) {
            auto value = self.getOr<int64_t>(key, defaultValue);
            return value;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get an integer configuration value with default")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.getOr<bool>(key, defaultValue);
            return value;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a boolean configuration value with default")
        .def("mergeWith", [](Config& self, const Config& other) {
            // Merge other config into this one
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                if (other.has(key)) {
                    // Get the value and source from other
                    if (other.getType(key) == nlm::ConfigType::Float) {
                        float value = other.getOr<float>(key, 0.0f);
                        self.set(key, value, nlm::ConfigSource::User);
                    } else if (other.getType(key) == nlm::ConfigType::Int) {
                        int64_t value = other.getOr<int64_t>(key, 0);
                        self.set(key, value, nlm::ConfigSource::User);
                    } else if (other.getType(key) == nlm::ConfigType::Bool) {
                        bool value = other.getOr<bool>(key, false);
                        self.set(key, value, nlm::ConfigSource::User);
                    }
                }
            }
        }, py::arg("other"),
             "Merge another configuration into this one")
        .def("clone", []() {
            auto config = std::make_shared<Config>();
            return config;
        }, "Create a copy of this configuration");

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
        // Additional convenience methods for batch processing and monitoring
        .def("runSteps", [](AgentBrain& self, const std::vector<std::shared_ptr<SensoryPercept>>& percepts, double timestep) {
            std::vector<SensoryPercept> perceptVec;
            for (const auto& percept : percepts) {
                perceptVec.push_back(*percept);
            }
            for (size_t i = 0; i < percepts.size(); ++i) {
                self.processSensoryInput(perceptVec[i]);
                auto* brain = self.getBrain();
                brain->step(static_cast<nlm::SimulationStep>(i), i * timestep);
            }
        }, py::arg("percepts"), py::arg("timestep"),
             "Process multiple sensory percepts and run corresponding brain steps")
        .def("runStepsWithActions", [](AgentBrain& self, const std::vector<std::shared_ptr<SensoryPercept>>& percepts, 
                                      double timestep, std::function<MotorCommand()> actionProvider) {
            std::vector<SensoryPercept> perceptVec;
            std::vector<MotorCommand> actions;
            for (size_t i = 0; i < percepts.size(); ++i) {
                perceptVec.push_back(*percepts[i]);
                self.processSensoryInput(perceptVec[i]);
                auto* brain = self.getBrain();
                brain->step(static_cast<nlm::SimulationStep>(i), i * timestep);
                actions.push_back(actionProvider());
            }
        }, py::arg("percepts"), py::arg("timestep"), py::arg("actionProvider"),
             "Run steps with actions provided by callback function")
        .def("getBrainStats", [](AgentBrain& self) {
            auto* brain = self.getBrain();
            return std::make_tuple(
                brain->getTotalNeuronCount(),
                brain->getFiringNeuronCount(),
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                self.getDevelopmentalStage()
            );
        }, "Get comprehensive brain statistics")
        .def("exportConfig", [](AgentBrain& self, const std::string& filepath) {
            auto* brain = self.getBrain();
            auto config = brain->getConfig();
            return config->saveToFile(filepath);
        }, py::arg("filepath"), "Export brain configuration to file")
        .def("importConfig", [](AgentBrain& self, const std::string& filepath) {
            auto* brain = self.getBrain();
            auto config = brain->getConfig();
            bool success = config->loadFromFile(filepath);
            if (success) {
                // Note: Would need brain->setConfig() method for full import
                // This is a limitation of current API
                return true;
            }
            return false;
        }, py::arg("filepath"), "Import configuration from file (note: may not fully restore brain state)")
        .def("monitorProgress", [](AgentBrain& self, std::function<void(int, float)> progressCallback, 
                                  double timestep, size_t totalSteps) {
            auto* brain = self.getBrain();
            for (size_t step = 0; step < totalSteps; ++step) {
                // Simulate sensory input for monitoring
                Vision vision(8, 8, 3);
                std::vector<float> data(8*8*3, 0.1f);
                vision.setData(data);
                
                SensoryPercept percept;
                percept.setVision(vision);
                self.processSensoryInput(percept);
                brain->step(static_cast<nlm::SimulationStep>(step), step * timestep);
                
                float progress = static_cast<float>(step) / totalSteps;
                float firingRate = brain->getAverageFiringRate();
                progressCallback(static_cast<int>(step), firingRate);
            }
        }, py::arg("progressCallback"), py::arg("timestep"), py::arg("totalSteps"),
             "Monitor simulation progress with callback function (step, firing_rate)");

    // Predefined configuration presets
    // Advanced error handling and validation
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (!config) {
            errors.push_back("Configuration is null");
            return std::make_pair(errors, warnings);
        }
        
        // Check required fields
        std::vector<std::string> requiredFields = {"random_seed", "simulation_timestep", "neuron_count"};
        for (const auto& field : requiredFields) {
            if (!config->has(field)) {
                warnings.push_back("Recommended field \"" + field + "\" not set");
            }
        }
        
        // Validate numeric ranges
        if (config->has("neuron_count")) {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                errors.push_back("neuron_count must be positive");
            } else if (neuronCount > 10000) {
                warnings.push_back("neuron_count is large (>10000) - simulation may be slow");
            }
        }
        
        if (config->has("region_count")) {
            int64_t regionCount = config->getOr<int64_t>("region_count", 0);
            if (regionCount <= 0) {
                errors.push_back("region_count must be positive");
            }
        }
        
        if (config->has("simulation_timestep")) {
            double timestep = config->getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0) {
                errors.push_back("simulation_timestep must be positive");
            } else if (timestep > 1.0) {
                warnings.push_back("simulation_timestep is large (>1.0) - may affect stability");
            }
        }
        
        return std::make_pair(errors, warnings);
    }, py::arg("config"), "Validate configuration and return list of errors and warnings");
    
    m.def("validateBrain", [](const std::shared_ptr<Brain>& brain) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (!brain) {
            errors.push_back("Brain is null");
            return std::make_pair(errors, warnings);
        }
        
        if (!brain->getConfig()) {
            errors.push_back("Brain has no configuration");
        }
        
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t activeCount = brain->getActiveNeuronCount();
        float firingRate = brain->getAverageFiringRate();
        
        if (neuronCount == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (activeCount > neuronCount * 0.9) {
            warnings.push_back("High proportion of active neurons (>90%)");
        }
        
        if (firingRate > 100.0f) {
            warnings.push_back("High firing rate (>100 Hz)");
        }
        
        return std::make_pair(errors, warnings);
    }, py::arg("brain"), "Validate brain state and return list of errors and warnings");
    
    m.def("safeStep", [](std::shared_ptr<Brain>& brain, nlm::SimulationStep step, nlm::Timestamp time = 0.0) {
        try {
            if (!brain) {
                throw std::runtime_error("Brain is null");
            }
            brain->step(step, time);
            return true;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error during step: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
         "Safely perform a brain step with error handling");
    
    m.def("safeInitialize", [](std::shared_ptr<Brain>& brain) {
        try {
            if (!brain) {
                throw std::runtime_error("Brain is null");
            }
            return brain->initialize();
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error during initialization: ") + e.what());
        }
    }, py::arg("brain"), "Safely initialize brain with error handling");
    
    m.def("checkForNaN", [](const std::shared_ptr<Brain>& brain) {
        std::vector<std::string> issues;
        
        if (!brain) {
            issues.push_back("Brain is null");
            return issues;
        }
        
        // Check neuron states for NaN values
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (const auto* neuron : neurons) {
                if (std::isnan(neuron->getMembranePotential())) {
                    issues.push_back("Found NaN membrane potential");
                    return issues;
                }
                if (std::isnan(neuron->getThreshold())) {
                    issues.push_back("Found NaN threshold");
                    return issues;
                }
            }
        }
        
        return issues;
    }, py::arg("brain"), "Check brain state for NaN values and return list of issues");
    
    m.def("compareBrains", [](const std::shared_ptr<Brain>& brain1, const std::shared_ptr<Brain>& brain2) {
        std::map<std::string, std::pair<float, float>> differences;
        
        if (!brain1 || !brain2) {
            differences["brain1_valid"] = std::make_pair(brain1 ? 1.0f : 0.0f, brain2 ? 1.0f : 0.0f);
            return differences;
        }
        
        // Compare neuron counts
        float neuronCount1 = static_cast<float>(brain1->getTotalNeuronCount());
        float neuronCount2 = static_cast<float>(brain2->getTotalNeuronCount());
        differences["neuron_count_diff"] = std::make_pair(neuronCount1, neuronCount2);
        
        // Compare spike counts
        float spikeCount1 = static_cast<float>(brain1->getTotalSpikeCount());
        float spikeCount2 = static_cast<float>(brain2->getTotalSpikeCount());
        differences["spike_count_diff"] = std::make_pair(spikeCount1, spikeCount2);
        
        // Compare firing rates
        float rate1 = brain1->getAverageFiringRate();
        float rate2 = brain2->getAverageFiringRate();
        differences["firing_rate_diff"] = std::make_pair(rate1, rate2);
        
    m.def("formatBrainStats", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            return std::string("Brain is null");
        }
        
        std::stringstream ss;
        ss << "=== Brain Statistics ===\n";
        ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Active: " << brain->getActiveNeuronCount() << "\n";
        ss << "Firing: " << brain->getFiringNeuronCount() << "\n";
        ss << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "Regions: " << brain->getRegionCount() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        
        return ss.str();
    }, py::arg("brain"), "Format brain statistics as a readable string");
    
    m.def("createProgressTracker", [](size_t totalSteps, std::function<void(float)> callback) {
        return std::make_shared<ProgressTracker>(totalSteps, callback);
    }, py::arg("totalSteps"), py::arg("callback"), "Create a progress tracker object");
    
    py::class_<ProgressTracker>(m, "ProgressTracker", R"pbdoc(Progress tracking utility)pbdoc")
        .def(py::init<size_t, std::function<void(float)>>(), py::arg("totalSteps"), py::arg("callback"))
        .def("update", &ProgressTracker::update, py::arg("current"),
             "Update progress with current step")
        .def("setMessage", &ProgressTracker::setMessage, py::arg("message"),
             "Set a custom message for the progress tracker")
        .def("getProgress", &ProgressTracker::getProgress,
             "Get current progress (0.0 to 1.0)")
        .def("getMessage", &ProgressTracker::getMessage,
             "Get current message");
    
    m.def("runWithProgress", [](std::shared_ptr<Brain>& brain, const std::vector<std::shared_ptr<SensoryPercept>>& percepts, 
                              const std::shared_ptr<ProgressTracker>& tracker,
                              double timestep = 0.001) {
        if (!brain || !tracker) {
            throw std::runtime_error("Invalid brain or progress tracker");
        }
        
        for (size_t i = 0; i < percepts.size(); ++i) {
            // Process sensory input
            if (percepts[i]) {
                brain->receiveSensoryInput(*percepts[i]);
            }
            
            // Run brain step
            brain->step(static_cast<nlm::SimulationStep>(i), i * timestep);
            
            // Update progress
            tracker->update(i + 1);
            
            // Small delay to prevent overwhelming the system
            if (i % 100 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }, py::arg("brain"), py::arg("percepts"), py::arg("tracker"), py::arg("timestep") = 0.001,
         "Run brain simulation with progress tracking");
    
    m.def("runBenchmark", [](const std::shared_ptr<Config>& config, size_t steps = 1000) {
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure world for benchmark
        world->configure(20, 20, 8, 8);
        agent->initialize(*world);
        
        // Create sensory input for benchmark
        Vision vision(8, 8, 3);
        std::vector<float> data(8*8*3, 0.5f);
        vision.setData(data);
        
        SensoryPercept percept;
        percept.setVision(vision);
        
        // Warm up
        agent->processSensoryInput(percept);
        brain->initialize();
        brain->step(0, 0.0);
        
        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 1; i <= steps; ++i) {
            agent->processSensoryInput(percept);
            brain->step(static_cast<nlm::SimulationStep>(i), i * 0.001);
            
            // Report progress every 10%
            if (i % (steps / 10) == 0) {
                auto current = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count();
                double stepsPerSec = (i * 1000.0) / duration;
                std::cout << "Progress: " << (i * 100.0 / steps) << "% - " 
                          << "Speed: " << stepsPerSec << " steps/sec" << std::endl;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        double stepsPerSec = (steps * 1000.0) / duration;
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            duration,
            stepsPerSec
        );
    }, py::arg("config"), py::arg("steps") = 1000,
         "Run a benchmark simulation and return performance metrics");

} // namespace nlm
