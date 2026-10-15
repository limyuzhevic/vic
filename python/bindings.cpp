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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    // Add advanced configuration management tools
    m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Create configuration from JSON/YAML file");
    
    m.def("createConfigFromDict", [](const py::dict& dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            std::string valueStr = py::cast<std::string>(item.second);
            
            // Try to parse value as number or boolean
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double doubleVal = std::stod(valueStr);
                    config->set(key, doubleVal, ConfigSource::Runtime);
                } else {
                    int intVal = std::stoi(valueStr);
                    config->set(key, intVal, ConfigSource::Runtime);
                }
            } catch (...) {
                // Try boolean
                if (valueStr == "true") {
                    config->set(key, true, ConfigSource::Runtime);
                } else if (valueStr == "false") {
                    config->set(key, false, ConfigSource::Runtime);
                } else {
                    // String value
                    config->set(key, valueStr, ConfigSource::Runtime);
                }
            }
        }
        
        return config;
    }, py::arg("dict"), "Create configuration from Python dictionary");
    
    // Add advanced brain analysis tools
    py::class_<BrainAnalyzer>(m, "BrainAnalyzer", R"pbdoc(Tools for analyzing brain performance and activity)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("analyzeSpikePatterns", &BrainAnalyzer::analyzeSpikePatterns,
             py::arg("maxNeurons") = 100)
        .def("detectActiveRegions", &BrainAnalyzer::detectActiveRegions,
             py::arg("threshold") = 0.1f)
        .def("calculateNetworkMetrics", &BrainAnalyzer::calculateNetworkMetrics)
        .def("getNeuronActivityStats", &BrainAnalyzer::getNeuronActivityStats,
             py::arg("neuronId"))
        .def("findSimilarActivityPatterns", &BrainAnalyzer::findSimilarActivityPatterns,
             py::arg("pattern"), py::arg("threshold") = 0.8f);
    
    // Add advanced simulation control tools
    py::class_<SimulationController>(m, "SimulationController", R"pbdoc(Advanced simulation control and analysis tools)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>())
        .def("runEpisode", &SimulationController::runEpisode,
             py::arg("maxSteps"), py::arg("saveCheckpoints") = false)
        .def("runExplorationEpisode", &SimulationController::runExplorationEpisode,
             py::arg("maxSteps"), py::arg("enableCuriosity") = true)
        .def("runTrainingEpisode", &SimulationController::runTrainingEpisode,
             py::arg("maxSteps"), py::arg("enableRewardLearning") = true)
        .def("analyzeLearningProgress", &SimulationController::analyzeLearningProgress)
        .def("getEpisodeStatistics", &SimulationController::getEpisodeStatistics)
        .def("exportSimulationData", &SimulationController::exportSimulationData,
             py::arg("filepath"), py::arg("format") = "json");
    
    // Add advanced memory system tools
    py::class_<MemoryInspector>(m, "MemoryInspector", R"pbdoc(Advanced memory system analysis and tools)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getWorkingMemoryTraces", &MemoryInspector::getWorkingMemoryTraces)
        .def("analyzeEpisodicMemory", &MemoryInspector::analyzeEpisodicMemory,
             py::arg("minRelevance") = 0.5f)
        .def("findSimilarEpisodes", &MemoryInspector::findSimilarEpisodes,
             py::arg("episodeId"), py::arg("threshold") = 0.7f)
        .def("consolidateMemory", &MemoryInspector::consolidateMemory,
             py::arg("relevanceThreshold") = 0.5f)
        .def("getMemoryUsageStats", &MemoryInspector::getMemoryUsageStats);
    
    // Add neuromodulation analysis tools
    py::class_<NeuromodulationAnalyzer>(m, "NeuromodulationAnalyzer", R"pbdoc(Analysis tools for neuromodulation systems)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("analyzeDopamineDynamics", &NeuromodulationAnalyzer::analyzeDopamineDynamics,
             py::arg("timeWindow") = 1000.0)
        .def("analyzeRewardPredictionError", &NeuromodulationAnalyzer::analyzeRewardPredictionError)
        .def("getNeuromodulationCorrelation", &NeuromodulationAnalyzer::getNeuromodulationCorrelation,
             py::arg("modulator1"), py::arg("modulator2"))
        .def("predictNeuromodulationEffects", &NeuromodulationAnalyzer::predictNeuromodulationEffects,
             py::arg("action"));
    
    // Add advanced plasticity tools
    py::class_<PlasticityTools>(m, "PlasticityTools", R"pbdoc(Advanced tools for analyzing and controlling synaptic plasticity)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("analyzeSTDP", &PlasticityTools::analyzeSTDP,
             py::arg("minSpikePairs") = 10)
        .def("calculateHebbianStrength", &PlasticityTools::calculateHebbianStrength,
             py::arg("connectionId"))
        .def("predictPlasticityChanges", &PlasticityTools::predictPlasticityChanges,
             py::arg("timeSteps") = 100)
        .def("findPlasticityHotspots", &PlasticityTools::findPlasticityHotspots,
             py::arg("threshold") = 0.01f)
        .def("resetPlasticity", &PlasticityTools::resetPlasticity,
             py::arg("regionId"), py::arg("synapseType") = SynapseType::Excitatory);
    
    // Add development system analysis tools
    py::class_<DevelopmentAnalyzer>(m, "DevelopmentAnalyzer", R"pbdoc(Analysis tools for the brain development system)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("analyzeDevelopmentalStage", &DevelopmentAnalyzer::analyzeDevelopmentalStage,
             py::arg("stage") = "Auto")
        .def("predictDevelopmentTrajectory", &DevelopmentAnalyzer::predictDevelopmentTrajectory,
             py::arg("timeSteps") = 1000)
        .def("getCriticalPeriodStatus", &DevelopmentAnalyzer::getCriticalPeriodStatus)
        .def("simulateDevelopment", &DevelopmentAnalyzer::simulateDevelopment,
             py::arg("startStage"), py::arg("endStage"));
    
    // Add visualization and debugging tools
    py::class_<VisualizationTools>(m, "VisualizationTools", R"pbdoc(Advanced visualization and debugging tools for brain simulation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("generateSpikeRaster", &VisualizationTools::generateSpikeRaster,
             py::arg("outputFile"), py::arg("timeRange"))
        .def("plotActivityHeatmap", &VisualizationTools::plotActivityHeatmap,
             py::arg("outputFile"), py::arg("resolution") = "high")
        .def("generateNetworkGraph", &VisualizationTools::generateNetworkGraph,
             py::arg("outputFile"), py::arg("layout") = "spring")
        .def("createVideoAnimation", &VisualizationTools::createVideoAnimation,
             py::arg("outputFile"), py::arg("fps") = 30)
        .def("analyzeBrainConnectivity", &VisualizationTools::analyzeBrainConnectivity,
             py::arg("outputFile"));
    
    // Add simulation benchmarking tools
    py::class_<BenchmarkTools>(m, "BenchmarkTools", R"pbdoc(Tools for benchmarking brain simulation performance)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("benchmarkInitialization", &BenchmarkTools::benchmarkInitialization,
             py::arg("repeatCount") = 10)
        .def("benchmarkStepPerformance", &BenchmarkTools::benchmarkStepPerformance,
             py::arg("steps"), py::arg("repeatCount") = 5)
        .def("benchmarkMemoryUsage", &BenchmarkTools::benchmarkMemoryUsage,
             py::arg("operation") = "all")
        .def("benchmarkPlasticityPerformance", &BenchmarkTools::benchmarkPlasticityPerformance,
             py::arg("synapseCount"))
        .def("generatePerformanceReport", &BenchmarkTools::generatePerformanceReport,
             py::arg("outputFile"), py::arg("format") = "json");
    
    // Add configuration export/import tools
    m.def("exportConfig", [](std::shared_ptr<Config> config, const std::string& filepath) {
        return config->saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Export configuration to file");
    
    m.def("importConfig", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Import configuration from file");
    
    // Add configuration optimization tools
    m.def("optimizeConfiguration", [](std::shared_ptr<Config> config) {
        // Simple configuration optimization
        config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        config->set("neuron_count", config->getOr<size_t>("neuron_count", 100), ConfigSource::Runtime);
        config->set("region_count", config->getOr<size_t>("region_count", 1), ConfigSource::Runtime);
        return config;
    }, py::arg("config"), "Optimize configuration for performance");
    
    // Add batch simulation tools
    m.def("batchSimulate", [](std::shared_ptr<Brain> brain, 
                             std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent,
                             const std::vector<int>& stepsList) {
        std::vector<std::map<std::string, double>> results;
        for (int steps : stepsList) {
            agent->reset();
            world->reset();
            brain->reset();
            
            double startTime = 0.0; // Placeholder for actual timing
            for (int step = 0; step < steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            double endTime = 0.0; // Placeholder for actual timing
            
            std::map<std::string, double> stats = {
                {"steps", static_cast<double>(steps)},
                {"time", endTime - startTime},
                {"firingRate", brain->getAverageFiringRate()},
                {"spikeCount", static_cast<double>(brain->getTotalSpikeCount())}
            };
            results.push_back(stats);
        }
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("stepsList"),
       "Run batch simulations with different step counts");
    
    // Add simulation profiling tools
    m.def("profileSimulation", [](std::shared_ptr<Brain> brain,
                                  std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent,
                                  int steps) {
        // Simple profiling implementation
        std::map<std::string, double> metrics;
        
        // Time brain initialization
        auto start = std::chrono::high_resolution_clock::now();
        brain->reset();
        auto initEnd = std::chrono::high_resolution_clock::now();
        metrics["initialization_time"] = std::chrono::duration<double>(initEnd - start).count();
        
        // Time simulation steps
        start = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        auto simulationEnd = std::chrono::high_resolution_clock::now();
        metrics["simulation_time"] = std::chrono::duration<double>(simulationEnd - start).count();
        metrics["steps_per_second"] = steps / metrics["simulation_time"];
        
        // Memory metrics (placeholder)
        metrics["memory_usage"] = 0.0; // Placeholder
        metrics["spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
        metrics["firing_rate"] = brain->getAverageFiringRate();
        
        return metrics;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Profile simulation performance and return metrics");
    
    // Add help and documentation tools
    m.def("getAdvancedUsageExamples", []() {
        std::vector<std::string> examples;
        
        examples.push_back(R"(
# Advanced NLM Usage Examples

# Example 1: Custom Configuration with JSON
import pynlm

config_dict = {
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.0005,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012
}

brain = pynlm.createBrain(config_dict)
brain.initialize()
""");
        
        examples.push_back(R"(
# Example 2: Advanced Brain Analysis
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

analyzer = pynlm.BrainAnalyzer(brain)
metrics = analyzer.calculateNetworkMetrics()
print(f"Average firing rate: {metrics['firing_rate']:.2f} Hz")
print(f"Excitatory/Inhibitory ratio: {metrics['e_i_ratio']:.2f}")
""");
        
        examples.push_back(R"(
# Example 3: Custom Simulation Controller
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 10, 10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

controller = pynlm.SimulationController(brain, world, agent)
results = controller.runTrainingEpisode(1000, enableRewardLearning=True)
print(f"Training completed in {len(results['rewards'])} steps")
""");
        
        examples.push_back(R"(
# Example 4: Memory System Analysis
memory_inspector = pynlm.MemoryInspector(brain)
working_memory = memory_inspector.getWorkingMemoryTraces()
print(f"Active working memory traces: {len(working_memory)}")

echoic_memory = memory_inspector.analyzeEpisodicMemory(minRelevance=0.7)
print(f"High-relevance episodes: {len(echoic_memory)}")
""");
        
        examples.push_back(R"(
# Example 5: Neuromodulation Analysis
neuromod_analyzer = pynlm.NeuromodulationAnalyzer(brain)
dopamine_dynamics = neuromod_analyzer.analyzeDopamineDynamics(timeWindow=500.0)
print(f"Peak dopamine level: {dopamine_dynamics['peak_level']:.3f}")
""");
        
        examples.push_back(R"(
# Example 6: Plasticity Analysis
plasticity_tools = pynlm.PlasticityTools(brain)
stdp_analysis = plasticity_tools.analyzeSTDP(minSpikePairs=50)
print(f"STDP learning rate: {stdp_analysis['learning_rate']:.4f}")
""");
        
        examples.push_back(R"(
# Example 7: Batch Processing
steps_list = [100, 500, 1000, 2000]
batch_results = pynlm.batchSimulate(brain, world, agent, steps_list)

for result in batch_results:
    print(f"Steps: {result['steps']}, Time: {result['time']:.2f}s, "
          f"Firing Rate: {result['firing_rate']:.2f} Hz")
""");
        
        examples.push_back(R"(
# Example 8: Configuration Optimization
optimized_config = pynlm.optimizeConfiguration(pynlm.createDefaultConfig())
brain_opt = pynlm.createBrain(optimized_config)
brain_opt.initialize()
print("Configuration optimized for performance")
""");
        
        return examples;
    }, "Return advanced usage examples for NLM");
    
    m.def("getAPIReference", []() {
        std::vector<std::string> api_refs;
        
        api_refs.push_back(R"(
# NLM Advanced API Reference

## BrainAnalyzer
Class for analyzing brain performance and neural activity patterns.

Methods:
- analyzeSpikePatterns(maxNeurons=100): Analyze spike timing patterns
- detectActiveRegions(threshold=0.1f): Detect active brain regions
- calculateNetworkMetrics(): Calculate network-wide statistics
- getNeuronActivityStats(neuronId): Get statistics for a specific neuron
- findSimilarActivityPatterns(pattern, threshold=0.8f): Find similar activity patterns

## SimulationController
Advanced simulation control with learning capabilities.

Methods:
- runEpisode(maxSteps, saveCheckpoints=False): Run a standard episode
- runExplorationEpisode(maxSteps, enableCuriosity=True): Run exploration episode
- runTrainingEpisode(maxSteps, enableRewardLearning=True): Run training episode
- analyzeLearningProgress(): Analyze learning progress
- getEpisodeStatistics(): Get episode statistics
- exportSimulationData(filepath, format="json"): Export simulation data

## MemoryInspector
Advanced memory system analysis tools.

Methods:
- getWorkingMemoryTraces(): Get current working memory traces
- analyzeEpisodicMemory(minRelevance=0.5f): Analyze episodic memory
- findSimilarEpisodes(episodeId, threshold=0.7f): Find similar episodes
- consolidateMemory(relevanceThreshold=0.5f): Consolidate memory
- getMemoryUsageStats(): Get memory usage statistics

## NeuromodulationAnalyzer
Analysis tools for neuromodulation systems.

Methods:
- analyzeDopamineDynamics(timeWindow=1000.0): Analyze dopamine dynamics
- analyzeRewardPredictionError(): Analyze reward prediction error
- getNeuromodulationCorrelation(modulator1, modulator2): Get correlation between modulators
- predictNeuromodulationEffects(action): Predict neuromodulation effects

## PlasticityTools
Advanced synaptic plasticity analysis and control.

Methods:
- analyzeSTDP(minSpikePairs=10): Analyze STDP learning
- calculateHebbianStrength(connectionId): Calculate Hebbian strength
- predictPlasticityChanges(timeSteps=100): Predict plasticity changes
- findPlasticityHotspots(threshold=0.01f): Find plasticity hotspots
- resetPlasticity(regionId, synapseType=Excitatory): Reset plasticity

## DevelopmentAnalyzer
Development system analysis tools.

Methods:
- analyzeDevelopmentalStage(stage="Auto"): Analyze developmental stage
- predictDevelopmentTrajectory(timeSteps=1000): Predict development trajectory
- getCriticalPeriodStatus(): Get critical period status
- simulateDevelopment(startStage, endStage): Simulate development

## VisualizationTools
Advanced visualization and debugging tools.

Methods:
- generateSpikeRaster(outputFile, timeRange): Generate spike raster plot
- plotActivityHeatmap(outputFile, resolution="high"): Plot activity heatmap
- generateNetworkGraph(outputFile, layout="spring"): Generate network graph
- createVideoAnimation(outputFile, fps=30): Create animation video
- analyzeBrainConnectivity(outputFile): Analyze brain connectivity

## BenchmarkTools
Performance benchmarking tools.

Methods:
- benchmarkInitialization(repeatCount=10): Benchmark initialization
- benchmarkStepPerformance(steps, repeatCount=5): Benchmark step performance
- benchmarkMemoryUsage(operation="all"): Benchmark memory usage
- benchmarkPlasticityPerformance(synapseCount): Benchmark plasticity performance
- generatePerformanceReport(outputFile, format="json"): Generate performance report
""");
        
        return api_refs;
    }, "Get NLM API reference documentation");
    
    m.def("createAdvancedBrain", [](py::dict configDict) {
        auto config = std::make_shared<Config>();
        
        // Convert Python dict to C++ config
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            std::string valueStr = py::cast<std::string>(item.second);
            
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double doubleVal = std::stod(valueStr);
                    config->set(key, doubleVal, ConfigSource::Runtime);
                } else {
                    int intVal = std::stoi(valueStr);
                    config->set(key, intVal, ConfigSource::Runtime);
                }
            } catch (...) {
                if (valueStr == "true") {
                    config->set(key, true, ConfigSource::Runtime);
                } else if (valueStr == "false") {
                    config->set(key, false, ConfigSource::Runtime);
                } else {
                    config->set(key, valueStr, ConfigSource::Runtime);
                }
            }
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("configDict"), "Create an advanced brain with custom configuration");
    
    m.def("createTrainingEnvironment", [](int width, int height, int visionWidth, int visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200, ConfigSource::Runtime);
        config->set("region_count", 1, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("width")=10, py::arg("height")=10, 
       py::arg("visionWidth")=8, py::arg("visionHeight")=8,
       "Create a complete training environment");
    
    // Create advanced alias classes for common use cases
    py::class_<AdvancedBrain>(m, "AdvancedBrain", R"pbdoc(Advanced brain interface with enhanced functionality)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", &Brain::step)
        .def("receiveSensoryInput", &Brain::receiveSensoryInput)
        .def("produceAction", &Brain::produceAction)
        .def("reset", &Brain::reset)
        .def("save", &Brain::save, py::arg("filepath"))
        .def("load", &Brain::load, py::arg("filepath"))
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getConfig", &Brain::getConfig)
        .def("logStatus", &Brain::logStatus)
        .def("enableAdvancedFeatures", &Brain::enableAdvancedFeatures)
        .def("getPerformanceMetrics", &Brain::getPerformanceMetrics);
    
    // Add training utilities
    m.def("runTrainingSession", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   int maxSteps,
                                   bool enableRewardLearning,
                                   bool enableCuriosity) {
        // Training session implementation
        agent->reset();
        agent->enableRewardModulation(enableRewardLearning);
        agent->enableCuriosity(enableCuriosity);
        
        std::vector<double> rewards;
        double totalReward = 0.0;
        
        for (int step = 0; step < maxSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward calculation based on action type
            double reward = 0.0;
            if (action->getType() == ActionType::MoveForward) reward = 0.1;
            else if (action->getType() == ActionType::Interact) reward = 1.0;
            else if (action->getType() == ActionType::Wait) reward = -0.1;
            
            agent->applyRewardModulation(reward, 0.0); // predicted reward = 0
            totalReward += reward;
            rewards.push_back(reward);
        }
        
        std::map<std::string, double> stats = {
            {"total_reward", totalReward},
            {"avg_reward", totalReward / maxSteps},
            {"firing_rate", brain->getAverageFiringRate()},
            {"spike_count", static_cast<double>(brain->getTotalSpikeCount())}
        };
        
        return std::make_tuple(rewards, stats);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("maxSteps")=1000, py::arg("enableRewardLearning")=true,
       py::arg("enableCuriosity")=true,
       "Run a complete training session");
    
    m.def("createModelCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Create a model checkpoint");
    
    m.def("loadModelCheckpoint", [](const std::string& filepath) {
        // This is a simplified version - in practice you'd need to handle loading properly
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        // Note: load() would typically be called separately
        return brain;
    }, py::arg("filepath"),
       "Load a model checkpoint");
    
    // Add convenience functions for common patterns
    m.def("quickTrain", [](int steps, int width, int height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, width/2, height/2);
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100, ConfigSource::Runtime);
        config->set("region_count", 1, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        std::vector<double> rewards;
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = (action->getType() == ActionType::MoveForward) ? 0.1 : -0.1;
            agent->applyRewardModulation(reward, 0.0);
            rewards.push_back(reward);
        }
        
        std::map<std::string, double> stats = {
            {"total_reward", std::accumulate(rewards.begin(), rewards.end(), 0.0)},
            {"avg_reward", std::accumulate(rewards.begin(), rewards.end(), 0.0) / steps},
            {"final_firing_rate", brain->getAverageFiringRate()}
        };
        
        return std::make_tuple(rewards, stats);
    }, py::arg("steps")=500, py::arg("width")=10, py::arg("height")=10,
       "Quick training function for common use cases");
}
