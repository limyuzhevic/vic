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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
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
        // getRegion is const/non-const overloaded; bind the mutable one.
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
             "Log brain status");

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

    // END: Basic utility functions

    // BEGIN: Advanced Python utilities and enhanced features
    
    // Advanced performance monitoring utility
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring utility for NLM simulations)pbdoc")
        .def(py::init<>())
        .def("start", &PerformanceMonitor::start)
        .def("stop", &PerformanceMonitor::stop)
        .def("getElapsedTime", &PerformanceMonitor::getElapsedTime)
        .def("getStepsPerSecond", &PerformanceMonitor::getStepsPerSecond)
        .def("reset", &PerformanceMonitor::reset)
        .def("recordSpikeCount", &PerformanceMonitor::recordSpikeCount)
        .def("getStatistics", &PerformanceMonitor::getStatistics)
        .def("getDetailedReport", &PerformanceMonitor::getDetailedReport);

    // Advanced simulation controller
    py::class_<SimulationController>(m, "SimulationController", R"pbdoc(Simulation controller for advanced NLM simulation management)pbdoc")
        .def(py::init<>())
        .def("runEpisode", &SimulationController::runEpisode)
        .def("runCustomPattern", &SimulationController::runCustomPattern)
        .def("enableRealTimeMode", &SimulationController::enableRealTimeMode)
        .def("disableRealTimeMode", &SimulationController::disableRealTimeMode)
        .def("isRealTimeMode", &SimulationController::isRealTimeMode)
        .def("setTargetFPS", &SimulationController::setTargetFPS)
        .def("getCurrentFPS", &SimulationController::getCurrentFPS)
        .def("getSimulationTime", &SimulationController::getSimulationTime)
        .def("setMaxSteps", &SimulationController::setMaxSteps)
        .def("getMaxSteps", &SimulationController::getMaxSteps)
        .def("pause", &SimulationController::pause)
        .def("resume", &SimulationController::resume)
        .def("getBrain", &SimulationController::getBrain, py::return_value_policy::reference_internal);

    // Advanced brain analyzer
    py::class_<BrainAnalyzer>(m, "BrainAnalyzer", R"pbdoc(Brain analyzer for detailed NLM brain analysis)pbdoc")
        .def(py::init<>())
        .def("analyzeConnectivity", &BrainAnalyzer::analyzeConnectivity)
        .def("analyzePlasticity", &BrainAnalyzer::analyzePlasticity)
        .def("analyzeExcitatoryInhibition", &BrainAnalyzer::analyzeExcitatoryInhibition)
        .def("analyzeSpikePatterns", &BrainAnalyzer::analyzeSpikePatterns)
        .def("getConnectionDensity", &BrainAnalyzer::getConnectionDensity)
        .def("getPlasticityMetrics", &BrainAnalyzer::getPlasticityMetrics)
        .def("getNetworkEfficiency", &BrainAnalyzer::getNetworkEfficiency)
        .def("getStabilityMetrics", &BrainAnalyzer::getStabilityMetrics)
        .def("generateReport", &BrainAnalyzer::generateReport)
        .def("generateJSONReport", &BrainAnalyzer::generateJSONReport);

    // Advanced configuration presets utility
    py::class_<ConfigPresets>(m, "ConfigPresets", R"pbdoc(Configuration presets utility for NLM)pbdoc")
        .def(py::init<>())
        .def("getPresets", &ConfigPresets::getPresets)
        .def("getPreset", &ConfigPresets::getPreset)
        .def("addPreset", &ConfigPresets::addPreset)
        .def("removePreset", &ConfigPresets::removePreset)
        .def("applyPreset", &ConfigPresets::applyPreset)
        .def("importPresets", &ConfigPresets::importPresets)
        .def("exportPresets", &ConfigPresets::exportPresets)
        .def("createFromConfig", &ConfigPresets::createFromConfig);

    // Advanced debug and inspection utilities
    py::class_<DebugTools>(m, "DebugTools", R"pbdoc(Debug and inspection tools for NLM)pbdoc")
        .def(py::init<>())
        .def("watchSpikeCount", &DebugTools::watchSpikeCount)
        .def("watchFiringNeurons", &DebugTools::watchFiringNeurons)
        .def("watchSynapticWeights", &DebugTools::watchSynapticWeights)
        .def("watchDevelopmentStage", &DebugTools::watchDevelopmentStage)
        .def("watchNeuromodulation", &DebugTools::watchNeuromodulation)
        .def("getNeuronInfo", &DebugTools::getNeuronInfo)
        .def("getSynapseInfo", &DebugTools::getSynapseInfo)
        .def("dumpBrainState", &DebugTools::dumpBrainState)
        .def("generateDebugReport", &DebugTools::generateDebugReport);

    // Basic visualization utilities
    py::class_<VisualizationUtils>(m, "VisualizationUtils", R"pbdoc(Basic visualization utilities for NLM)pbdoc")
        .def(py::init<>())
        .def("plotSpikeRaster", &VisualizationUtils::plotSpikeRaster)
        .def("plotActivityOverTime", &VisualizationUtils::plotActivityOverTime)
        .def("plotConnectionMatrix", &VisualizationUtils::plotConnectionMatrix)
        .def("plotWeightDistribution", &VisualizationUtils::plotWeightDistribution)
        .def("saveVisualization", &VisualizationUtils::saveVisualization)
        .def("createNetworkGraph", &VisualizationUtils::createNetworkGraph);

    // Advanced experiment management utilities
    py::class_<ExperimentManager>(m, "ExperimentManager", R"pbdoc(Experiment management utilities for NLM)pbdoc")
        .def(py::init<>())
        .def("createExperiment", &ExperimentManager::createExperiment)
        .def("runExperiment", &ExperimentManager::runExperiment)
        .def("compareExperiments", &ExperimentManager::compareExperiments)
        .def("getExperimentResults", &ExperimentManager::getExperimentResults)
        .def("saveExperimentResults", &ExperimentManager::saveExperimentResults)
        .def("importResults", &ExperimentManager::importResults)
        .def("exportResults", &ExperimentManager::exportResults);

    // Module-level advanced utility functions
    m.def("createPerformanceMonitor", []() -> std::shared_ptr<PerformanceMonitor> {
        return std::make_shared<PerformanceMonitor>();
    }, "Create a performance monitor");

    m.def("createSimulationController", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<SimulationController> {
        return std::make_shared<SimulationController>(brain);
    }, py::arg("brain"), "Create a simulation controller for a brain");

    m.def("createBrainAnalyzer", []() -> std::shared_ptr<BrainAnalyzer> {
        return std::make_shared<BrainAnalyzer>();
    }, "Create a brain analyzer");

    m.def("createExperimentManager", []() -> std::shared_ptr<ExperimentManager> {
        return std::make_shared<ExperimentManager>();
    }, "Create an experiment manager");

    m.def("createDebugTools", []() -> std::shared_ptr<DebugTools> {
        return std::make_shared<DebugTools>();
    }, "Create debug tools");

    m.def("createVisualizationUtils", []() -> std::shared_ptr<VisualizationUtils> {
        return std::make_shared<VisualizationUtils>();
    }, "Create visualization utilities");

    m.def("createConfigPresets", []() -> std::shared_ptr<ConfigPresets> {
        return std::make_shared<ConfigPresets>();
    }, "Create configuration presets utility");

    m.def("runQuickBenchmark", [](std::shared_ptr<Brain> brain, size_t steps) -> BenchmarkResult {
        return runQuickBenchmark(brain, steps);
    }, py::arg("brain"), py::arg("steps"), "Run a quick benchmark on a brain");

    m.def("compareBrains", [](const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<size_t>& steps) -> std::vector<BenchmarkResult> {
        return compareBrains(brains, steps);
    }, py::arg("brains"), py::arg("steps"), "Compare multiple brains");

    m.def("exportBrainToJSON", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        exportBrainToJSON(brain, filepath);
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to JSON file");

    m.def("importBrainFromJSON", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        return importBrainFromJSON(filepath);
    }, py::arg("filepath"), "Import brain state from JSON file");

    m.def("createReplayConfiguration", [](std::shared_ptr<Brain> brain, size_t replaySteps) -> std::shared_ptr<Config> {
        return createReplayConfiguration(brain, replaySteps);
    }, py::arg("brain"), py::arg("replaySteps"), "Create a configuration for replay simulation");

    m.def("generateLearningReport", [](std::shared_ptr<Brain> brain, size_t steps) -> std::string {
        return generateLearningReport(brain, steps);
    }, py::arg("brain"), py::arg("steps"), "Generate a learning report for a brain");

    m.def("createScenario", [](const std::string& name, const std::string& description, std::function<void(std::shared_ptr<Brain>, std::shared_ptr<Config>)> setupFunc) {
        return createScenario(name, description, setupFunc);
    }, py::arg("name"), py::arg("description"), py::arg("setupFunc"), "Create a custom simulation scenario");

    m.def("getAdvancedPresets", []() -> std::vector<std::string> {
        return getAdvancedPresets();
    }, "Get available advanced configuration presets");

    m.def("getDefaultAdvancedConfig", []() -> std::shared_ptr<Config> {
        return getDefaultAdvancedConfig();
    }, "Get default advanced configuration");

    m.def("validateConfiguration", [](std::shared_ptr<Config> config) -> bool {
        return validateConfiguration(config);
    }, py::arg("config"), "Validate a configuration");

    m.def("profileSimulation", [](std::shared_ptr<Brain> brain, size_t steps, std::shared_ptr<PerformanceMonitor> monitor) -> ProfileResult {
        return profileSimulation(brain, steps, monitor);
    }, py::arg("brain"), py::arg("steps"), py::arg("monitor") = nullptr, "Profile a simulation run");

    m.def("analyzeLearningCurve", [](std::shared_ptr<Brain> brain, size_t steps) -> LearningCurveAnalysis {
        return analyzeLearningCurve(brain, steps);
    }, py::arg("brain"), py::arg("steps"), "Analyze learning curve of a brain");

    m.def("createNeuralNetworkVisualizer", []() -> std::shared_ptr<NeuralNetworkVisualizer> {
        return std::make_shared<NeuralNetworkVisualizer>();
    }, "Create a neural network visualizer");

    m.def("createExperimentRunner", []() -> std::shared_ptr<ExperimentRunner> {
        return std::make_shared<ExperimentRunner>();
    }, "Create an experiment runner");

    m.def("createHyperparameterOptimizer", []() -> std::shared_ptr<HyperparameterOptimizer> {
        return std::make_shared<HyperparameterOptimizer>();
    }, "Create a hyperparameter optimizer");

    m.def("createTransferLearningUtils", []() -> std::shared_ptr<TransferLearningUtils> {
        return std::make_shared<TransferLearningUtils>();
    }, "Create transfer learning utilities");

    m.def("createAdvancedDebugTools", []() -> std::shared_ptr<DebugTools> {
        return std::make_shared<DebugTools>();
    }, "Create advanced debug tools with comprehensive inspection capabilities");

    m.def("createBrainInspector", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<BrainInspector> {
        return std::make_shared<BrainInspector>(brain);
    }, py::arg("brain"), "Create a brain inspector for detailed brain analysis");

    m.def("createMemoryProfiler", []() -> std::shared_ptr<MemoryProfiler> {
        return std::make_shared<MemoryProfiler>();
    }, "Create a memory profiler for detailed memory analysis");

    m.def("createNetworkAnalyzer", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NetworkAnalyzer> {
        return std::make_shared<NetworkAnalyzer>(brain);
    }, py::arg("brain"), "Create a network analyzer for brain connectivity analysis");

    m.def("createPerformanceDebugger", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PerformanceDebugger> {
        return std::make_shared<PerformanceDebugger>(brain);
    }, py::arg("brain"), "Create a performance debugger for step-by-step performance analysis");

    m.def("createConfigDebugger", [](std::shared_ptr<Config> config) -> std::shared_ptr<ConfigDebugger> {
        return std::make_shared<ConfigDebugger>(config);
    }, py::arg("config"), "Create a configuration debugger for configuration analysis");

    m.def("createSynchronizationDebugger", []() -> std::shared_ptr<SynchronizationDebugger> {
        return std::make_shared<SynchronizationDebugger>();
    }, "Create a synchronization debugger for thread safety analysis");

    m.def("createErrorAnalyzer", []() -> std::shared_ptr<ErrorAnalyzer> {
        return std::make_shared<ErrorAnalyzer>();
    }, "Create an error analyzer for detailed error reporting and analysis");

    m.def("createStepByStepDebugger", [](std::shared_ptr<Brain> brain, size_t maxSteps) -> std::shared_ptr<StepByStepDebugger> {
        return std::make_shared<StepByStepDebugger>(brain, maxSteps);
    }, py::arg("brain"), py::arg("maxSteps"), "Create a step-by-step debugger for detailed simulation inspection");

    m.def("analyzeBrainState", [](std::shared_ptr<Brain> brain) -> BrainStateAnalysis {
        return analyzeBrainState(brain);
    }, py::arg("brain"), "Analyze and return comprehensive brain state information");

    m.def("validateBrainIntegrity", [](std::shared_ptr<Brain> brain) -> bool {
        return validateBrainIntegrity(brain);
    }, py::arg("brain"), "Validate brain integrity and consistency");

    m.def("findBrainIssues", [](std::shared_ptr<Brain> brain) -> std::vector<std::string> {
        return findBrainIssues(brain);
    }, py::arg("brain"), "Find and return list of brain issues and potential problems");

    m.def("getBrainPerformanceProfile", [](std::shared_ptr<Brain> brain, size_t steps) -> PerformanceProfile {
        return getBrainPerformanceProfile(brain, steps);
    }, py::arg("brain"), py::arg("steps"), "Get comprehensive performance profile for a brain");

    m.def("compareBrainsState", [](const std::vector<std::shared_ptr<Brain>>& brains) -> BrainComparison {
        return compareBrainsState(brains);
    }, py::arg("brains"), "Compare multiple brains and return comparison results");

    m.def("analyzeLearningProgress", [](std::shared_ptr<Brain> brain, size_t steps) -> LearningAnalysis {
        return analyzeLearningProgress(brain, steps);
    }, py::arg("brain"), py::arg("steps"), "Analyze learning progress and patterns");

    m.def("createDevelopmentDebugger", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<DevelopmentDebugger> {
        return std::make_shared<DevelopmentDebugger>(brain);
    }, py::arg("brain"), "Create a development debugger for developmental stage analysis");

    m.def("createNeuromodulationDebugger", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuromodulationDebugger> {
        return std::make_shared<NeuromodulationDebugger>(brain);
    }, py::arg("brain"), "Create a neuromodulation debugger for neuromodulator analysis");

    m.def("createMemoryDebugger", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<MemoryDebugger> {
        return std::make_shared<MemoryDebugger>(brain);
    }, py::arg("brain"), "Create a memory debugger for memory system analysis");

    m.def("createPlasticityDebugger", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PlasticityDebugger> {
        return std::make_shared<PlasticityDebugger>(brain);
    }, py::arg("brain"), "Create a plasticity debugger for plasticity rule analysis");

    m.def("exportDebugInfo", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        exportDebugInfo(brain, filepath);
    }, py::arg("brain"), py::arg("filepath"), "Export comprehensive debug information to file");

    m.def("importDebugInfo", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        return importDebugInfo(filepath);
    }, py::arg("filepath"), "Import brain state from debug information file");

    m.def("createDebugSession", [](const std::string& sessionName) -> std::shared_ptr<DebugSession> {
        return std::make_shared<DebugSession>(sessionName);
    }, py::arg("sessionName"), "Create a debug session for recording and analysis");

    m.def("createBenchmarkDebugger", [](std::shared_ptr<Brain> brain, const std::vector<size_t>& testConfigurations) -> std::shared_ptr<BenchmarkDebugger> {
        return std::make_shared<BenchmarkDebugger>(brain, testConfigurations);
    }, py::arg("brain"), py::arg("testConfigurations"), "Create a benchmark debugger for performance testing");

    m.def("analyzeDebugData", [](const std::string& debugData) -> DebugAnalysis {
        return analyzeDebugData(debugData);
    }, py::arg("debugData"), "Analyze debug data and return comprehensive analysis");

    m.def("createDebugReporter", []() -> std::shared_ptr<DebugReporter> {
        return std::make_shared<DebugReporter>();
    }, "Create a debug reporter for comprehensive reporting");

    m.def("createLiveDebugger", [](std::shared_ptr<Brain> brain, double updateInterval) -> std::shared_ptr<LiveDebugger> {
        return std::make_shared<LiveDebugger>(brain, updateInterval);
    }, py::arg("brain"), py::arg("updateInterval"), "Create a live debugger for real-time debugging");

    m.def("createArchiveDebugger", []() -> std::shared_ptr<ArchiveDebugger> {
        return std::make_shared<ArchiveDebugger>();
    }, "Create an archive debugger for historical analysis");

    m.def("createProfileDebugger", [](std::shared_ptr<Brain> brain, const std::string& profileName) -> std::shared_ptr<ProfileDebugger> {
        return std::make_shared<ProfileDebugger>(brain, profileName);
    }, py::arg("brain"), py::arg("profileName"), "Create a profile debugger for performance profiling");
