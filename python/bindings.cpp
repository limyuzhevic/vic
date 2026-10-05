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
        // Advanced brain management methods
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get spike system access")
        .def("getSTDP", &Brain::getSTDP,
             "Get STDP plasticity access")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian plasticity access")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity access")
        // Working memory access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory access")
        // Episodic memory access
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory access")
        // Associative memory access
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory access")
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system access")
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner access")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation access")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection access")
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system access")
        // Neuromodulation systems access
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator access")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator access")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty neuromodulator access")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal access")
    // Random number generator access
    .def("getRandomGenerator", &Brain::getRandomGenerator,
         "Get random generator access")
    // Advanced brain management methods
    // Configuration methods
    .def("configure", &Brain::getConfig,
         py::return_value_policy::reference_internal,
         "Get configuration for modification")
    .def("configureNeuronCount", [](Brain& self, size_t count) {
        auto config = self.getConfig();
        config->set("neuron_count", count);
    }, py::arg("count"),
         "Configure neuron count")
    .def("configureSynapseDensity", [](Brain& self, float density) {
        auto config = self.getConfig();
        config->set("synapse_density", density);
    }, py::arg("density"),
         "Configure synapse density")
    // Utility methods
    .def("getStatistics", &Brain::logStatus,
         "Get brain statistics and status")
    .def("getInfo", [](const Brain& self) {
        std::ostringstream oss;
        oss << "NLM Brain Statistics:\n";
        oss << "  Neurons: " << self.getTotalNeuronCount() << "\n";
        oss << "  Synapses: " << self.getTotalSynapseCount() << "\n";
        oss << "  Firing: " << self.getFiringNeuronCount() << " (" 
            << self.getAverageFiringRate() << " Hz average)\n";
        oss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
        oss << "  Total Spikes: " << self.getTotalSpikeCount() << "\n";
        return oss.str();
    }, "Get detailed brain information")
    // Memory management utilities
    .def("saveCheckpoint", &Brain::save,
         py::arg("filepath"),
         "Save brain state to checkpoint file")
    .def("loadCheckpoint", &Brain::load,
         py::arg("filepath"),
         "Load brain state from checkpoint file")
    // Advanced simulation control
    .def("pause", &Brain::step, py::arg("0"),
         "Pause simulation (no-ops on brain)")
    .def("resume", [](Brain& self, SimulationStep step) {
        // Just call step with a high step number to effectively continue
        self.step(step);
    }, py::arg("step"),
         "Resume simulation")
    // Development utilities
    .def("resetDevelopment", [](Brain& self) {
        auto config = self.getConfig();
        config->set("neuron_count", 1000);  // Reset to default
        self.setDevelopmentalStage(DevelopmentalStage::Initial);
    }, "Reset brain development")
    .def("setDevelopmentStage", &Brain::setDevelopmentalStage,
         py::arg("stage"),
         "Force development stage (for testing)")
    // Health and monitoring
    .def("getSystemHealth", [](const Brain& self) {
        std::unordered_map<std::string, bool> health;
        health["brain_initialized"] = &self != nullptr;
        health["has_config"] = self.getConfig() != nullptr;
        health["has_regions"] = self.getRegionCount() > 0;
        health["has_neurons"] = self.getTotalNeuronCount() > 0;
        health["working_memory"] = self.getWorkingMemory() != nullptr;
        health["episodic_memory"] = self.getEpisodicMemory() != nullptr;
        health["prediction_system"] = self.getPredictionSystem() != nullptr;
        health["planner"] = self.getPlanner() != nullptr;
        
        return health;
    }, "Get system health status")
    // Connection utilities
    .def("addInterRegionConnection", &Brain::addInterRegionConnection,
         py::arg("source"), py::arg("target"),
         py::arg("weight") = 0.0f, py::arg("delay") = 1,
         "Add connection between brain regions")
    .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
         py::arg("source"), py::arg("target"),
         "Remove connection between brain regions")
    // Batch operations
    .def("batchReset", [](Brain& self) {
        self.reset();
        auto config = self.getConfig();
        if (config) {
            config->set("neuron_count", 1000);
        }
    }, "Reset brain and restore default configuration")
    .def("batchStep", [](Brain& self, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            self.step(i);
        }
    }, py::arg("steps"),
         "Run multiple simulation steps efficiently");
    
    // AgentBrain advanced methods
    .def("enableAllSystems", &AgentBrain::enableRewardModulation,
         py::arg("true"),
         "Enable all neuromodulation and learning systems")
    .def("enableAllSystems", &AgentBrain::enableStructuralPlasticity,
         py::arg("true"),
         "Enable structural plasticity")
    .def("enableAllSystems", &AgentBrain::enableDevelopment,
         py::arg("true"),
         "Enable developmental changes")
    .def("enableAllSystems", &AgentBrain::enableCuriosity,
         py::arg("true"),
         "Enable curiosity-driven exploration")
    .def("getSystemStatus", [](const AgentBrain& self) {
        std::ostringstream oss;
        oss << "AgentBrain Status:\n";
        oss << "  Reward Modulation: " << (self.isRewardModulationEnabled() ? "ENABLED" : "DISABLED") << "\n";
        oss << "  Structural Plasticity: " << (self.isStructuralPlasticityEnabled() ? "ENABLED" : "DISABLED") << "\n";
        oss << "  Development: " << (self.isDevelopmentEnabled() ? "ENABLED" : "DISABLED") << "\n";
        oss << "  Curiosity: " << (self.isCuriosityEnabled() ? "ENABLED" : "DISABLED") << "\n";
        oss << "  Current Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
        oss << "  Neuromodulation Level: " << self.getNeuromodulationLevel() << "\n";
        oss << "  Curiosity Level: " << self.getCuriosityLevel() << "\n";
        oss << "  Novelty Level: " << self.getNoveltyLevel() << "\n";
        oss << "  Prediction Error: " << self.getPredictionError() << "\n";
        return oss.str();
    }, "Get detailed agent status")
    .def("resetSystems", &AgentBrain::reset,
         "Reset all agent systems for new episode")
    .def("setExplorationLevel", [](AgentBrain& self, float level) {
        self.enableCuriosity(level > 0.0f);
    }, py::arg("level"),
         "Set exploration level (0.0-1.0)");
    
    // SimpleWorld advanced methods
    .def("setEnvironmentSeed", &SimpleWorld::setRandomSeed,
         py::arg("seed"),
         "Set random seed for reproducible world")
    .def("getEnvironmentInfo", [](SimpleWorld& self) {
        std::ostringstream oss;
        oss << "World Configuration:\n";
        oss << "  Size: " << self.getWidth() << "x" << self.getHeight() << "\n";
        oss << "  Vision: " << self.getVisionWidth() << "x" << self.getVisionHeight() << "\n";
        oss << "  Max Energy: " << self.getMaxEnergy() << "\n";
        oss << "  Energy Decay Rate: " << self.getEnergyDecayRate() << "\n";
        oss << "  Current Time: " << self.getSimulationTime() << "\n";
        return oss.str();
    }, "Get detailed world configuration")
    .def("setEnergyParameters", [](SimpleWorld& self, float maxEnergy, float decayRate) {
        self.setMaxEnergy(maxEnergy);
        self.setEnergyDecayRate(decayRate);
    }, py::arg("maxEnergy"), py::arg("decayRate"),
         "Configure energy parameters")
    .def("addResource", &SimpleWorld::addObject,
         py::arg("obj"),
         "Add resource object to world")
    .def("addHazard", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
        WorldObject obj(x, y, WorldObjectType::Hazard, value);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
         "Add hazard object to world")
    .def("addResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
        WorldObject obj(x, y, WorldObjectType::Resource, value);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
         "Add resource object to world")
    .def("addWall", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
        WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
         "Add wall object to world")
    .def("simulateStep", &SimpleWorld::update,
         py::arg("timestep"),
         "Run world simulation step")
    .def("getObservation", &SimpleWorld::getSensoryPercept,
         py::return_value_policy::reference_internal,
         "Get current sensory observation")
    .def("getAgentPosition", [](SimpleWorld& self) {
        AgentBody body = self.getAgentBody();
        return std::make_pair(body.x, body.y);
    }, "Get agent position (x, y)")
    .def("getAgentOrientation", [](SimpleWorld& self) {
        return self.getAgentBody().orientation;
    }, "Get agent orientation (degrees)");
    
    // Config advanced methods
    .def("getConfigKeys", &Config::getKeys,
         "Get all configuration keys")
    .def("hasConfig", &Config::has,
         py::arg("key"),
         "Check if configuration key exists")
    .def("getConfigValue", [](Config& self, const std::string& key) {
        if (!self.has(key)) {
            throw py::key_error("Configuration key not found: " + key);
        }
        std::string result;
        std::visit([&result](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                result = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                result = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, double>) {
                result = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                result = std::to_string(arg);
            } else {
                result = "";
            }
        }, self.get(ConfigValue{0})?self.get<ConfigValue>(key):ConfigValue{});
        return result;
    }, py::arg("key"),
         "Get configuration value as string")
    .def("setConfigValue", [](Config& self, const std::string& key, const std::string& value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
         "Set configuration value")
    .def("getConfigSummary", &Config::summary,
         "Get configuration summary as formatted string")
    .def("mergeConfig", [](Config& self, const Config& other) {
        // This is a simplified merge - for proper implementation, need deeper integration
        for (const auto& key : other.getKeys()) {
            if (auto val = other.get<std::string>(key)) {
                self.set(key, *val);
            }
        }
    }, py::arg("other"),
         "Merge another configuration into this one")
    .def("createTemplateConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("synapse_density", 0.1);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001);
        config->set("replay_interval", 100);
        config->set("consolidation_interval", 1000);
        config->set("random_seed", 42ULL);
        return config;
    }, "Create a template configuration with standard values");
    
    // Global factory functions with advanced presets
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface");
    
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create default configuration");
    
    // Advanced preset configurations
    m.def("createSmallBrainConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.05);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        return config;
    }, "Create configuration for small brain simulation");
    
    m.def("createLargeBrainConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 5000);
        config->set("region_count", 5);
        config->set("connection_probability", 0.1);
        config->set("stdp_ltp_weight", 0.015f);
        config->set("stdp_ltd_weight", 0.012f);
        return config;
    }, "Create configuration for large brain simulation");
    
    m.def("createDevelopmentalConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("development_enabled", true);
        config->set("synaptogenesis_rate", 0.0002f);
        config->set("pruning_rate", 0.00002f);
        config->set("replay_interval", 50);
        config->set("consolidation_interval", 500);
        return config;
    }, "Create configuration optimized for developmental learning");
    
    m.def("createHighLearningConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000);
        config->set("connection_probability", 0.15);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.018f);
        config->set("plasticity_stdp_enable", true);
        config->set("plasticity_hebbian_enable", true);
        config->set("plasticity_structural_enable", true);
        return config;
    }, "Create configuration optimized for high learning capacity");
    
    // Error handling types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    
    // Constants
    m.attr("VERSION") = "1.0.0";
    m.attr("PHASE") = "6";
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1f;
    m.attr("DEFAULT_TIMESTEP") = 0.001;
    
    // Documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Advanced Python API
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        This API provides advanced features for researchers and developers.
        
        Key Features:
        - Advanced brain management and simulation control
        - Enhanced world and agent management
        - Configuration utilities and presets
        - Development and debugging tools
        - System monitoring and health checks
        - Batch operations for efficient simulation
        
        Usage Examples:
        
        Advanced Brain Simulation:
            >>> import pynlm
            >>> config = pynlm.createDevelopmentalConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Run simulation with monitoring
            >>> for i in range(1000):
            ...     brain.step(i)
            ...     if i % 100 == 0:
            ...         print(brain.getStatistics())
        
        Complex World Environment:
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(50, 50, 16, 16)
            >>> world.addResource(10.0, 10.0, 5.0)  # Food
            >>> world.addHazard(30.0, 30.0, 2.0)    # Dangerous area
            >>> 
            >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> 
            >>> # Enable advanced learning
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
            >>> agent.enableDevelopment(True)
        
        Developmental Learning:
            >>> config = pynlm.createDevelopmentalConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Simulate development over time
            >>> for step in range(5000):
            ...     brain.step(step)
            ...     stage = brain.getDevelopmentalStage()
            ...     if stage.name() in ['Initial', 'CriticalPeriod', 'Maturation']:
            ...         print(f"Stage: {stage.name()}, Plasticity: {brain.getDevelopmentSystem().getPlasticityLevel()}")
        
        System Monitoring:
            >>> health = brain.getSystemHealth()
            >>> print("Brain initialized:", health["brain_initialized"])
            >>> print("Has regions:", health["has_regions"])
            >>> print("Working memory:", health["working_memory"])
            
        Advanced Configuration:
            >>> config = pynlm.createHighLearningConfig()
            >>> config.set("neuron_count", 3000)
            >>> config.set("connection_probability", 0.2)
            >>> 
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
    )pbdoc";

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
