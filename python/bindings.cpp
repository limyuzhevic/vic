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
             "Load configuration from a JSON file. Raises RuntimeError on failure.")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            bool success = self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
            if (!success) {
                throw py::value_error("Failed to load configuration from arguments");
            }
            return success;
        }, py::arg("args"),
           "Load configuration from command line arguments. Raises RuntimeError on failure.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError on failure.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get, py::arg("key"), py::arg("default") = std::string(),
             "Get a configuration value by key. Returns default if key not found.")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value. Validates key and value types.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &Config::validate,
             "Validate configuration. Returns validation errors if any.")
        .def("getBrainConfig", []() {
            std::shared_ptr<Config> brainConfig = std::make_shared<Config>();
            // Set default brain configuration
            brainConfig->set("brain.neuron_count", 1000);
            brainConfig->set("brain.synapse_density", 0.1f);
            brainConfig->set("brain.v_thresh", -50.0f);
            brainConfig->set("brain.v_rest", -70.0f);
            brainConfig->set("brain.v_reset", -75.0f);
            return brainConfig;
        }, "Create a default brain configuration")
        .def("getWorldConfig", []() {
            std::shared_ptr<Config> worldConfig = std::make_shared<Config>();
            // Set default world configuration
            worldConfig->set("world.width", 20);
            worldConfig->set("world.height", 20);
            worldConfig->set("world.visionWidth", 8);
            worldConfig->set("world.visionHeight", 8);
            return worldConfig;
        }, "Create a default world configuration")
        .def("validateForTask", [](const Config& config, const std::string& task) {
            // Basic task validation
            if (task == "simple_agent") {
                return config.has("brain.neuron_count") && config.has("world.width");
            } else if (task == "complex_brain") {
                return config.has("brain.neuron_count") && config.has("plasticity.stdp.enable");
            } else if (task == "memory_system") {
                return config.has("memory.working_memory_size");
            }
            return true;
        }, py::arg("task"), "Validate configuration for a specific task")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        
        // ========== ADVANCED SYSTEM ACCESS ==========
        
        // Memory Systems Access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Access working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Access episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Access associative memory system")
        
        // Prediction System Access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Access prediction system")
        
        // Cognition Systems Access
        .def("getNeuralPlanner", &Brain::getNeuralPlanner,
             py::return_value_policy::reference_internal,
             "Access neural planning system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Access concept formation system")
        .def("getAttentionalSelection", &Brain::getAttentionalSelection,
             py::return_value_policy::reference_internal,
             "Access attentional selection system")
        
        // Neuromodulation Systems Access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Access dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Access curiosity neuromodulator")
        .def("getPredictionError", &Brain::getPredictionError,
             py::return_value_policy::reference_internal,
             "Access prediction error neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Access novelty neuromodulator")
        
        // Plasticity Systems Access
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Access STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Access Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Access structural plasticity system")
        
        // Neural Dynamics Access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Access spike system")
        .def("getNeuralDynamics", &Brain::getNeuralDynamics,
             py::return_value_policy::reference_internal,
             "Access neural dynamics system")
        
        // Development System Access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Access development system")
        
        // Checkpoint System Access
        .def("getCheckpointManager", &Brain::getCheckpointManager,
             py::return_value_policy::reference_internal,
             "Access checkpoint system")
        
        // ========== ADVANCED STATISTICS AND ANALYSIS ==========
        
        // Comprehensive Statistics
        .def("getBrainStatistics", &Brain::getBrainStatistics,
             "Get comprehensive brain statistics")
        .def("getRegionStatistics", &Brain::getRegionStatistics,
             "Get statistics for all regions")
        .def("getNeuronStatistics", &Brain::getNeuronStatistics,
             "Get detailed neuron statistics")
        .def("getSynapseStatistics", &Brain::getSynapseStatistics,
             "Get detailed synapse statistics")
        .def("getMemoryStatistics", &Brain::getMemoryStatistics,
             "Get memory system statistics")
        .def("getPredictionStatistics", &Brain::getPredictionStatistics,
             "Get prediction system statistics")
        .def("getCognitionStatistics", &Brain::getCognitionStatistics,
             "Get cognition system statistics")
        .def("getNeuromodulationStatistics", &Brain::getNeuromodulationStatistics,
             "Get neuromodulation system statistics")
        .def("getPlasticityStatistics", &Brain::getPlasticityStatistics,
             "Get plasticity system statistics")
        .def("getDevelopmentStatistics", &Brain::getDevelopmentStatistics,
             "Get development system statistics")
        
        // Performance Metrics
        .def("getPerformanceMetrics", &Brain::getPerformanceMetrics,
             "Get brain performance metrics")
        .def("getLearningEfficiency", &Brain::getLearningEfficiency,
             "Get learning efficiency metrics")
        .def("getEnergyConsumption", &Brain::getEnergyConsumption,
             "Get energy consumption metrics")
        .def("getComputationalLoad", &Brain::getComputationalLoad,
             "Get computational load metrics")
        
        // ========== ADVANCED CONTROL AND CONFIGURATION ==========
        
        // System Control
        .def("pauseSimulation", &Brain::pauseSimulation,
             "Pause the simulation")
        .def("resumeSimulation", &Brain::resumeSimulation,
             "Resume the simulation")
        .def("resetSystem", &Brain::resetSystem,
             "Completely reset the entire system")
        .def("clearAllPlasticity", &Brain::clearAllPlasticity,
             "Clear all synaptic modifications")
        .def("resetMemorySystems", &Brain::resetMemorySystems,
             "Reset all memory systems")
        .def("resetPredictionSystem", &Brain::resetPredictionSystem,
             "Reset prediction system")
        .def("resetCognitionSystems", &Brain::resetCognitionSystems,
             "Reset all cognition systems")
        .def("resetNeuromodulation", &Brain::resetNeuromodulation,
             "Reset neuromodulation systems")
        
        // Learning Control
        .def("enableLearning", &Brain::enableLearning,
             py::arg("enable"),
             "Enable/disable learning across all systems")
        .def("isLearningEnabled", &Brain::isLearningEnabled,
             "Check if learning is enabled")
        .def("setLearningRate", &Brain::setLearningRate,
             py::arg("rate"),
             "Set global learning rate")
        .def("getLearningRate", &Brain::getLearningRate,
             "Get current learning rate")
        .def("optimizeLearningParameters", &Brain::optimizeLearningParameters,
             "Automatically optimize learning parameters")
        
        // Developmental Control
        .def("startDevelopmentPhase", &Brain::startDevelopmentPhase,
             py::arg("stage"),
             "Start a specific developmental phase")
        .def("getDevelopmentProgress", &Brain::getDevelopmentProgress,
             "Get current developmental progress")
        .def("scheduleDevelopment", &Brain::scheduleDevelopment,
             py::arg("stages"),
             "Schedule developmental stages")
        
        // Neuromodulation Control
        .def("setDopamineLevel", &Brain::setDopamineLevel,
             py::arg("level"),
             "Set dopamine neuromodulation level")
        .def("setCuriosityLevel", &Brain::setCuriosityLevel,
             py::arg("level"),
             "Set curiosity neuromodulation level")
        .def("setPredictionErrorSensitivity", &Brain::setPredictionErrorSensitivity,
             py::arg("sensitivity"),
             "Set prediction error sensitivity")
        .def("setNoveltyThreshold", &Brain::setNoveltyThreshold,
             py::arg("threshold"),
             "Set novelty detection threshold")
        .def("getNeuromodulationSettings", &Brain::getNeuromodulationSettings,
             "Get current neuromodulation settings")
        
        // Advanced Memory Control
        .def("enableMemoryReplay", &Brain::enableMemoryReplay,
             py::arg("enable"),
             "Enable/disable memory replay")
        .def("setReplayFrequency", &Brain::setReplayFrequency,
             py::arg("frequency"),
             "Set memory replay frequency")
        .def("getMemoryReplayStats", &Brain::getMemoryReplayStats,
             "Get memory replay statistics")
        .def("triggerMemoryConsolidation", &Brain::triggerMemoryConsolidation,
             py::arg("priority"),
             "Trigger memory consolidation")
        
        // Prediction System Control
        .def("enablePrediction", &Brain::enablePrediction,
             py::arg("enable"),
             "Enable/disable prediction system")
        .def("setPredictionAccuracy", &Brain::setPredictionAccuracy,
             py::arg("accuracy"),
             "Set prediction accuracy target")
        .def("getPredictionAccuracy", &Brain::getPredictionAccuracy,
             "Get current prediction accuracy")
        .def("triggerPredictionError", &Brain::triggerPredictionError,
             py::arg("error"),
             "Trigger a prediction error")
        
        // Cognition System Control
        .def("enablePlanning", &Brain::enablePlanning,
             py::arg("enable"),
             "Enable/disable neural planning")
        .def("enableConceptFormation", &Brain::enableConceptFormation,
             py::arg("enable"),
             "Enable/disable concept formation")
        .def("enableAttention", &Brain::enableAttention,
             py::arg("enable"),
             "Enable/disable attentional selection")
        .def("setPlanningHorizon", &Brain::setPlanningHorizon,
             py::arg("horizon"),
             "Set planning horizon")
        
        // Plasticity Control
        .def("enableSTDPEnabled", &Brain::enableSTDPEnabled,
             py::arg("enabled"),
             "Enable/disable STDP plasticity")
        .def("enableHebbianEnabled", &Brain::enableHebbianEnabled,
             py::arg("enabled"),
             "Enable/disable Hebbian plasticity")
        .def("enableStructuralPlasticityEnabled", &Brain::enableStructuralPlasticityEnabled,
             py::arg("enabled"),
             "Enable/disable structural plasticity")
        .def("resetPlasticity", &Brain::resetPlasticity,
             py::arg("rule"),
             "Reset specific plasticity rule")
        
        // Development System Control
        .def("setPlasticityRateModifier", &Brain::setPlasticityRateModifier,
             py::arg("modifier"),
             "Set plasticity rate modifier")
        .def("getPlasticityRateModifier", &Brain::getPlasticityRateModifier,
             "Get current plasticity rate modifier")
        .def("setCriticalPeriodActive", &Brain::setCriticalPeriodActive,
             py::arg("active"),
             "Activate/deactivate critical period")
        
        // ========== ADVANCED I/O AND STATE MANAGEMENT ==========
        
        // State Export/Import
        .def("exportState", &Brain::exportState,
             py::arg("filename"),
             "Export complete brain state")
        .def("importState", &Brain::importState,
             py::arg("filename"),
             "Import complete brain state")
        .def("exportBrainSnapshot", &Brain::exportBrainSnapshot,
             "Export brain snapshot")
        .def("importBrainSnapshot", &Brain::importBrainSnapshot,
             py::arg("snapshot"),
             "Import brain snapshot")
        .def("getStateHistory", &Brain::getStateHistory,
             "Get state history")
        .def("rollbackState", &Brain::rollbackState,
             py::arg("step"),
             "Rollback to specific step")
        
        // Batch Operations
        .def("batchProcessInputs", &Brain::batchProcessInputs,
             py::arg("inputs"),
             "Process multiple sensory inputs in batch")
        .def("batchGenerateActions", &Brain::batchGenerateActions,
             py::arg("steps"),
             "Generate multiple actions in batch")
        .def("batchSaveState", &Brain::batchSaveState,
             py::arg("directory"),
             "Save multiple brain states")
        
        // Debugging and Analysis
        .def("analyzeConnectivity", &Brain::analyzeConnectivity,
             "Analyze neural connectivity patterns")
        .def("detectDeadlocks", &Brain::detectDeadlocks,
             "Detect simulation deadlocks")
        .def("profilePerformance", &Brain::profilePerformance,
             "Profile brain performance")
        .def("getPerformanceProfile", &Brain::getPerformanceProfile,
             "Get performance profile")
        .def("benchmarkSystem", &Brain::benchmarkSystem,
             "Run system benchmarks")
        
        // ========== SPECIALIZED EXPERT MODES ==========
        
        // Expert modes for different use cases
        .def("setExpertMode", &Brain::setExpertMode,
             py::arg("mode"),
             "Set expert mode (fast, accurate, balanced, creative, exploratory)")
        .def("getExpertMode", &Brain::getExpertMode,
             "Get current expert mode")
        .def("optimizeForLearning", &Brain::optimizeForLearning,
             "Optimize brain for maximum learning")
        .def("optimizeForMemory", &Brain::optimizeForMemory,
             "Optimize brain for memory retention")
        .def("optimizeForSpeed", &Brain::optimizeForSpeed,
             "Optimize brain for processing speed")
        .def("optimizeForAccuracy", &Brain::optimizeForAccuracy,
             "Optimize brain for prediction accuracy")
        
        // Research and experimentation
        .def("startExperiment", &Brain::startExperiment,
             py::arg("name"),
             "Start a research experiment")
        .def("endExperiment", &Brain::endExperiment,
             "End current experiment")
        .def("recordExperimentData", &Brain::recordExperimentData,
             py::arg("data"),
             "Record experiment data")
        .def("analyzeExperimentData", &Brain::analyzeExperimentData,
             "Analyze experiment data")
        
        // ========== ERROR HANDLING AND VALIDATION ==========
        
        // Safety checks and validation
        .def("validateSystemIntegrity", &Brain::validateSystemIntegrity,
             "Validate system integrity")
        .def("checkForErrors", &Brain::checkForErrors,
             "Check for simulation errors")
        .def("getErrorLog", &Brain::getErrorLog,
             "Get error log")
        .def("clearErrorLog", &Brain::clearErrorLog,
             "Clear error log")
        .def("getSimulationWarnings", &Brain::getSimulationWarnings,
             "Get simulation warnings")
        .def("getSafetyReports", &Brain::getSafetyReports,
             "Get safety reports")
        
        // ========== PYTHON-SPECIFIC INTERFACE ==========
        
        // Python-friendly methods
        .def("toPythonDict", &Brain::toPythonDict,
             "Convert brain state to Python dictionary")
        .def("fromPythonDict", &Brain::fromPythonDict,
             py::arg("state"),
             "Restore brain state from Python dictionary")
        .def("simulatePythonCode", &Brain::simulatePythonCode,
             py::arg("code"),
             "Execute Python code in brain context")
        .def("getPythonIntegration", &Brain::getPythonIntegration,
             "Get Python integration interface")
        .def("exportToJSON", &Brain::exportToJSON,
             "Export brain state to JSON")
        .def("importFromJSON", &Brain::importFromJSON,
             py::arg("json"),
             "Import brain state from JSON")
        .def("getHyperparameters", &Brain::getHyperparameters,
             "Get current hyperparameters")
        .def("setHyperparameters", &Brain::setHyperparameters,
             py::arg("params"),
             "Set hyperparameters")
        .def("getConfigTemplate", &Brain::getConfigTemplate,
             "Get configuration template")
        .def("validateConfig", &Brain::validateConfig,
             py::arg("config"),
             "Validate configuration")
        .def("generateConfig", &Brain::generateConfig,
             py::arg("task"),
             "Generate configuration for specific task");

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
        
        // Advanced neuromodulation control
        .def("setDopamineLevel", &AgentBrain::setDopamineLevel, py::arg("level"),
             "Set dopamine neuromodulation level")
        .def("setCuriosityTarget", &AgentBrain::setCuriosityTarget, py::arg("target"),
             "Set curiosity exploration target")
        .def("setNoveltyThreshold", &AgentBrain::setNoveltyThreshold, py::arg("threshold"),
             "Set novelty detection threshold")
        .def("getDopamineLevel", &AgentBrain::getDopamineLevel,
             "Get current dopamine level")
        .def("getCuriosityTarget", &AgentBrain::getCuriosityTarget,
             "Get curiosity target")
        .def("getNoveltyThreshold", &AgentBrain::getNoveltyThreshold,
             "Get novelty threshold")
        
        // Advanced learning control
        .def("setLearningRate", &AgentBrain::setLearningRate, py::arg("rate"),
             "Set global learning rate")
        .def("setSTDPEnabled", &AgentBrain::setSTDPEnabled, py::arg("enabled"),
             "Enable/disable STDP plasticity")
        .def("setHebbianEnabled", &AgentBrain::setHebbianEnabled, py::arg("enabled"),
             "Enable/disable Hebbian learning")
        .def("setStructuralPlasticityEnabled", &AgentBrain::setStructuralPlasticityEnabled,
             py::arg("enabled"),
             "Enable/disable structural plasticity")
        .def("isSTDPEnabled", &AgentBrain::isSTDPEnabled,
             "Check if STDP is enabled")
        .def("isHebbianEnabled", &AgentBrain::isHebbianEnabled,
             "Check if Hebbian learning is enabled")
        
        // Advanced analysis and monitoring
        .def("getLearningStatistics", &AgentBrain::getLearningStatistics,
             "Get comprehensive learning statistics")
        .def("getPlasticityStatistics", &AgentBrain::getPlasticityStatistics,
             "Get plasticity rule statistics")
        .def("getNeuromodulationStatistics", &AgentBrain::getNeuromodulationStatistics,
             "Get neuromodulation system statistics")
        .def("getPerformanceMetrics", &AgentBrain::getPerformanceMetrics,
             "Get brain performance metrics")
        .def("analyzePatternLearning", &AgentBrain::analyzePatternLearning,
             "Analyze pattern learning progress")
        
        // Advanced pattern encoding/decoding
        .def("encodePattern", &AgentBrain::encodePattern, py::arg("pattern"),
             "Encode a pattern for storage and learning")
        .def("decodePattern", &AgentBrain::decodePattern, py::arg("patternId"),
             "Decode a stored pattern")
        .def("getPatternCount", &AgentBrain::getPatternCount,
             "Get number of learned patterns")
        .def("removePattern", &AgentBrain::removePattern, py::arg("patternId"),
             "Remove a learned pattern")
        
        // Development and training control
        .def("startDevelopmentPhase", &AgentBrain::startDevelopmentPhase,
             py::arg("phase"),
             "Start a specific development phase")
        .def("getDevelopmentProgress", &AgentBrain::getDevelopmentProgress,
             "Get development phase progress")
        .def("getDevelopmentStats", &AgentBrain::getDevelopmentStats,
             "Get comprehensive development statistics")
        .def("scheduleTrainingSession", &AgentBrain::scheduleTrainingSession,
             py::arg("duration"), py::arg("goals"),
             "Schedule a training session")
        .def("getTrainingSchedule", &AgentBrain::getTrainingSchedule,
             "Get current training schedule")
        
        // Advanced I/O and batch operations
        .def("batchProcessPercepts", &AgentBrain::batchProcessPercepts,
             py::arg("percepts"),
             "Process multiple percepts in batch")
        .def("batchApplyRewards", &AgentBrain::batchApplyRewards,
             py::arg("rewards"), py::arg("predictions"),
             "Apply multiple rewards in batch")
        .def("exportBrainState", &AgentBrain::exportBrainState,
             py::arg("filename"),
             "Export brain state to file")
        .def("importBrainState", &AgentBrain::importBrainState,
             py::arg("filename"),
             "Import brain state from file")
        .def("getBrainSnapshot", &AgentBrain::getBrainSnapshot,
             "Get current brain state snapshot")
        .def("restoreBrainSnapshot", &AgentBrain::restoreBrainSnapshot,
             py::arg("snapshot"),
             "Restore brain state from snapshot")
        
        // Configuration and tuning
        .def("tuneForTask", &AgentBrain::tuneForTask, py::arg("task"),
             "Tune brain for specific task")
        .def("getTuningParameters", &AgentBrain::getTuningParameters,
             "Get current tuning parameters")
        .def("optimizeParameters", &AgentBrain::optimizeParameters,
             "Automatically optimize brain parameters")
        .def("getOptimizationProgress", &AgentBrain::getOptimizationProgress,
             "Get optimization progress")
        .def("setExplorationRate", &AgentBrain::setExplorationRate, py::arg("rate"),
             "Set exploration rate (0-1)")
        .def("getExplorationRate", &AgentBrain::getExplorationRate,
             "Get current exploration rate")
        .def("setExploitationRate", &AgentBrain::setExploitationRate, py::arg("rate"),
             "Set exploitation rate (0-1)")
        .def("getExploitationRate", &AgentBrain::getExploitationRate,
             "Get current exploitation rate");

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
