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
#include "../src/core/Errors/Errors.hpp"
#include "../src/core/PluginSystem/PluginSystem.hpp"
#include "../src/core/Performance/PerformanceProfiler.hpp"
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

    // Register exceptions for Python
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigException>(m, "ConfigException");
    py::register_exception<ConfigLoadException>(m, "ConfigLoadException");
    py::register_exception<ConfigValidationException>(m, "ConfigValidationException");
    py::register_exception<BrainException>(m, "BrainException");
    py::register_exception<BrainInitializationException>(m, "BrainInitializationException");
    py::register_exception<SimulationStepException>(m, "SimulationStepException");

    py::class_<ErrorCode>(m, "ErrorCode")
        .value("Success", ErrorCode::Success)
        .value("FileNotFound", ErrorCode::FileNotFound)
        .value("InvalidFormat", ErrorCode::InvalidFormat)
        .value("OutOfMemory", ErrorCode::OutOfMemory)
        .value("InvalidParameter", ErrorCode::InvalidParameter)
        .value("InitializationFailed", ErrorCode::InitializationFailed)
        .value("SimulationError", ErrorCode::SimulationError)
        .value("ConfigError", ErrorCode::ConfigError)
        .value("PluginError", ErrorCode::PluginError)
        .value("UnknownError", ErrorCode::UnknownError)
        .export_values();

    py::class_<Result>(m, "Result")
        .def(py::init<>())
        .def("success", &Result::success)
        .def("errorCode", &Result::errorCode)
        .def("message", &Result::message)
        .def("throwIfError", &Result::throwIfError)
        .def("__repr__", [](const Result& r) {
            return "<Result: success=" + std::to_string(r.success()) + ", error_code=" +
                std::to_string(static_cast<int>(r.errorCode())) + ", message=" + r.message() + ">";
        });

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

    // Performance profiler bindings
    py::enum_<PerformanceMetric>(m, "PerformanceMetric", R"pbdoc(Performance metric enumeration)pbdoc")
        .value("Time", PerformanceMetric::Time)
        .value("Memory", PerformanceMetric::Memory)
        .value("CPU", PerformanceMetric::CPU)
        .value("IOWait", PerformanceMetric::IOWait)
        .value("Network", PerformanceMetric::Network)
        .value("Custom", PerformanceMetric::Custom)
        .export_values();

    py::class_<PerformanceSample>(m, "PerformanceSample")
        .def(py::init<>())
        .def_readwrite("metric", &PerformanceSample::metric)
        .def_readwrite("value", &PerformanceSample::value)
        .def_readwrite("description", &PerformanceSample::description)
        .def_readwrite("tags", &PerformanceSample::tags);

    py::class_<PerformanceCounter>(m, "PerformanceCounter")
        .def(py::init<const std::string&, PerformanceMetric>(), py::arg("name"), py::arg("type") = PerformanceMetric::Time)
        .def("getName", &PerformanceCounter::getName)
        .def("getType", &PerformanceCounter::getType)
        .def("get", &PerformanceCounter::get)
        .def("update", &PerformanceCounter::update, py::arg("value"))
        .def("increment", &PerformanceCounter::increment)
        .def("add", &PerformanceCounter::add, py::arg("value"))
        .def("getMin", &PerformanceCounter::getMin)
        .def("getMax", &PerformanceCounter::getMax)
        .def("getAverage", &PerformanceCounter::getAverage)
        .def("getCount", &PerformanceCounter::getCount)
        .def("getElapsedTime", &PerformanceCounter::getElapsedTime)
        .def("setMetadata", &PerformanceCounter::setMetadata, py::arg("key"), py::arg("value"))
        .def("getMetadata", &PerformanceCounter::getMetadata, py::arg("key"));

    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<>(), py::arg("name") = "default_session")
        .def("start", &PerformanceProfiler::start)
        .def("stop", &PerformanceProfiler::stop)
        .def("isRecording", &PerformanceProfiler::isRecording)
        .def("recordSample", &PerformanceProfiler::recordSample, py::arg("metric"), py::arg("value"), py::arg("description") = "")
        .def("updateCounter", &PerformanceProfiler::updateCounter, py::arg("name"), py::arg("value"))
        .def("incrementCounter", &PerformanceProfiler::incrementCounter, py::arg("name"))
        .def("getCounter", &PerformanceProfiler::getCounter, py::arg("name"), py::return_value_policy::reference)
        .def("getAllCounters", &PerformanceProfiler::getAllCounters)
        .def("getSamplesByMetric", &PerformanceProfiler::getSamplesByMetric, py::arg("metric"))
        .def("getSessionDuration", &PerformanceProfiler::getSessionDuration)
        .def("getSamplesCount", &PerformanceProfiler::getSamplesCount)
        .def("clear", &PerformanceProfiler::clear)
        .def("exportSamplesToJSON", &PerformanceProfiler::exportSamplesToJSON)
        .def("exportSamplesToCSV", &PerformanceProfiler::exportSamplesToCSV)
        .def("exportCountersToCSV", &PerformanceProfiler::exportCountersToCSV)
        .def("importSamplesFromJSON", &PerformanceProfiler::importSamplesFromJSON, py::arg("jsonData"));

    py::class_<MemoryProfiler>(m, "MemoryProfiler")
        .def(py::init<>())
        .def("start", &MemoryProfiler::start)
        .def("stop", &MemoryProfiler::stop)
        .def("recordUsage", &MemoryProfiler::recordUsage)
        .def("getCurrentUsage", &MemoryProfiler::getCurrentUsage)
        .def("getMemoryUsageTrend", &MemoryProfiler::getMemoryUsageTrend)
        .def("getMaxUsage", &MemoryProfiler::getMaxUsage)
        .def("getMinUsage", &MemoryProfiler::getMinUsage)
        .def("exportToCSV", &MemoryProfiler::exportToCSV);

    py::class_<CPUMProfiler>(m, "CPUMProfiler")
        .def(py::init<>())
        .def("start", &CPUMProfiler::start)
        .def("stop", &CPUMProfiler::stop)
        .def("recordSample", &CPUMProfiler::recordSample)
        .def("getCurrentCPUUsage", &CPUMProfiler::getCurrentCPUUsage)
        .def("getAverageCPUUsage", &CPUMProfiler::getAverageCPUUsage)
        .def("exportToCSV", &CPUMProfiler::exportToCSV);

    // Convenience functions for performance profiling
    m.def("startPerformanceProfiling", &startPerformanceProfiling, py::arg("sessionName") = "default", "Start performance profiling");
    m.def("stopPerformanceProfiling", &stopPerformanceProfiling, "Stop performance profiling");
    m.def("recordPerformanceSample", &recordPerformanceSample, py::arg("metric"), py::arg("value"), py::arg("description") = "", "Record a performance sample");
    m.def("updatePerformanceCounter", &updatePerformanceCounter, py::arg("name"), py::arg("value"), "Update a performance counter");
    m.def("recordMemoryUsage", &recordMemoryUsage, "Record current memory usage");
    m.def("recordCPUSample", &recordCPUSample, "Record CPU usage sample");
    m.def("exportPerformanceData", &exportPerformanceData, "Export all performance data to string");
    m.def("exportPerformanceCountersCSV", &exportPerformanceCountersCSV, "Export performance counters to CSV format");

    // Get global profiler instances
    m.def("getPerformanceProfiler", []() -> PerformanceProfiler& {
        return getPerformanceProfiler();
    }, "Get the global performance profiler");
    
    m.def("getMemoryProfiler", []() -> MemoryProfiler& {
        return getMemoryProfiler();
    }, "Get the global memory profiler");
    
    m.def("getCPUMProfiler", []() -> CPUMProfiler& {
        return getCPUMProfiler();
    }, "Get the global CPU profiler");

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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced error handling)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("initializeSafe", &Brain::initializeSafe,
             "Initialize the brain with configuration (returns Result on error)")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepSafe", &Brain::stepSafe, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp (returns Result on error)")
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
        .def("saveSafe", &Brain::saveSafe, py::arg("filepath"),
             "Save brain state to file (returns Result on error)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("loadSafe", &Brain::loadSafe, py::arg("filepath"),
             "Load brain state from file (returns Result on error)")
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

    // Plugin system bindings
    py::class_<IPlugin>(m, "IPlugin")
        .def("getName", &IPlugin::getName)
        .def("getVersion", &IPlugin::getVersion)
        .def("getDescription", &IPlugin::getDescription)
        .def("getAuthor", &IPlugin::getAuthor)
        .def("initialize", &IPlugin::initialize, py::arg("config"))
        .def("shutdown", &IPlugin::shutdown)
        .def("preStep", &IPlugin::preStep, py::arg("dt"))
        .def("postStep", &IPlugin::postStep, py::arg("dt"));

    py::enum_<PluginType>(m, "PluginType", R"pbdoc(Plugin type enumeration)pbdoc")
        .value("Plasticity", PluginType::Plasticity)
        .value("Neuromodulator", PluginType::Neuromodulator)
        .value("Development", PluginType::Development)
        .value("Memory", PluginType::Memory)
        .value("Cognition", PluginType::Cognition)
        .value("Sensor", PluginType::Sensor)
        .value("Motor", PluginType::Motor)
        .value("Environment", PluginType::Environment)
        .export_values();

    py::class_<PluginManager>(m, "PluginManager")
        .def(py::init<>())
        .def("registerPlugin", &PluginManager::registerPlugin)
        .def("unregisterPlugin", &PluginManager::unregisterPlugin)
        .def("getPlugin", &PluginManager::getPlugin)
        .def("getPluginsByType", &PluginManager::getPluginsByType)
        .def("initializeAll", &PluginManager::initializeAll)
        .def("shutdownAll", &PluginManager::shutdownAll)
        .def("updateAll", &PluginManager::updateAll, py::arg("dt"))
        .def("getPluginNames", &PluginManager::getPluginNames)
        .def("getFactoryTypeNames", &PluginManager::getFactoryTypeNames);

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

    // Performance profiler convenience functions
    m.def("startPerformanceProfiling", &startPerformanceProfiling, py::arg("sessionName") = "default", "Start performance profiling");
    m.def("stopPerformanceProfiling", &stopPerformanceProfiling, "Stop performance profiling");
    m.def("recordPerformanceSample", &recordPerformanceSample, py::arg("metric"), py::arg("value"), py::arg("description") = "", "Record a performance sample");
    m.def("updatePerformanceCounter", &updatePerformanceCounter, py::arg("name"), py::arg("value"), "Update a performance counter");
    m.def("recordMemoryUsage", &recordMemoryUsage, "Record current memory usage");
    m.def("recordCPUSample", &recordCPUSample, "Record CPU usage sample");
    m.def("exportPerformanceData", &exportPerformanceData, "Export all performance data to string");
    m.def("exportPerformanceCountersCSV", &exportPerformanceCountersCSV, "Export performance counters to CSV format");

    // Get global profiler instances
    m.def("getPerformanceProfiler", []() -> PerformanceProfiler& {
        return getPerformanceProfiler();
    }, "Get the global performance profiler");
    
    m.def("getMemoryProfiler", []() -> MemoryProfiler& {
        return getMemoryProfiler();
    }, "Get the global memory profiler");
    
    m.def("getCPUMProfiler", []() -> CPUMProfiler& {
        return getCPUMProfiler();
    }, "Get the global CPU profiler");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
