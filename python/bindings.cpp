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

    // Enhanced configuration API with parameter validation
    .def("configure", &Config::configure, py::arg("key"), py::arg("value"),
         "Set configuration parameter with type validation")
    .def("get", py::overload_cast<const std::string&>(&Config::get),
         py::arg("key"), "Get configuration parameter")
    .def("getWithDefault", [](const Config& self, const std::string& key, const std::string& defaultValue) {
        auto result = self.get<std::string>(key);
        return result.has_value() ? result.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
         "Get configuration parameter with default value")
    .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
        auto result = self.get<int>(key);
        return result.has_value() ? result.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
         "Get integer configuration parameter with default value")
    .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
        auto result = self.get<float>(key);
        return result.has_value() ? result.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
         "Get float configuration parameter with default value")
    .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
        auto result = self.get<bool>(key);
        return result.has_value() ? result.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
         "Get boolean configuration parameter with default value")
    .def("validateConfig", &Config::validate, "Validate configuration parameters")
    .def("getConfigTemplate", &Config::getTemplate, "Get configuration parameter template")
    .def("mergeConfig", &Config::mergeConfig, py::arg("otherConfig"), "Merge another configuration")
    
    // Enhanced debugging and monitoring tools
    .def("startMonitoring", &Brain::startMonitoring,
         "Start real-time monitoring of brain activity")
    .def("stopMonitoring", &Brain::stopMonitoring,
         "Stop real-time monitoring")
    .def("isMonitoring", &Brain::isMonitoring,
         "Check if monitoring is active")
    .def("getMonitorData", &Brain::getMonitorData,
         "Get current monitoring data")
    .def("exportMetrics", &Brain::exportMetrics, py::arg("filepath"),
         "Export metrics to file")
    .def("getPerformanceMetrics", &Brain::getPerformanceMetrics,
         "Get performance metrics")
    
    // Enhanced serialization utilities
    .def("saveWithMetadata", [](Brain& self, const std::string& filepath, const std::string& metadata) {
        return self.save(filepath, metadata);
    }, py::arg("filepath"), py::arg("metadata"),
         "Save brain state with metadata")
    .def("loadWithVersion", [](Brain& self, const std::string& filepath, uint32_t expectedVersion) {
        return self.load(filepath, expectedVersion);
    }, py::arg("filepath"), py::arg("expectedVersion"),
         "Load brain state with version validation")
    .def("validateCheckpoint", &Brain::validateCheckpoint, py::arg("filepath"),
         "Validate checkpoint file integrity")
    .def("getCheckpointInfo", &Brain::getCheckpointInfo, py::arg("filepath"),
         "Get checkpoint file information")
    
    // Performance monitoring and timing tools
    .def("timeStep", &Brain::timeStep, py::arg("currentStep"),
         "Time a single brain step operation")
    .def("getTimingStats", &Brain::getTimingStats,
         "Get timing statistics for brain operations")
    .def("resetTiming", &Brain::resetTiming,
         "Reset timing statistics")
    .def("getMemoryUsage", &Brain::getMemoryUsage,
         "Get current memory usage")
    .def("getCPUUsage", &Brain::getCPUUsage,
         "Get current CPU usage")
    
    // Batch processing tools
    .def("runBatchSimulation", [](std::shared_ptr<Brain> brain, int numSteps, int batchSize) {
        std::vector<std::shared_ptr<Brain>> results;
        brain->runBatchSimulation(numSteps, batchSize, results);
        return results;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("batchSize"),
         "Run batch simulation and return results")
    .def("runParallelSimulation", [](std::shared_ptr<Brain> brain, int numSteps, int numThreads) {
        std::vector<std::shared_ptr<Brain>> results;
        brain->runParallelSimulation(numSteps, numThreads, results);
        return results;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("numThreads"),
         "Run parallel simulation and return results")
    
    // Interactive exploration tools
    .def("exploreParameterSpace", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& parameters, 
                                  const std::vector<std::vector<float>>& ranges, int numSamples) {
        std::vector<std::shared_ptr<Brain>> results;
        brain->exploreParameterSpace(parameters, ranges, numSamples, results);
        return results;
    }, py::arg("brain"), py::arg("parameters"), py::arg("ranges"), py::arg("numSamples"),
         "Explore parameter space and return results")
    .def("getExplorationResults", &Brain::getExplorationResults,
         "Get exploration results")
    
    // Advanced AgentBrain interface
    .def("createAdvancedAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                  const std::string& strategy, const std::string& policy) {
        return std::make_shared<AgentBrain>(brain, world, strategy, policy);
    }, py::arg("brain"), py::arg("world"), py::arg("strategy"), py::arg("policy"),
         "Create advanced agent with strategy and policy")
    .def("enableAdvancedFeatures", [](AgentBrain& agent, bool rewardModulation, bool structuralPlasticity,
                                    bool development, bool curiosity, bool prediction) {
        agent.enableAdvancedFeatures(rewardModulation, structuralPlasticity, development, curiosity, prediction);
    }, py::arg("rewardModulation"), py::arg("structuralPlasticity"), py::arg("development"),
       py::arg("curiosity"), py::arg("prediction"),
         "Enable advanced agent features")
    .def("setAgentPolicy", &AgentBrain::setPolicy, py::arg("policy"),
         "Set agent policy")
    .def("getAgentPolicy", &AgentBrain::getPolicy,
         "Get agent policy")
    
    // Simple high-level APIs for common use cases
    m.def("createBrainWithDefaults", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create brain with default configuration")
    
    m.def("runSimpleSimulation", [](int numSteps, int neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 1);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int i = 0; i < numSteps; ++i) {
            brain->step(i);
        }
        
        return brain;
    }, py::arg("numSteps"), py::arg("neuronCount"),
         "Run simple simulation and return brain")
    
    m.def("createAgentWithWorld", [](std::shared_ptr<Brain> brain, int width, int height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("width"), py::arg("height"),
         "Create agent with world")
    
    // Serialization and checkpoint utilities
    m.def("createCheckpoint", &nlm::createCheckpoint, py::arg("name"), py::arg("description"),
         "Create new checkpoint with name and description")
    m.def("getAvailableCheckpoints", &nlm::getAvailableCheckpoints,
         "Get list of available checkpoints")
    m.def("deleteCheckpoint", &nlm::deleteCheckpoint, py::arg("name"),
         "Delete checkpoint")
    m.def("exportForAnalysis", &nlm::exportForAnalysis, py::arg("brain"), py::arg("outputPath"),
         "Export brain for external analysis")
    m.def("importForAnalysis", &nlm::importForAnalysis, py::arg("inputPath"),
         "Import brain from external analysis")
    
    // Visualization and plotting tools (lightweight)
    m.def("createSimplePlot", &nlm::createSimplePlot, py::arg("data"), py::arg("title"), py::arg("filename"),
         "Create simple plot of simulation data")
    m.def("plotMetrics", &nlm::plotMetrics, py::arg("brain"), py::arg("metrics"), py::arg("filename"),
         "Plot brain metrics")
    m.def("animateSimulation", &nlm::animateSimulation, py::arg("brain"), py::arg("steps"), py::arg("outputPath"),
         "Animate simulation")
    
    // Utility functions and helpers
    m.def("benchmarkBrain", &nlm::benchmarkBrain, py::arg("brain"), py::arg("steps"), py::arg("warmup"),
         "Benchmark brain performance")
    m.def("validateBrainHealth", &nlm::validateBrainHealth, py::arg("brain"),
         "Validate brain health and integrity")
    m.def("getBrainDiagnostics", &nlm::getBrainDiagnostics, py::arg("brain"),
         "Get brain diagnostic information")
    m.def("repairBrain", &nlm::repairBrain, py::arg("brain"),
         "Attempt to repair brain integrity issues")
    
    // Error handling and recovery
    .def("safeStep", [](Brain& self, int step) {
        try {
            self.step(step);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Step failed: ") + e.what());
            return false;
        }
    }, py::arg("step"),
         "Safely execute brain step with error handling")
    .def("getLastError", &Brain::getLastError,
         "Get last error message")
    .def("clearErrors", &Brain::clearErrors,
         "Clear error history")
    
    // Advanced visualization and analysis
    .def("createNeuroView", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<NeuroView>(brain);
    }, py::arg("brain"),
         "Create neuroscientific visualization view")
    .def("createBehaviorPlot", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        return std::make_shared<BehaviorPlot>(brain, world);
    }, py::arg("brain"), py::arg("world"),
         "Create behavior visualization plot")
    .def("createMemoryTrace", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<MemoryTrace>(brain);
    }, py::arg("brain"),
         "Create memory trace visualization")
    .def("createTimeSeries", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& metrics) {
        return std::make_shared<TimeSeries>(brain, metrics);
    }, py::arg("brain"), py::arg("metrics"),
         "Create time series plot of metrics")
    
    // Session management and persistence
    m.def("createSession", &nlm::createSession, py::arg("name"), py::arg("description"),
         "Create simulation session")
    m.def("getSession", &nlm::getSession, py::arg("name"),
         "Get simulation session")
    m.def("listSessions", &nlm::listSessions,
         "List all simulation sessions")
    m.def("endSession", &nlm::endSession, py::arg("name"),
         "End simulation session")
    m.def("saveSession", &nlm::saveSession, py::arg("name"), py::arg("filepath"),
         "Save session to file")
    m.def("loadSession", &nlm::loadSession, py::arg("filepath"),
         "Load session from file")
    
    // Educational and research tools
    m.def("createTutorialBrain", &nlm::createTutorialBrain,
         "Create brain for educational tutorials")
    m.def("runTutorial", &nlm::runTutorial, py::arg("tutorialName"),
         "Run specific tutorial")
    m.def("getTutorialList", &nlm::getTutorialList,
         "Get list of available tutorials")
    m.def("createResearchExperiment", &nlm::createResearchExperiment, py::arg("experimentType"), py::arg("parameters"),
         "Create research experiment")
    m.def("runResearchExperiment", &nlm::runResearchExperiment, py::arg("experiment"), py::arg("brain"),
         "Run research experiment")
    m.def("analyzeResults", &nlm::analyzeResults, py::arg("results"), py::arg("analysisType"),
         "Analyze experiment results")
}

} // namespace nlm
