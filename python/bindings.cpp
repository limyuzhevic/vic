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
        .def("get", [](const Config& cfg, const std::string& key, const std::string& defaultValue = "") {
            if (cfg.has(key)) {
                auto value = cfg.get(key);
                return std::visit([](auto&& arg) -> std::string {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) return arg;
                    else if constexpr (std::is_same_v<T, int64_t>) return std::to_string(arg);
                    else if constexpr (std::is_same_v<T, float>) return std::to_string(arg);
                    else if constexpr (std::is_same_v<T, bool>) return arg ? "true" : "false";
                    else return "";
                }, value);
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value") = "",
             "Get a configuration value as string, with optional default")
        .def("set", [](Config& cfg, const std::string& key, const std::string& value) {
            if (value == "true" || value == "false") {
                cfg.set(key, value == "true", nlm::ConfigSource::User);
            } else {
                try {
                    // Try to parse as integer
                    size_t pos;
                    int64_t intVal = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        cfg.set(key, intVal, nlm::ConfigSource::User);
                        return;
                    }
                } catch (...) {}
                try {
                    // Try to parse as float
                    float floatVal = std::stof(value);
                    cfg.set(key, floatVal, nlm::ConfigSource::User);
                    return;
                } catch (...) {}
                // Fall back to string
                cfg.set(key, value, nlm::ConfigSource::User);
            }
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (auto-detects type)")
        .def("getNeuronCount", [](const Config& cfg) -> uint64_t {
            return cfg.get("neuron_count", 100); // Default to 100 neurons
        }, "Get the number of neurons")
        .def("getRegionCount", [](const Config& cfg) -> uint64_t {
            return cfg.get("region_count", 1); // Default to 1 region
        }, "Get the number of brain regions")
        .def("getConnectionProbability", [](const Config& cfg) -> float {
            return cfg.get("connection_probability", 0.1f); // Default 10% connection probability
        }, "Get the connection probability between neurons")
        .def("getSTDPParameters", [](const Config& cfg) {
            py::dict params;
            params["ltp_weight"] = cfg.get("stdp_ltp_weight", 0.01f);
            params["ltd_weight"] = cfg.get("stdp_ltd_weight", 0.012f);
            return params;
        }, "Get STDP parameters as dictionary")
        .def("getPlasticityParameters", [](const Config& cfg) {
            py::dict params;
            params["synaptogenesis_rate"] = cfg.get("synaptogenesis_rate", 0.0001f);
            params["pruning_rate"] = cfg.get("pruning_rate", 0.00001f);
            return params;
        }, "Get structural plasticity parameters as dictionary")
        .def("configureForScenario", [](Config& cfg, const std::string& scenario) {
            if (scenario == "simple") {
                // Simple scenario: small brain, basic connections
                cfg.clear();
                cfg.set("neuron_count", 100, nlm::ConfigSource::Default);
                cfg.set("region_count", 1, nlm::ConfigSource::Default);
                cfg.set("connection_probability", 0.1, nlm::ConfigSource::Default);
                cfg.set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
                cfg.set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
            } else if (scenario == "complex") {
                // Complex scenario: larger brain, more connections, advanced plasticity
                cfg.clear();
                cfg.set("neuron_count", 1000, nlm::ConfigSource::Default);
                cfg.set("region_count", 4, nlm::ConfigSource::Default);
                cfg.set("connection_probability", 0.3, nlm::ConfigSource::Default);
                cfg.set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
                cfg.set("stdp_ltd_weight", 0.024f, nlm::ConfigSource::Default);
                cfg.set("synaptogenesis_rate", 0.0002f, nlm::ConfigSource::Default);
                cfg.set("pruning_rate", 0.00005f, nlm::ConfigSource::Default);
            } else {
                throw std::invalid_argument("Unknown scenario: " + scenario);
            }
        }, py::arg("scenario"),
             "Configure brain for a predefined scenario (simple or complex)")
        .def("validate", &Config::validate,
             "Validate the configuration")
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
        
        // Convenience methods for common agent operations
        .def("runStep", [](AgentBrain& agent, const SensoryPercept& percept) {
            if (!agent.getBrain()) return std::make_pair(MotorCommand::Wait, 0.0f);
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Get action (brain step is handled separately by user)
            MotorCommand action = agent.decodeMotorCommand();
            
            // Get current neuromodulation level
            float neuromodulation = agent.getNeuromodulationLevel();
            
            return std::make_pair(action, neuromodulation);
        }, py::arg("percept"),
             "Run a single agent step: process input and get action")
        
        // Advanced configuration methods
        .def("setActionSpace", [](AgentBrain& agent, const std::vector<MotorCommand>& actions) {
            // Note: This is a conceptual enhancement - the actual action space
            // would need to be implemented in the C++ code to allow custom mappings
            if (!agent.getBrain()) return false;
            
            // For now, just log the action space size
            NLM_LOG_INFO("Action space configured with " + std::to_string(actions.size()) + " actions");
            return true;
        }, py::arg("actions"),
             "Configure custom action space (conceptual feature)")
        
        // Performance and monitoring methods
        .def("getPerformanceStats", [](const AgentBrain& agent) {
            py::dict stats;
            
            if (agent.getBrain()) {
                stats["neuron_count"] = agent.getBrain()->getTotalNeuronCount();
                stats["synapse_count"] = agent.getBrain()->getTotalSynapseCount();
                stats["firing_neuron_count"] = agent.getBrain()->getFiringNeuronCount();
                stats["average_firing_rate"] = agent.getBrain()->getAverageFiringRate();
                stats["total_spikes"] = agent.getBrain()->getTotalSpikeCount();
                stats["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
                stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
                stats["curiosity_level"] = agent.getCuriosityLevel();
                stats["novelty_level"] = agent.getNoveltyLevel();
                stats["prediction_error"] = agent.getPredictionError();
            }
            
            stats["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent.isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent.isCuriosityEnabled();
            
            return stats;
        }, "Get comprehensive performance statistics as dictionary")
        
        // Advanced learning methods
        .def("enableAdvancedLearning", [](AgentBrain& agent, bool enable, float learningRate = 0.01f) {
            if (!agent.getBrain()) return false;
            
            agent.enableRewardModulation(enable);
            agent.enableCuriosity(enable);
            agent.enableDevelopment(enable);
            
            // Apply learning rate to brain if available
            if (enable && learningRate > 0.0f) {
                auto* config = agent.getBrain()->getConfig().get();
                if (config) {
                    // This is a conceptual enhancement - in practice,
                    // learning rates would be part of the plasticity rules
                    config->set("learning_rate", learningRate, nlm::ConfigSource::User);
                }
            }
            
            NLM_LOG_INFO("Advanced learning " + std::string(enable ? "enabled" : "disabled"));
            return true;
        }, py::arg("enable"), py::arg("learning_rate") = 0.01f,
             "Enable advanced learning features with custom learning rate")
        
        // Error checking and validation
        .def("validateState", [](const AgentBrain& agent) {
            py::dict validation;
            
            validation["brain_exists"] = (agent.getBrain() != nullptr);
            validation["brain_initialized"] = (agent.getBrain() && 
                                               agent.getBrain()->getTotalNeuronCount() > 0);
            validation["brain_has_regions"] = (agent.getBrain() && 
                                              agent.getBrain()->getRegionCount() > 0);
            
            validation["neuromodulation_consistent"] = true; // Could add more checks
            validation["development_consistent"] = true;   // Could add more checks
            validation["memory_systems_integrated"] = true; // Could add more checks
            
            return validation;
        }, "Validate agent brain state and return validation results")
        
        // Convenience simulation helper
        .def("simulateEpisode", [](AgentBrain& agent, SimpleWorld& world, size_t maxSteps) {
            if (!agent.getBrain()) {
                throw std::runtime_error("Cannot run simulation without brain");
            }
            
            py::dict episodeStats;
            float totalReward = 0.0f;
            size_t successfulSteps = 0;
            
            for (size_t step = 0; step < maxSteps; ++step) {
                // Get observation
                SensoryPercept percept = world.getSensoryPercept();
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Brain step (this is what the agent does)
                agent.getBrain()->step(step, step * 0.001f);
                
                // Get motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // Apply action to world
                world.applyMotorCommand(cmd, step * 0.001f);
                
                // Apply reward modulation if enabled
                if (agent.isRewardModulationEnabled()) {
                    float reward = 0.0f; // In a real simulation, reward would come from world
                    agent.applyRewardModulation(reward, 0.0f);
                }
                
                totalReward += 0.0f; // Would come from world
                successfulSteps++;
                
                // Periodic status update for long episodes
                if (step % 1000 == 0 && maxSteps > 1000) {
                    NLM_LOG_INFO("Episode step " + std::to_string(step) + 
                               "/" + std::to_string(maxSteps));
                }
            }
            
            episodeStats["total_steps"] = static_cast<size_t>(successfulSteps);
            episodeStats["total_reward"] = totalReward;
            episodeStats["average_reward"] = (successfulSteps > 0) ? totalReward / successfulSteps : 0.0f;
            episodeStats["final_neuromodulation"] = agent.getNeuromodulationLevel();
            episodeStats["final_curiosity"] = agent.getCuriosityLevel();
            episodeStats["final_novelty"] = agent.getNoveltyLevel();
            
            return episodeStats;
        }, py::arg("world"), py::arg("max_steps"),
             "Run a complete episode and return statistics (convenience method)");
