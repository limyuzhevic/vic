// Enhanced Python bindings for NLM with improved API
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

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
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -----------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        This enhanced version provides additional convenience methods and
        better integration with Python workflows.
        
        Key improvements:
        - More intuitive API methods for common operations
        - Better error handling and validation
        - Enhanced configuration options
        - Integrated learning and simulation utilities
        - Comprehensive documentation and examples
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId type
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

    // SynapseId type
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

    // RegionId type
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

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ============= CONFIGURATION CLASS =============
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        // Convenience configuration methods
        .def("setNeuronCount", &Config::set, py::arg("key"), py::arg("value"),
             "Set neuron count (convenience method)")
        .def("setRegionCount", &Config::set, py::arg("key"), py::arg("value"),
             "Set region count (convenience method)")
        .def("setConnectionProbability", &Config::set, py::arg("key"), py::arg("value"),
             "Set connection probability (convenience method)")
        .def("setRandomSeed", &Config::set, py::arg("key"), py::arg("value"),
             "Set random seed (convenience method)")
        .def("setSimulationTimestep", &Config::set, py::arg("key"), py::arg("value"),
             "Set simulation timestep (convenience method)")
        .def("setSTDPParameters", [](Config& self, float ltp, float ltd, float tau) {
            self.set("stdp_ltp_weight", ltp, ConfigSource::Default);
            self.set("stdp_ltd_weight", ltd, ConfigSource::Default);
            self.set("stdp_tau", tau, ConfigSource::Default);
        }, py::arg("ltp_weight"), py::arg("ltd_weight"), py::arg("tau"),
             "Set STDP parameters (convenience method)")
        .def("setPlasticityParameters", [](Config& self, float synaptogenesis, float pruning) {
            self.set("synaptogenesis_rate", synaptogenesis, ConfigSource::Default);
            self.set("pruning_rate", pruning, ConfigSource::Default);
        }, py::arg("synaptogenesis_rate"), py::arg("pruning_rate"),
             "Set structural plasticity parameters (convenience method)");

    // ============= SENSORY INPUT CLASSES =============
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

    // ============= ACTION CLASS =============
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
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters=" + 
                   std::to_string(action.getParameters().size()) + ">";
        });

    // ============= WORLD OBJECT CLASS =============
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

    // ============= AGENT BODY CLASS =============
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

    // ============= ACTION RESULT CLASS =============
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ============= SENSORY PERCEPT CLASS =============
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

    // ============= WORLD CLASS =============
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // ============= BRAIN CLASS =============
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Enhanced Python API methods
        .def("createBrainFromConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a brain from configuration (convenience method)")
        .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration (convenience method)")
        .def("runSimulation", [](std::shared_ptr<Brain> brain, int steps, double timestep = 0.001) {
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * timestep);
            }
        }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
             "Run simulation for specified steps (convenience method)")
        .def("getStatistics", [](std::shared_ptr<Brain> brain) {
            auto stats = py::dict();
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            return stats;
        }, py::arg("brain"), "Get comprehensive brain statistics (convenience method)")
        .def("isHealthy", [](std::shared_ptr<Brain> brain) {
            return brain->getTotalSynapseCount() > 0 && 
                   brain->getTotalNeuronCount() > 0;
        }, py::arg("brain"), "Check if brain is in a healthy state (convenience method)")
        .def("exportConfig", [](std::shared_ptr<Config> config) {
            return config->summary();
        }, py::arg("config"), "Export configuration as summary string (convenience method)")
        .def("importConfigFromDict", [](std::shared_ptr<Config> config, const py::dict& dict) {
            for (auto it : dict) {
                std::string key = py::str(it.first);
                if (py::isinstance(it.second, py::int_())) {
                    config->set(key, static_cast<int64_t>(py::int_(it.second)), ConfigSource::User);
                } else if (py::isinstance(it.second, py::float_())) {
                    config->set(key, static_cast<float>(py::float_(it.second)), ConfigSource::User);
                } else if (py::isinstance(it.second, py::str())) {
                    config->set(key, py::str(it.second), ConfigSource::User);
                }
            }
        }, py::arg("config"), py::arg("dict"), "Import configuration from dictionary (convenience method)")
        .def("exportConfigToDict", [](std::shared_ptr<Config> config) {
            auto result = py::dict();
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                if (config->has(key)) {
                    // Try to get the value and determine its type
                    if (auto intVal = config->get<int64_t>(key)) {
                        result[py::str(key)] = *intVal;
                    } else if (auto floatVal = config->get<float>(key)) {
                        result[py::str(key)] = *floatVal;
                    } else if (auto strVal = config->get<std::string>(key)) {
                        result[py::str(key)] = *strVal;
                    }
                }
            }
            return result;
        }, py::arg("config"), "Export configuration to dictionary (convenience method)");

    // ============= AGENT BRAIN CLASS =============
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        // Enhanced Python API methods
        .def("createAgentWithConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<AgentBrain> {
            auto brain = std::make_shared<Brain>(config);
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("config"), "Create agent brain with configuration (convenience method)")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create agent brain from brain (convenience method)")
        .def("runEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                            int steps, int episodes) {
            for (int episode = 0; episode < episodes; ++episode) {
                world->reset();
                agent->reset();
                
                for (int step = 0; step < steps; ++step) {
                    const auto& percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    // Run multiple brain steps per action
                    for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                        agent->getBrain()->step(episode * steps + step * 10 + neuralStep);
                    }
                    
                    auto cmd = agent->decodeMotorCommand();
                    world->applyMotorCommand(cmd, world->getSimulationTime());
                    world->update(0.1);
                    agent->updateDevelopment(0.1);
                }
            }
        }, py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("episodes"),
             "Run complete episode simulation (convenience method)")
        .def("getEpisodeMetrics", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                                   int steps) {
            auto metrics = py::dict();
            int totalReward = 0;
            int actionCount = 0;
            
            for (int step = 0; step < steps; ++step) {
                const auto& percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    agent->getBrain()->step(step * 10 + neuralStep);
                }
                
                auto cmd = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(cmd, world->getSimulationTime());
                world->update(0.1);
                agent->updateDevelopment(0.1);
                
                totalReward += result.reward;
                actionCount++;
            }
            
            metrics["total_reward"] = totalReward;
            metrics["average_reward_per_step"] = static_cast<float>(totalReward) / steps;
            metrics["actions_taken"] = actionCount;
            return metrics;
        }, py::arg("agent"), py::arg("world"), py::arg("steps"),
             "Get episode performance metrics (convenience method)");

    // ============= FACTORY FUNCTIONS =============
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (factory function)");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (factory function)");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (factory function)");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (factory function)");

    // ============= VALIDATION AND UTILITY FUNCTIONS =============
    m.def("validateBrainConfiguration", [](std::shared_ptr<Config> config) {
        bool valid = true;
        std::vector<std::string> errors;
        
        if (!config->has("neuron_count")) {
            valid = false;
            errors.push_back("Missing required parameter: neuron_count");
        } else {
            auto neuronCount = config->get<int64_t>("neuron_count");
            if (!neuronCount || *neuronCount <= 0) {
                valid = false;
                errors.push_back("Invalid neuron_count: must be positive");
            }
        }
        
        if (!config->has("region_count")) {
            valid = false;
            errors.push_back("Missing required parameter: region_count");
        }
        
        if (!config->has("simulation_timestep")) {
            valid = false;
            errors.push_back("Missing required parameter: simulation_timestep");
        } else {
            auto timestep = config->get<double>("simulation_timestep");
            if (!timestep || *timestep <= 0.0) {
                valid = false;
                errors.push_back("Invalid simulation_timestep: must be positive");
            }
        }
        
        if (!config->has("connection_probability")) {
            valid = false;
            errors.push_back("Missing required parameter: connection_probability");
        } else {
            auto prob = config->get<float>("connection_probability");
            if (!prob || *prob < 0.0f || *prob > 1.0f) {
                valid = false;
                errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
            }
        }
        
        return py::make_tuple(valid, errors);
    }, py::arg("config"), "Validate brain configuration (utility function)");

    m.def("estimateSimulationTime", [](int neuronCount, int steps, double timestep = 0.001) {
        // Very rough estimate: assume 1ms per 1000 neurons per step
        double estimatedSeconds = (neuronCount / 1000.0) * (steps * timestep);
        return estimatedSeconds;
    }, py::arg("neuronCount"), py::arg("steps"), py::arg("timestep") = 0.001,
         "Estimate simulation time in seconds (utility function)");

    m.def("benchmarkConfiguration", [](std::shared_ptr<Config> config, int steps = 1000) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for benchmarking");
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double msPerStep = static_cast<double>(duration.count()) / steps;
        
        return py::dict(
            "total_time_ms", duration.count(),
            "time_per_step_ms", msPerStep,
            "steps_per_second", 1000.0 / msPerStep,
            "neurons", config->getOr<int64_t>("neuron_count", 1000),
            "steps", steps
        );
    }, py::arg("config"), py::arg("steps") = 1000,
         "Run benchmark test on configuration (utility function)");

    // ============= ALIASES AND SHORTCUTS =============
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Convenience constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;
    m.attr("DEFAULT_RANDOM_SEED") = 42;

    // ============= DEVELOPER HELPERS =============
    m.def("getVersionInfo", []() {
        return py::dict(
            "version", "1.0.0",
            "build_date", "2026-10-31",
            "description", "Enhanced NLM Python bindings with improved API",
            "features", std::vector<std::string>{
                "Enhanced convenience methods",
                "Better error handling",
                "Improved configuration utilities",
                "Integration helpers",
                "Performance monitoring",
                "Validation utilities"
            }
        );
    }, "Get version and feature information (developer helper)");

    // Set module-level documentation
    m.attr("__version__") = "1.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced NLM Python bindings for neural simulation";

} // namespace nlm
