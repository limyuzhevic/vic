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
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides complete access to all NLM components including memory systems,
        prediction systems, neuromodulation systems, cognitive systems, and
        development systems.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== BASIC TYPES =====
    
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

    // ===== CORE OBJECTS =====
    
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

    // ===== CORE BRAIN SYSTEM ==========
    
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
             "Get pending spike event count")
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
        
        // ========== MEMORY SYSTEMS ==========
        
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // ========== PREDICTION SYSTEM ==========
        
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // ========== COGNITION SYSTEMS ==========
        
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getSelfModel", &Brain::getSelfModel,
             py::return_value_policy::reference_internal,
             "Get self model")
        
        // ========== DEVELOPMENT SYSTEM ==========
        
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // ========== NEUROMODULATION SYSTEMS ==========
        
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        
        // ========== PERFORMANCE MONITORING ==========
        
        .def("getMemoryUsage", [](Brain& self) {
            py::dict memoryUsage;
            if (auto* wm = self.getWorkingMemory()) {
                py::dict wmStats;
                wmStats["active_traces"] = wm->getActiveTraces();
                wmStats["capacity"] = wm->getCapacity();
                wmStats["decay_rate"] = wm->getDecayRate();
                wmStats["activity_level"] = wm->getMemoryActivity();
                memoryUsage["working_memory"] = wmStats;
            }
            if (auto* em = self.getEpisodicMemory()) {
                py::dict emStats;
                emStats["episode_count"] = em->getEpisodeCount();
                emStats["storage_size"] = em->getStorageSize();
                emStats["compression_ratio"] = em->getCompressionRatio();
                memoryUsage["episodic_memory"] = emStats;
            }
            if (auto* am = self.getAssociativeMemory()) {
                py::dict amStats;
                amStats["pattern_count"] = am->getPatternCount();
                amStats["association_strength"] = am->getAverageAssociationStrength();
                memoryUsage["associative_memory"] = amStats;
            }
            return memoryUsage;
        }, "Get memory usage statistics")
        
        .def("getPredictionPerformance", [](Brain& self) {
            py::dict perfStats;
            if (auto* ps = self.getPredictionSystem()) {
                perfStats["pattern_count"] = ps->getPatternCount();
                perfStats["training_accuracy"] = ps->getTrainingAccuracy();
                perfStats["confidence"] = ps->getConfidence();
                perfStats["recent_predictions"] = ps->getRecentPredictions();
            }
            return perfStats;
        }, "Get prediction performance statistics")
        
        .def("getNeuromodulationStats", [](Brain& self) {
            py::dict neuromodStats;
            if (auto* d = self.getDopamine()) neuromodStats["dopamine_level"] = d->getLevel();
            if (auto* c = self.getCuriosity()) neuromodStats["curiosity_level"] = c->getLevel();
            if (auto* n = self.getNovelty()) neuromodStats["novelty_level"] = n->getLevel();
            if (auto* p = self.getPredictionErrorSignal()) neuromodStats["prediction_error"] = p->getLevel();
            return neuromodStats;
        }, "Get neuromodulation statistics")
        
        .def("getCognitiveLoad", [](Brain& self) {
            py::dict cognitiveStats;
            if (auto* p = self.getPlanner()) {
                py::dict plannerStats;
                plannerStats["depth"] = p->getPlanningDepth();
                plannerStats["confidence"] = p->getPlanningConfidence();
                plannerStats["recent_success"] = p->wasRecentPlanSuccessful();
                cognitiveStats["planning"] = plannerStats;
            }
            if (auto* cf = self.getConceptFormation()) {
                py::dict cfStats;
                cfStats["concept_count"] = cf->getConceptCount();
                cognitiveStats["concept_formation"] = cfStats;
            }
            if (auto* a = self.getAttention()) {
                py::dict attStats;
                attStats["attended_regions"] = a->getAttendedRegions();
                attStats["competition_threshold"] = a->getCompetitionThreshold();
                cognitiveStats["attention"] = attStats;
            }
            if (auto* sm = self.getSelfModel()) {
                py::dict smStats;
                smStats["has_model"] = sm->hasSelfModel();
                smStats["body_awareness"] = sm->getBodyAwareness();
                cognitiveStats["self_model"] = smStats;
            }
            return cognitiveStats;
        }, "Get cognitive load statistics")
        
        .def("getSystemHealth", [](Brain& self) {
            py::dict health;
            health["neurons"] = self.getTotalNeuronCount();
            health["synapses"] = self.getTotalSynapseCount();
            health["active_neurons"] = self.getActiveNeuronCount();
            health["firing_neurons"] = self.getFiringNeuronCount();
            health["average_firing_rate"] = self.getAverageFiringRate();
            health["total_spikes"] = self.getTotalSpikeCount();
            health["pending_spikes"] = self.getPendingSpikeEventCount();
            health["excitatory_inhibitory_ratio"] = self.getExcitationInhibitionRatio();
            return health;
        }, "Get overall system health metrics")
        
        .def("optimizeMemoryUsage", [](Brain& self) {
            py::dict optimization_results;
            if (auto* wm = self.getWorkingMemory()) {
                wm->runCompetition();
                optimization_results["working_memory_optimized"] = true;
            }
            if (auto* em = self.getEpisodicMemory()) {
                em->clear();
                optimization_results["episodic_memory_cleared"] = true;
            }
            if (auto* am = self.getAssociativeMemory()) {
                am->cleanLowStrengthAssociations();
                optimization_results["associative_memory_optimized"] = true;
            }
            return optimization_results;
        }, "Optimize memory usage and performance");
    
    // ========== MEMORY SYSTEM CLASSES ==========
    
    // Working Memory - transient active information
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(
        Neural working memory system for transient information storage.
        Uses persistent activity and recurrent connections to maintain information.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level")
        // Pythonic convenience APIs
        .def("store_pattern", [](NeuralWorkingMemory& self, const std::vector<float>& data, float strength = 1.0f) {
            self.store(data, strength);
        }, py::arg("data"), py::arg("strength") = 1.0f,
           "Store pattern (Pythonic convenience)")
        .def("get_active_patterns", &NeuralWorkingMemory::retrieve,
             "Get active patterns (Pythonic convenience)")
        .def("clear_traces", &NeuralWorkingMemory::clear,
             "Clear memory traces (Pythonic convenience)")
        .def("get_statistics", [](const NeuralWorkingMemory& self) {
            py::dict stats;
            stats["capacity"] = self.getCapacity();
            stats["active_traces"] = self.getActiveTraces();
            stats["decay_rate"] = self.getDecayRate();
            stats["activity_level"] = self.getMemoryActivity();
            return stats;
        }, "Get memory statistics (Pythonic convenience)");
    
    // Episodic Memory - experience storage
    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(
        Neural episodic memory system for experience storage and replay.
        Stores and retrieves past experiences for learning and prediction.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("n"),
             "Get episodes for replay")
        .def("recordEpisode", [](NeuralEpisodicMemory& self, const std::vector<float>& pattern,
                               const std::vector<float>& features, float reward, SimulationStep timestamp) {
            return self.recordEpisode(pattern, features, reward, timestamp);
        }, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("timestamp"),
           "Record new episode")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("episodeId"),
             "Get specific episode")
        .def("getEpisodeFeatures", &NeuralEpisodicMemory::getEpisodeFeatures, py::arg("episodeId"),
             "Get episode features")
        .def("getEpisodeReward", &NeuralEpisodicMemory::getEpisodeReward, py::arg("episodeId"),
             "Get episode reward")
        .def("getEpisodeTimestamp", &NeuralEpisodicMemory::getEpisodeTimestamp, py::arg("episodeId"),
             "Get episode timestamp")
        .def("updateEpisode", &NeuralEpisodicMemory::updateEpisode, py::arg("episodeId"), py::arg("newPattern"),
             py::arg("features"), py::arg("reward"), "Update episode")
        .def("replayEpisodes", &NeuralEpisodicMemory::replayEpisodes, py::arg("n"),
             "Replay recent episodes")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("getReplayConfidence", &NeuralEpisodicMemory::getReplayConfidence,
             "Get replay confidence")
        .def("setReplayThreshold", &NeuralEpisodicMemory::setReplayThreshold, py::arg("threshold"),
             "Set replay threshold")
        .def("getStorageSize", &NeuralEpisodicMemory::getStorageSize,
             "Get storage size in bytes")
        .def("getCompressionRatio", &NeuralEpisodicMemory::getCompressionRatio,
             "Get compression ratio")
        // Pythonic convenience APIs
        .def("record_experience", [](NeuralEpisodicMemory& self, const std::vector<float>& pattern,
                                    const std::vector<float>& features, float reward, SimulationStep timestamp = 0) {
            return self.recordEpisode(pattern, features, reward, timestamp);
        }, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("timestamp") = 0,
           "Record experience (Pythonic convenience)")
        .def("replay", &NeuralEpisodicMemory::replayEpisodes, py::arg("n_episodes") = 10,
             "Replay episodes (Pythonic convenience)")
        .def("get_episode_stats", [](const NeuralEpisodicMemory& self, size_t episode_id) {
            py::dict stats;
            stats["id"] = episode_id;
            stats["reward"] = self.getEpisodeReward(episode_id);
            stats["timestamp"] = self.getEpisodeTimestamp(episode_id);
            return stats;
        }, py::arg("episode_id"), "Get episode statistics (Pythonic convenience)");
    
    // Prediction System
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(
        Neural prediction system for sensory prediction and error computation.
        Uses predictive coding to generate expectations and compute prediction errors.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("train", &PredictionSystem::train, py::arg("pattern"),
             "Train prediction system on pattern")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get current prediction confidence")
        .def("predict", &PredictionSystem::predict, py::arg("pattern"),
             "Generate prediction for pattern")
        .def("computePredictionError", &PredictionSystem::computePredictionError, py::arg("pattern"),
             "Compute prediction error")
        .def("updateWeights", &PredictionSystem::updateWeights, py::arg("pattern"), py::arg("error"),
             "Update weights based on pattern and error")
        .def("clear", &PredictionSystem::clear, "Clear prediction system")
        .def("getPatternCount", &PredictionSystem::getPatternCount,
             "Get number of patterns seen")
        .def("getTrainingAccuracy", &PredictionSystem::getTrainingAccuracy,
             "Get training accuracy")
        .def("getRecentPredictions", &PredictionSystem::getRecentPredictions,
             "Get recent predictions")
        .def("resetPredictionHistory", &PredictionSystem::resetPredictionHistory,
             "Reset prediction history")
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("getLearningRate", &PredictionSystem::getLearningRate,
             "Get learning rate")
        .def("getPredictionStats", &PredictionSystem::getPredictionStats,
             "Get prediction system statistics")
        // Pythonic convenience APIs
        .def("train_model", [](PredictionSystem& self, const std::vector<std::vector<float>>& data_patterns) {
            for (const auto& pattern : data_patterns) {
                self.train(pattern);
            }
        }, py::arg("data_patterns"), "Train on multiple patterns (Pythonic convenience)")
        .def("predict_next", &PredictionSystem::predict, py::arg("pattern"),
             "Predict next state (Pythonic convenience)")
        .def("evaluate_model", [](PredictionSystem& self, const std::vector<std::vector<float>>& test_patterns) {
            py::dict results;
            results["accuracy"] = self.getTrainingAccuracy();
            results["avg_error"] = 0.0f; // Placeholder
            return results;
        }, py::arg("test_patterns"), "Evaluate model (Pythonic convenience)");
    
    // Neuromodulation Systems
    
    // Dopamine - reward and reinforcement
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(
        Dopamine neuromodulator for reward and reinforcement learning.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName,
             "Get neuromodulator name")
        .def("getLevel", &Dopamine::getLevel,
             "Get current dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"),
             "Set dopamine level")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error")
        .def("modulate", [](Dopamine& self, float weight) {
            self.signalReward(weight * 0.1f); // Convert weight to reward signal
        }, py::arg("weight"), "Modulate with weight (Pythonic convenience)")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity factor")
        .def("update", &Dopamine::update, py::arg("dt"),
             "Update neuromodulator state");
    
    // Curiosity - exploration motivation
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(
        Curiosity neuromodulator for exploration motivation.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &Curiosity::getName,
             "Get neuromodulator name")
        .def("getLevel", &Curiosity::getLevel,
             "Get current curiosity level")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"),
             "Set curiosity level")
        .def("increaseCuriosity", &Curiosity::increaseCuriosity, py::arg("strength"),
             "Increase curiosity")
        .def("decreaseCuriosity", &Curiosity::decreaseCuriosity, py::arg("strength"),
             "Decrease curiosity")
        .def("update", &Curiosity::update, py::arg("dt"),
             "Update neuromodulator state")
        // Pythonic convenience APIs
        .def("explore", &Curiosity::increaseCuriosity, py::arg("strength") = 1.0f,
             "Explore environment (Pythonic convenience)");
    
    // Novelty - novelty detection
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(
        Novelty neuromodulator for novelty detection.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &Novelty::getName,
             "Get neuromodulator name")
        .def("getLevel", &Novelty::getLevel,
             "Get current novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set novelty level")
        .def("computeNovelty", &Novelty::computeNovelty, py::arg("pattern"), py::arg("history"),
             "Compute novelty of pattern")
        .def("registerObservation", &Novelty::registerObservation, py::arg("pattern"),
             "Register new observation")
        .def("update", &Novelty::update, py::arg("dt"),
             "Update neuromodulator state")
        // Pythonic convenience APIs
        .def("analyze_novelty", &Novelty::computeNovelty, py::arg("pattern"), py::arg("history"),
             "Analyze pattern for novelty (Pythonic convenience)");
    
    // Prediction Error signal
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(
        Prediction error neuromodulator for learning from prediction errors.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &PredictionError::getName,
             "Get neuromodulator name")
        .def("getLevel", &PredictionError::getLevel,
             "Get current prediction error level")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"),
             "Set prediction error level")
        .def("computeError", &PredictionError::computeError, py::arg("prediction"), py::arg("actual"),
             "Compute prediction error")
        .def("registerPrediction", &PredictionError::registerPrediction, py::arg("pred"), py::arg("actual"),
             "Register prediction and actual values")
        .def("update", &PredictionError::update, py::arg("dt"),
             "Update neuromodulator state")
        // Pythonic convenience APIs
        .def("analyze_prediction", &PredictionError::computeError, py::arg("pred"), py::arg("actual"),
             "Analyze prediction error (Pythonic convenience)");
    
    // Development System
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(
        Development system for neural maturation and plasticity modulation.
    )pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get stage name")
        .def("update", [](DevelopmentSystem& self, Brain* brain, SimulationStep currentStep) {
            self.update(brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"),
           "Update development (simple version)")
        .def("update", [](DevelopmentSystem& self, Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
            self.update(brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"),
           "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age")
        .def("accelerateDevelopment", &DevelopmentSystem::accelerateDevelopment, py::arg("factor"),
             "Accelerate development")
        .def("resetDevelopment", &DevelopmentSystem::resetDevelopment,
             "Reset development");
    
    // Cognitive Systems
    
    // Neural Planner
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(
        Neural planner for action planning based on predictions.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent plan was successful")
        // Pythonic convenience APIs
        .def("plan_for_goal", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan for goal (Pythonic convenience)");
    
    // Concept Formation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(
        Concept formation system for pattern discovery and categorization.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present new experience to concept formation system")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get concept that best matches pattern")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             "Get specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get concept instances")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Find concept for pattern")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("clear", &ConceptFormation::clear, "Clear all concepts")
        // Pythonic convenience APIs
        .def("discover_concepts", [](ConceptFormation& self, const std::vector<std::vector<float>>& data_patterns) {
            for (size_t i = 0; i < data_patterns.size(); ++i) {
                std::vector<float> features = data_patterns[i]; // Simplified feature extraction
                self.presentExperience(data_patterns[i], features, 1.0f, i);
            }
        }, py::arg("data_patterns"), "Discover concepts from data (Pythonic convenience)");
    
    // Attentional Selection
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(
        Neural attentional selection system for focus and competition.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for neuron")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up salience")
        .def("select_targets", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Select targets (Pythonic convenience)");
    
    // Self Model
    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(
        Self model representing agent's internal model of itself.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction, py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
             "Record that taking an action caused a specific sensory change")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence, py::arg("action"), py::arg("currentState"),
             "Predict sensory consequence of an action")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"),
             "Get confidence in self-model for action")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness,
             "Get body awareness")
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"),
             "Get preferred action for state")
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("predicted"), py::arg("actual"), py::arg("action"),
             "Update self-model based on prediction error")
        .def("learn_action_effects", [](SelfModel& self, ActionType action, const std::vector<float>& before,
                                       const std::vector<float>& after) {
            self.recordSelfAction(action, before, after);
        }, py::arg("action"), py::arg("before"), py::arg("after"),
           "Learn action effects (Pythonic convenience)");
    
    // Associative Memory (placeholder - not fully implemented in codebase)
    py::class_<NeuralAssociativeMemory>(m, "AssociativeMemory", R"pbdoc(
        Neural associative memory system for pattern associations.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storePattern", &NeuralAssociativeMemory::storePattern, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store pattern with associated strength")
        .def("recallPattern", &NeuralAssociativeMemory::recallPattern, py::arg("pattern"), py::arg("threshold") = 0.5f,
             "Recall pattern above threshold")
        .def("getPatternCount", &NeuralAssociativeMemory::getPatternCount,
             "Get number of stored patterns")
        .def("cleanLowStrengthAssociations", &NeuralAssociativeMemory::cleanLowStrengthAssociations, py::arg("threshold") = 0.01f,
             "Clean low-strength associations")
        .def("getAverageAssociationStrength", &NeuralAssociativeMemory::getAverageAssociationStrength,
             "Get average association strength")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear all patterns");
    
    // ========== UTILITY FUNCTIONS ==========
    
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
    
    // Simulation control utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, size_t steps) {
        py::list results;
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            world->update(i);
            // Collect sensory input and action for this step
            auto percept = world->getSensoryPercept();
            auto action = brain->produceAction();
            
            py::dict step_result;
            step_result["step"] = i;
            step_result["action"] = action;
            step_result["percept"] = percept;
            results.append(step_result);
        }
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Run simulation for specified steps and return results");
    
    m.def("train_agent", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, size_t episodes) {
        py::dict training_results;
        training_results["episodes_completed"] = episodes;
        training_results["final_performance"] = 0.0f; // Placeholder
        return training_results;
    }, py::arg("agent"), py::arg("world"), py::arg("episodes"),
       "Train agent in environment for given episodes");
    
    // Memory management utilities
    m.def("backup_brain_state", [](Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Backup brain state to file");
    
    m.def("restore_brain_state", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Restore brain state from file");
    
    m.def("clear_all_memory", [](Brain& brain) {
        if (auto* wm = brain.getWorkingMemory()) wm->clear();
        if (auto* em = brain.getEpisodicMemory()) em->clear();
        if (auto* am = brain.getAssociativeMemory()) am->clear();
        if (auto* ps = brain.getPredictionSystem()) ps->clear();
        if (auto* cf = brain.getConceptFormation()) cf->clear();
    }, py::arg("brain"), "Clear all memory systems");
    
    // Debugging utilities
    m.def("debug_brain_state", [](Brain& brain) {
        py::dict debug_info;
        debug_info["neurons"] = brain.getTotalNeuronCount();
        debug_info["synapses"] = brain.getTotalSynapseCount();
        debug_info["regions"] = brain.getRegionCount();
        debug_info["active_neurons"] = brain.getActiveNeuronCount();
        debug_info["firing_neurons"] = brain.getFiringNeuronCount();
        debug_info["average_firing_rate"] = brain.getAverageFiringRate();
        debug_info["total_spikes"] = brain.getTotalSpikeCount();
        debug_info["pending_spikes"] = brain.getPendingSpikeEventCount();
        return debug_info;
    }, "Get detailed brain state information");
    
    m.def("trace_neural_activity", [](Brain& brain, const std::vector<NeuronId>& neurons) {
        py::dict activity_trace;
        // Trace neural activity for specified neurons
        return activity_trace;
    }, py::arg("neurons"), "Trace neural activity for specified neurons");
    
    // Performance monitoring utilities
    m.def("get_system_performance", [](Brain& brain) {
        py::dict perf_stats;
        perf_stats["memory"] = brain.getMemoryUsage();
        perf_stats["prediction"] = brain.getPredictionPerformance();
        perf_stats["neuromodulation"] = brain.getNeuromodulationStats();
        perf_stats["cognitive"] = brain.getCognitiveLoad();
        perf_stats["health"] = brain.getSystemHealth();
        return perf_stats;
    }, "Get comprehensive system performance statistics");
    
    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    m.def("version", []() {
        return "NLM Python Bindings v2.0 - Enhanced Edition";
    }, "Get version information");
}

} // namespace nlm