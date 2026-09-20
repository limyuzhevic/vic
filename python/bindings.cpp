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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("enable_all_systems", &AgentBrain::enableAllSystems,
             py::arg("enable"),
             "Enable/disable all neuromodulation systems at once (convenience method)");

    // Context manager for automatic cleanup
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "BrainContext", R"pbdoc(Context manager for automatic brain cleanup and resource management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", &AgentBrain::getBrain,
             "Enter context manager - returns the underlying brain")
        .def("__exit__", [](std::shared_ptr<Brain> brain, const py::object&, const py::object&, const py::object&) {
             brain->reset();
             return false;
        }, "Exit context manager - resets brain state")
        .def("reset", &AgentBrain::reset,
             "Reset brain state (can be called inside context manager)")
        .def("getStats", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get brain statistics (convenience method)");

    // Convenience factory functions for common configurations
    m.def("createPhase6DemoConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.15f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, "Create a default Phase 6 demo configuration")
    .def("createPhase4Config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
        return config;
    }, "Create a Phase 4 demo configuration (cognition focused)")
    .def("createPhase3Config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.05f);
        config->set("plasticity.stdp.enable", true);
        return config;
    }, "Create a Phase 3 demo configuration (basic learning)")
    .def("createMinimalConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        return config;
    }, "Create a minimal configuration for quick testing")
    .def("createDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 3000);
        config->set("brain.synapse_density", 0.12f);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.system.enable", true);
        return config;
    }, "Create a development-focused configuration")
    .def("createHighPerformanceConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 10000);
        config->set("brain.synapse_density", 0.2f);
        config->set("performance.optimization", true);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.0005f);
        return config;
    }, "Create a high-performance configuration for large networks")
    .def("createEducationalConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.08f);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("visualization.enable", true);
        return config;
    }, "Create an educational configuration for learning and demonstration");

    // Performance monitoring utilities
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring and profiling utilities)pbdoc")
        .def(py::init<>())
        .def("start", &PerformanceMonitor::start,
             "Start performance monitoring")
        .def("stop", &PerformanceMonitor::stop,
             "Stop performance monitoring")
        .def("is_running", &PerformanceMonitor::isRunning,
             "Check if monitoring is active")
        .def("get_neuron_count", &PerformanceMonitor::getNeuronCount,
             "Get current neuron count")
        .def("get_firing_rate", &PerformanceMonitor::getFiringRate,
             "Get current average firing rate")
        .def("get_simulation_time", &PerformanceMonitor::getSimulationTime,
             "Get total simulation time")
        .def("get_cpu_usage", &PerformanceMonitor::getCPUUsage,
             "Get current CPU usage (percentage)")
        .def("get_memory_usage", &PerformanceMonitor::getMemoryUsage,
             "Get current memory usage (MB)")
        .def("get_metrics", &PerformanceMonitor::getMetrics,
             "Get all performance metrics as a dictionary")
        .def("reset", &PerformanceMonitor::reset,
             "Reset performance statistics")
        .def("enable_memory_profiling", &PerformanceMonitor::enableMemoryProfiling,
             py::arg("enable"),
             "Enable/disable memory profiling")
        .def("enable_cpu_profiling", &PerformanceMonitor::enableCPUMonitoring,
             py::arg("enable"),
             "Enable/disable CPU monitoring")
        .def("export_metrics", &PerformanceMonitor::exportMetrics,
             py::arg("filename"),
             "Export metrics to JSON file");

    // Visualization integration helpers
    py::class_<VisualizationHelper>(m, "VisualizationHelper", R"pbdoc(Helpers for integrating NLM with visualization tools)pbdoc")
        .def(py::init<>())
        .def("create_network_visualization", &VisualizationHelper::createNetworkVisualization,
             py::arg("brain"), py::arg("output_file"),
             "Create network visualization from brain state")
        .def("create_activity_animation", &VisualizationHelper::createActivityAnimation,
             py::arg("brain"), py::arg("output_file"), py::arg("duration"),
             "Create activity animation from brain simulation")
        .def("export_for_external_viewer", &VisualizationHelper::exportForExternalViewer,
             py::arg("brain"), py::arg("format"),
             "Export brain state for external visualization tools")
        .def("enable_web_visualization", &VisualizationHelper::enableWebVisualization,
             py::arg("port"), py::arg("host"),
             "Enable web-based visualization server")
        .def("get_visualization_ports", &VisualizationHelper::getAvailablePorts,
             "Get available visualization server ports")
        .def("close_visualization", &VisualizationHelper::closeVisualization,
             "Close all visualization connections");

    // Educational utilities
    py::class_<EducationalUtils>(m, "EducationalUtils", R"pbdoc(Educational utilities for learning NLM concepts)pbdoc")
        .def(py::init<>())
        .def("create_learning_environment", &EducationalUtils::createLearningEnvironment,
             py::arg("difficulty"), py::arg("focus_area"),
             "Create a learning environment with specific difficulty and focus")
        .def("get_exercise", &EducationalUtils::getExercise,
             py::arg("topic"), py::arg("step"),
             "Get educational exercise for specific NLM topic")
        .def("check_answer", &EducationalUtils::checkAnswer,
             py::arg("exercise_id"), py::arg("user_answer"),
             "Check user's answer to educational exercise")
        .def("get_concept_explanation", &EducationalUtils::getConceptExplanation,
             py::arg("concept"),
             "Get explanation of NLM concept for educational purposes")
        .def("create_exercise", &EducationalUtils::createExercise,
             py::arg("type"), py::arg("parameters"),
             "Create custom educational exercise")
        .def("export_simulation", &EducationalUtils::exportSimulation,
             py::arg("brain"), py::arg("output_file"),
             "Export simulation data in educational format")
        .def("create_concept_video", &EducationalUtils::createConceptVideo,
             py::arg("concept"), py::arg("output_file"),
             "Create educational video explaining NLM concept");

    // Convenience simulation runner
    m.def("run_phase6_demo", &runPhase6Demo, py::arg("num_steps"), py::arg("config"),
          "Run Phase 6 integration demo with default settings")
    .def("run_phase4_demo", &runPhase4Demo, py::arg("num_steps"), py::arg("config"),
          "Run Phase 4 demo (cognition focused)")
    .def("run_phase3_demo", &runPhase3Demo, py::arg("num_steps"), py::arg("config"),
          "Run Phase 3 demo (basic learning)")
    .def("run_custom_simulation", &runCustomSimulation, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          "Run a custom simulation with provided brain, world, and agent components")
    .def("run_learning_session", &runLearningSession, py::arg("config_name"), py::arg("duration"),
          "Run a learning session with predefined configuration")
    .def("run_performance_benchmark", &runPerformanceBenchmark, py::arg("neuron_count"), py::arg("duration"),
          "Run performance benchmark with specified neuron count");

    // Utility functions
    m.def("benchmark_memory_usage", &benchmarkMemoryUsage,
          py::arg("brain"), py::arg("steps"),
          "Benchmark memory usage over simulation steps")
    .def("benchmark_firing_patterns", &benchmarkFiringPatterns,
          py::arg("brain"), py::arg("steps"),
          "Benchmark firing patterns and statistics")
    .def("get_available_phase_configs", &getAvailablePhaseConfigs,
          "Get list of available Phase configuration presets")
    .def("validate_config", &validateConfig, py::arg("config"),
          "Validate configuration and return list of issues (empty if valid)")
    .def("get_system_info", &getSystemInfo,
          "Get system information including version and capabilities")
    .def("create_reproducible_session", &createReproducibleSession,
          py::arg("seed"), py::arg("duration"),
          "Create a reproducible simulation session with seed")
    .def("restore_session", &restoreSession, py::arg("checkpoint_file"),
          "Restore a simulation session from checkpoint file");

    // Legacy compatibility functions (deprecated but still available)
    m.def("createBrain", []() {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Legacy function - creates brain with default config")
    .def("createWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Legacy function - creates simple world")
    .def("createAgent", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Legacy function - creates agent brain interface");

    // Convenience simulation runner implementations
    m.def("runPhase6Demo", [](int num_steps) {
        // Simplified version - in production, would use full Phase6Config
        auto config = createPhase6DemoConfig();
        Phase6IntegratedExperiment experiment;
        return experiment.run(num_steps);
    }, py::arg("num_steps") = 1000,
          "Run Phase 6 integration demo with default settings")
    .def("runPhase6DemoWithConfig", [](int num_steps, std::shared_ptr<Config> config) {
        Phase6IntegratedExperiment experiment;
        return experiment.run(config);
    }, py::arg("num_steps") = 1000, py::arg("config"),
          "Run Phase 6 demo with custom configuration")
    .def("runPhase4Demo", [](int num_steps) {
        Phase4Experiment experiment;
        auto config = createPhase4Config();
        return experiment.run(num_steps, config);
    }, py::arg("num_steps") = 500,
          "Run Phase 4 demo (cognition focused)")
    .def("runPhase3Demo", [](int num_steps) {
        Phase3Experiment experiment;
        auto config = createPhase3Config();
        return experiment.run(num_steps, config);
    }, py::arg("num_steps") = 200,
          "Run Phase 3 demo (basic learning)")
    .def("runCustomSimulation", &runCustomSimulation, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          "Run a custom simulation with provided brain, world, and agent components")
    .def("runLearningSession", &runLearningSession, py::arg("config_name"), py::arg("duration"),
          "Run a learning session with predefined configuration")
    .def("runPerformanceBenchmark", &runPerformanceBenchmark, py::arg("neuron_count"), py::arg("duration"),
          "Run performance benchmark with specified neuron count");

    // Utility function implementations
    m.def("benchmarkMemoryUsage", &benchmarkMemoryUsage,
          py::arg("brain"), py::arg("steps"),
          "Benchmark memory usage over simulation steps")
    .def("benchmarkFiringPatterns", &benchmarkFiringPatterns,
          py::arg("brain"), py::arg("steps"),
          "Benchmark firing patterns and statistics")
    .def("getAvailablePhaseConfigs", &getAvailablePhaseConfigs,
          "Get list of available Phase configuration presets")
    .def("validateConfig", &validateConfig, py::arg("config"),
          "Validate configuration and return list of issues (empty if valid)")
    .def("getSystemInfo", &getSystemInfo,
          "Get system information including version and capabilities")
    .def("createReproducibleSession", &createReproducibleSession,
          py::arg("seed"), py::arg("duration"),
          "Create a reproducible simulation session with seed")
    .def("restoreSession", &restoreSession, py::arg("checkpoint_file"),
          "Restore a simulation session from checkpoint file");

    // Statistical analysis helpers
    m.def("compute_firing_statistics", [](std::shared_ptr<Brain> brain, int steps) {
        std::vector<float> firing_rates;
        for (int i = 0; i < steps; ++i) {
            firing_rates.push_back(brain->getAverageFiringRate());
            brain->step(i);
        }
        float mean_rate = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0f) / firing_rates.size();
        float variance = 0.0f;
        for (float rate : firing_rates) {
            variance += (rate - mean_rate) * (rate - mean_rate);
        }
        variance /= firing_rates.size();
        float stddev = std::sqrt(variance);
        
        return std::make_tuple(mean_rate, stddev, *std::max_element(firing_rates.begin(), firing_rates.end()),
                              *std::min_element(firing_rates.begin(), firing_rates.end()));
    }, py::arg("brain"), py::arg("steps"),
          "Compute firing rate statistics (mean, stddev, max, min)")
    .def("analyze_spike_patterns", [](std::shared_ptr<Brain> brain, int steps) {
        std::vector<size_t> spike_counts;
        for (int i = 0; i < steps; ++i) {
            spike_counts.push_back(brain->getTotalSpikeCount());
            brain->step(i);
        }
        float total_spikes = std::accumulate(spike_counts.begin(), spike_counts.end(), 0.0f);
        float avg_spikes = total_spikes / spike_counts.size();
        
        return std::make_tuple(total_spikes, avg_spikes, spike_counts.back());
    }, py::arg("brain"), py::arg("steps"),
          "Analyze spike patterns (total, average, final count)")
    .def("simulate_efficiency", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double simulated_time = steps * 0.001; // Assuming 1ms per step
        double real_time = duration.count() / 1000.0; // Convert to seconds
        double efficiency = simulated_time / real_time;
        
        return std::make_tuple(simulated_time, real_time, efficiency);
    }, py::arg("brain"), py::arg("steps"),
          "Simulate efficiency metrics (simulated time, real time, efficiency factor)")
    .def("export_brain_state", &exportBrainState,
          py::arg("brain"), py::arg("filename"),
          "Export brain state to file for later analysis")
    .def("import_brain_state", &importBrainState,
          py::arg("filename"), py::arg("config"),
          "Import brain state from file and create brain instance");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
