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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Memory systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"

// Prediction system
#include "../src/prediction/PredictionSystem.hpp"

// Cognition systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

// Neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"

namespace py = pybind11;
namespace nlm {

// Context manager for Brain class
class PyBrainContextManager {
public:
    PyBrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    PyBrainContextManager& operator=(const PyBrainContextManager&) = delete;
    PyBrainContextManager(const PyBrainContextManager&) = delete;
    
    void enter() {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    void exit(int type, const char* value, const char* traceback) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive bindings for Brain, memory systems, 
        prediction systems, cognition systems, and neuromodulation.
    )pbdoc";

    // Register standard exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");

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
        .def("__ne__", &NeuronId::operator!=)
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

    // PopulationId type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
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

    // Configuration class
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

    // Sensory input classes
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

    // Action class
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

    // World object
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

    // Agent body
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

    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory percept
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

    // Simple world
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

    // BRAIN CLASS WITH CONTEXT MANAGER SUPPORT
    py::class_<Brain, PyBrainContextManager>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", &PyBrainContextManager::enter)
        .def("__exit__", &PyBrainContextManager::exit,
             py::arg("type"), py::arg("value"), py::arg("traceback"))
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
        .def("validateConfiguration", &Brain::validateConfiguration,
             "Validate the brain configuration")
        .def("getConfigurationSchema", &Brain::getConfigurationSchema,
             "Get the configuration schema with field definitions and validation rules")
        .def("validate_config", [](std::shared_ptr<Config> config) {
            if (config->validate()) {
                return py::dict("valid", true, "errors", py::list());
            } else {
                // Collect validation errors
                py::list errors;
                // Get all keys and validate them
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    if (auto valueOpt = config->get<std::string>(key)) {
                        if (!config->validateValue(key, *valueOpt)) {
                            errors.append("Key \"" + key + "\": Invalid value \"" + *valueOpt + "\"");
                        }
                    }
                }
                return py::dict("valid", false, "errors", errors);
            }
        }, "Validate configuration and return errors if any")
        .def("get_config_schema", []() {
            return ConfigSchema::getSchema();
        }, "Get configuration schema with field definitions and validation rules")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Memory system status reporting
        .def("getMemorySystemStatus", [](const Brain& brain) {
            std::map<std::string, std::string> status;
            auto* workingMem = brain.getWorkingMemory();
            auto* episodicMem = brain.getEpisodicMemory();
            auto* assocMem = brain.getAssociativeMemory();
            
            if (workingMem) {
                status["working_memory"] = "active";
                status["working_memory_traces"] = std::to_string(workingMem->getActiveTraces());
                status["working_memory_capacity"] = std::to_string(workingMem->getCapacity());
                status["working_memory_decay_rate"] = std::to_string(workingMem->getDecayRate());
                status["working_memory_activity"] = std::to_string(workingMem->getMemoryActivity());
            }
            
            if (episodicMem) {
                status["episodic_memory"] = "active";
                status["episodic_memory_count"] = std::to_string(episodicMem->getEpisodeCount());
                status["episodic_memory_replay_enabled"] = episodicMem->isReplayEnabled() ? "true" : "false";
                status["episodic_memory_max_episodes"] = std::to_string(episodicMem->maxEpisodes_);
            }
            
            if (assocMem) {
                status["associative_memory"] = "active";
                status["associative_memory_count"] = std::to_string(assocMem->getAssociationCount());
            }
            
            return status;
        }, "Get status of all memory systems")
        // Prediction system status
        .def("getPredictionSystemStatus", [](const Brain& brain) {
            std::map<std::string, std::string> status;
            auto* predSys = brain.getPredictionSystem();
            
            if (predSys) {
                status["prediction_system"] = "active";
                status["prediction_error"] = std::to_string(predSys->getPredictionError());
                status["prediction_confidence"] = std::to_string(predSys->getConfidence());
                status["error_history_size"] = std::to_string(predSys->getErrorHistory().size());
            }
            
            return status;
        }, "Get status of prediction system")
        // Cognition system status
        .def("getCognitionSystemStatus", [](const Brain& brain) {
            std::map<std::string, std::string> status;
            auto* planner = brain.getPlanner();
            auto* conceptForm = brain.getConceptFormation();
            auto* attention = brain.getAttention();
            
            if (planner) {
                status["neural_planner"] = "active";
                status["planning_depth"] = std::to_string(planner->getPlanningDepth());
                status["planning_confidence"] = std::to_string(planner->getPlanningConfidence());
                status["recent_plan_successful"] = planner->wasRecentPlanSuccessful() ? "true" : "false";
            }
            
            if (conceptForm) {
                status["concept_formation"] = "active";
                status["concept_count"] = std::to_string(conceptForm->getConceptCount());
                status["formation_threshold"] = std::to_string(conceptForm->formationThreshold_);
            }
            
            if (attention) {
                status["attentional_selection"] = "active";
                status["inhibition_strength"] = std::to_string(attention->inhibitionStrength_);
                status["excitation_strength"] = std::to_string(attention->excitationStrength_);
                status["competition_threshold"] = std::to_string(attention->competitionThreshold_);
            }
            
            return status;
        }, "Get status of cognition systems")
        // Neuromodulation system status
        .def("getNeuromodulationSystemStatus", [](const Brain& brain) {
            std::map<std::string, std::string> status;
            auto* dopamine = brain.getDopamine();
            auto* curiosity = brain.getCuriosity();
            auto* novelty = brain.getNovelty();
            auto* predictionError = brain.getPredictionErrorSignal();
            auto* reward = brain.getReward();
            
            if (dopamine) {
                status["dopamine"] = "active";
                status["dopamine_level"] = std::to_string(dopamine->getLevel());
                status["dopamine_plasticity_factor"] = std::to_string(dopamine->getPlasticityFactor());
            }
            
            if (curiosity) {
                status["curiosity"] = "active";
                status["curiosity_level"] = std::to_string(curiosity->getLevel());
            }
            
            if (novelty) {
                status["novelty"] = "active";
                status["novelty_level"] = std::to_string(novelty->getLevel());
            }
            
            if (predictionError) {
                status["prediction_error"] = "active";
                status["prediction_error_value"] = std::to_string(predictionError->getError());
            }
            
            if (reward) {
                status["reward"] = "active";
                status["reward_value"] = std::to_string(reward->getValue());
            }
            
            return status;
        }, "Get status of neuromodulation systems");

    // AgentBrain class
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

    // FACTORY FUNCTIONS
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // BRAIN MEMORY SYSTEMS BINDINGS

    // NeuralWorkingMemory
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory storage and retrieval system)pbdoc")
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
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level")
        .def("logStatus", [](NeuralWorkingMemory& self) {
            std::string status = "Working Memory Status:\n";
            status += "  Active traces: " + std::to_string(self.getActiveTraces()) + "\n";
            status += "  Capacity: " + std::to_string(self.getCapacity()) + "\n";
            status += "  Decay rate: " + std::to_string(self.getDecayRate()) + "\n";
            status += "  Memory activity: " + std::to_string(self.getMemoryActivity()) + "\n";
            status += "  Memory neurons: " + std::to_string(self.getMemoryNeurons().size()) + "\n";
            status += "  Winning neurons: " + std::to_string(self.isWinning(self.getMemoryNeurons().empty() ? NeuronId() : self.getMemoryNeurons()[0]) ? 1 : 0) + "\n";
            return status;
        }, "Log working memory status");

    // NeuralEpisodicMemory
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory storage for experiences)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience")
        .def("retrieveSimilar", [](NeuralEpisodicMemory& self, const std::vector<float>& sensoryPattern, size_t maxResults) {
            auto episodes = self.retrieveSimilar(sensoryPattern, maxResults);
            std::vector<const EpisodicMemoryItem*> result;
            for (auto* ep : episodes) {
                result.push_back(ep);
            }
            return result;
        }, py::arg("sensoryPattern"), py::arg("maxResults") = 5,
           "Retrieve episodes similar to a query pattern")
        .def("retrieveTemporal", [](NeuralEpisodicMemory& self, SimulationStep startTime, SimulationStep endTime, size_t maxResults) {
            auto episodes = self.retrieveTemporal(startTime, endTime, maxResults);
            std::vector<const EpisodicMemoryItem*> result;
            for (auto* ep : episodes) {
                result.push_back(ep);
            }
            return result;
        }, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
           "Retrieve episodes from a specific time window")
        .def("retrieveByLocation", [](NeuralEpisodicMemory& self, float x, float y, float radius, size_t maxResults) {
            auto episodes = self.retrieveByLocation(x, y, radius, maxResults);
            std::vector<const EpisodicMemoryItem*> result;
            for (auto* ep : episodes) {
                result.push_back(ep);
            }
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
           "Retrieve episodes by location")
        .def("retrieveAfterAction", [](NeuralEpisodicMemory& self, ActionType action, size_t maxResults) {
            auto episodes = self.retrieveAfterAction(action, maxResults);
            std::vector<const EpisodicMemoryItem*> result;
            for (auto* ep : episodes) {
                result.push_back(ep);
            }
            return result;
        }, py::arg("action"), py::arg("maxResults") = 5,
           "Retrieve episodes following a specific action")
        .def("replayEpisode", [](NeuralEpisodicMemory& self, const EpisodicMemoryItem* episode) {
            if (episode) self.replayEpisode(episode);
        }, py::arg("episode"),
             "Replay an episode - reactivates the neural patterns associated with it")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance based on recent use")
        .def("getEpisode", [](NeuralEpisodicMemory& self, size_t index) {
            const EpisodicMemoryItem* episode = self.getEpisode(index);
            return episode;
        }, py::arg("index"),
             "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", [](NeuralEpisodicMemory& self, size_t count) {
            auto episodes = self.getRecentEpisodes(count);
            std::vector<const EpisodicMemoryItem*> result;
            for (auto* ep : episodes) {
                result.push_back(ep);
            }
            return result;
        }, py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate (clear old episodes)")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", [](NeuralEpisodicMemory& self, size_t count) {
            auto episodes = self.getEpisodesForReplay(count);
            std::vector<const EpisodicMemoryItem*> result;
            for (auto* ep : episodes) {
                result.push_back(ep);
            }
            return result;
        }, py::arg("count"),
             "Get episodes for replay")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
             "Replay multiple episodes (for sleep-like consolidation)")
        .def("logStatus", [](NeuralEpisodicMemory& self) {
            std::string status = "Episodic Memory Status:\n";
            status += "  Episode count: " + std::to_string(self.getEpisodeCount()) + "\n";
            status += "  Replay enabled: " + std::string(self.isReplayEnabled() ? "yes" : "no") + "\n";
            status += "  Max episodes: " + std::to_string(self.maxEpisodes_) + "\n";
            status += "  Average reward: " + std::to_string(self.getAverageReward()) + "\n";
            return status;
        }, "Log episodic memory status");

    // NeuralAssociativeMemory
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Pattern associations and memory)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("associateFromExperience", [](NeuralAssociativeMemory& self, const EpisodicMemoryItem& episode) {
            self.associateFromExperience(episode);
        }, py::arg("episode"),
             "Create association based on experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with a query pattern")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"),
             "Get association strength between two patterns")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association based on outcome")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation, py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation from cue pattern")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations")
        .def("logStatus", [](NeuralAssociativeMemory& self) {
            std::string status = "Associative Memory Status:\n";
            status += "  Association count: " + std::to_string(self.getAssociationCount()) + "\n";
            status += "  Pattern neurons: " + std::to_string(self.patternNeurons_.size()) + "\n";
            return status;
        }, "Log associative memory status");

    // EpisodicMemoryItem struct
    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Represents a stored experience)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("sensoryState", &EpisodicMemoryItem::sensoryState)
        .def_readwrite("positionX", &EpisodicMemoryItem::positionX)
        .def_readwrite("positionY", &EpisodicMemoryItem::positionY)
        .def_readwrite("orientation", &EpisodicMemoryItem::orientation)
        .def_readwrite("action", &EpisodicMemoryItem::action)
        .def_readwrite("reward", &EpisodicMemoryItem::reward)
        .def_readwrite("energy", &EpisodicMemoryItem::energy)
        .def_readwrite("novelty", &EpisodicMemoryItem::novelty)
        .def_readwrite("resultingSensoryState", &EpisodicMemoryItem::resultingSensoryState)
        .def_readwrite("resultingReward", &EpisodicMemoryItem::resultingReward)
        .def_readwrite("activeNeurons", &EpisodicMemoryItem::activeNeurons)
        .def_readwrite("neuronActivations", &EpisodicMemoryItem::neuronActivations)
        .def_readwrite("age", &EpisodicMemoryItem::age)
        .def("__repr__", [](const EpisodicMemoryItem& item) {
            std::stringstream ss;
            ss << "<EpisodicMemoryItem: age=" << item.age 
               << ", action=" << static_cast<int>(item.action)
               << ", reward=" << item.reward << ">";
            return ss.str();
        });

    // PredictionSystem
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for predictive coding and error computation)pbdoc")
        .def(py::init<>())
        .def("predictNextState", [](PredictionSystem& self, const SensoryInput& currentState) {
            auto result = self.predictNextState(currentState);
            return result;
        }, py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction error history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model")
        .def("logStatus", [](PredictionSystem& self) {
            std::string status = "Prediction System Status:\n";
            status += "  Prediction error: " + std::to_string(self.getPredictionError()) + "\n";
            status += "  Confidence: " + std::to_string(self.getConfidence()) + "\n";
            status += "  Error history size: " + std::to_string(self.getErrorHistory().size()) + "\n";
            return status;
        }, "Log prediction system status");

    // Cognition Systems

    // NeuralPlanner
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", [](NeuralPlanner& self, const std::vector<ActionType>& actions, const std::vector<float>& startState) {
            PlanningCandidate candidate = self.evaluateSequence(actions, startState);
            return candidate;
        }, py::arg("actions"), py::arg("startState"),
           "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality function (from experience)")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?")
        .def("logStatus", [](NeuralPlanner& self) {
            std::string status = "Neural Planner Status:\n";
            status += "  Planning depth: " + std::to_string(self.getPlanningDepth()) + "\n";
            status += "  Planning confidence: " + std::to_string(self.getPlanningConfidence()) + "\n";
            status += "  Recent plan successful: " + std::string(self.wasRecentPlanSuccessful() ? "yes" : "no") + "\n";
            status += "  Action quality range: [" + std::to_string(self.actionQuality_.size() > 0 ? *std::min_element(self.actionQuality_.begin(), self.actionQuality_.end()) : 0.0f) +
                ", " + std::to_string(self.actionQuality_.size() > 0 ? *std::max_element(self.actionQuality_.begin(), self.actionQuality_.end()) : 0.0f) + "]\n";
            return status;
        }, "Log neural planner status");

    // PlanningCandidate struct
    py::class_<PlanningCandidate>(m, "PlanningCandidate", R"pbdoc(A possible action sequence considered during planning)pbdoc")
        .def(py::init<>())
        .def_readwrite("actions", &PlanningCandidate::actions)
        .def_readwrite("predictedStates", &PlanningCandidate::predictedStates)
        .def_readwrite("expectedReward", &PlanningCandidate::expectedReward)
        .def_readwrite("confidence", &PlanningCandidate::confidence)
        .def_readwrite("depth", &PlanningCandidate::depth)
        .def("__repr__", [](const PlanningCandidate& candidate) {
            std::stringstream ss;
            ss << "<PlanningCandidate: depth=" << candidate.depth 
               << ", reward=" << candidate.expectedReward
               << ", confidence=" << candidate.confidence << ">";
            return ss.str();
        });

    // ConceptFormation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", [](ConceptFormation& self, const std::vector<float>& pattern, const std::vector<float>& features, float reward, SimulationStep currentTime) {
            size_t conceptId = self.presentExperience(pattern, features, reward, currentTime);
            return conceptId;
        }, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
           "Present a new experience to the concept formation system")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get the concept that best matches a pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all concepts")
        .def("getConcept", [](ConceptFormation& self, size_t conceptId) {
            const DiscoveredConcept* concept = self.getConcept(conceptId);
            return concept;
        }, py::arg("conceptId"),
             "Get a specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Measure concept stability (0-1)")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts (when discovered they are actually the same)")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Get concept for a given pattern")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             "Get generalization ability (can concept handle variations?)")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set stability window")
        .def("logStatus", [](ConceptFormation& self) {
            std::string status = "Concept Formation Status:\n";
            status += "  Concept count: " + std::to_string(self.getConceptCount()) + "\n";
            status += "  Formation threshold: " + std::to_string(self.formationThreshold_) + "\n";
            status += "  Stability threshold: " + std::to_string(self.stabilityThreshold_) + "\n";
            status += "  Stability window: " + std::to_string(self.stabilityWindow_) + "\n";
            status += "  Next concept ID: " + std::to_string(self.nextConceptId_) + "\n";
            return status;
        }, "Log concept formation status");

    // DiscoveredConcept struct
    py::class_<DiscoveredConcept>(m, "DiscoveredConcept", R"pbdoc(A concept that emerged from repeated experience)pbdoc")
        .def(py::init<>())
        .def_readwrite("id", &DiscoveredConcept::id)
        .def_readwrite("prototype", &DiscoveredConcept::prototype)
        .def_readwrite("instances", &DiscoveredConcept::instances)
        .def_readwrite("categoryHint", &DiscoveredConcept::categoryHint)
        .def_readwrite("associatedConceptIds", &DiscoveredConcept::associatedConceptIds)
        .def_readwrite("totalObservations", &DiscoveredConcept::totalObservations)
        .def_readwrite("avgStability", &DiscoveredConcept::avgStability)
        .def("__repr__", [](const DiscoveredConcept& concept) {
            std::stringstream ss;
            ss << "<DiscoveredConcept: id=" << concept.id 
               << ", category='" << concept.categoryHint << "'"
               << ", observations=" << concept.totalObservations << ">";
            return ss.str();
        });

    // ConceptInstance struct
    py::class_<ConceptInstance>(m, "ConceptInstance", R"pbdoc(Represents a single instance of a concept)pbdoc")
        .def(py::init<>())
        .def_readwrite("pattern", &ConceptInstance::pattern)
        .def_readwrite("features", &ConceptInstance::features)
        .def_readwrite("observationCount", &ConceptInstance::observationCount)
        .def_readwrite("lastObserved", &ConceptInstance::lastObserved)
        .def_readwrite("avgReward", &ConceptInstance::avgReward)
        .def_readwrite("stability", &ConceptInstance::stability);

    // ConceptFormation Helper Functions
    m.def("computeConceptSimilarity", [](const std::vector<float>& a, const std::vector<float>& b) {
        // Simple cosine similarity implementation
        if (a.empty() || b.empty() || a.size() != b.size()) return 0.0f;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }, py::arg("a"), py::arg("b"),
       "Compute similarity between two patterns (cosine similarity)");

    // AttentionalSelection
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection for focus and selection)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", [](AttentionalSelection& self, const std::vector<NeuronId>& competitors, float globalInhibition) {
            std::vector<NeuronId> winners = self.processCompetition(competitors, globalInhibition);
            return winners;
        }, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
           "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region (focus processing there)")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set attention parameters")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set attention parameters")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set attention parameters")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Is this neuron currently attended?")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Top-down attention (goal-directed)")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Bottom-up attention (stimulus-driven)")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention")
        .def("logStatus", [](AttentionalSelection& self) {
            std::string status = "Attentional Selection Status:\n";
            status += "  Inhibition strength: " + std::to_string(self.inhibitionStrength_) + "\n";
            status += "  Excitation strength: " + std::to_string(self.excitationStrength_) + "\n";
            status += "  Competition threshold: " + std::to_string(self.competitionThreshold_) + "\n";
            status += "  Attended regions: " + std::to_string(self.getAttendedRegions().size()) + "\n";
            status += "  Winners: " + std::to_string(self.getWinners().size()) + "\n";
            return status;
        }, "Log attentional selection status");

    // Neuromodulation Systems

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName,
             "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get plasticity factor for this neuromodulator")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             "Update neuromodulator state");

    // Dopamine
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - reward and reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &Dopamine::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward for reinforcement learning")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get dopamine-modulated plasticity factor")
        .def("update", &Dopamine::update, py::arg("dt"),
             "Update dopamine dynamics")
        .def("logStatus", [](Dopamine& self) {
            std::string status = "Dopamine Status:\n";
            status += "  Level: " + std::to_string(self.getLevel()) + "\n";
            status += "  Plasticity factor: " + std::to_string(self.getPlasticityFactor()) + "\n";
            return status;
        }, "Log dopamine status");

    // Curiosity
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator - exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset,
             "Reset curiosity")
        .def("logStatus", [](Curiosity& self) {
            std::string status = "Curiosity Status:\n";
            status += "  Level: " + std::to_string(self.getLevel()) + "\n";
            status += "  Exploration drive: " + std::to_string(self.getExplorationDrive()) + "\n";
            return status;
        }, "Log curiosity status");

    // Novelty
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator - novelty detection)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel,
             "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set novelty level")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& currentPattern, const std::vector<float>& previousPattern) {
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"),
           "Detect novelty from sensory input pattern")
        .def("update", &Novelty::update, py::arg("dt"),
             "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory,
             "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory,
             "Clear novelty history")
        .def("logStatus", [](Novelty& self) {
            std::string status = "Novelty Status:\n";
            status += "  Level: " + std::to_string(self.getLevel()) + "\n";
            status += "  History size: " + std::to_string(self.getHistory().size()) + "\n";
            return status;
        }, "Log novelty status");

    // PredictionError
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator - prediction error signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getError", &PredictionError::getError,
             "Get error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory,
             "Get error history")
        .def("clearHistory", &PredictionError::clearHistory,
             "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude,
             "Get error magnitude")
        .def("logStatus", [](PredictionError& self) {
            std::string status = "Prediction Error Status:\n";
            status += "  Error: " + std::to_string(self.getError()) + "\n";
            status += "  Magnitude: " + std::to_string(self.getMagnitude()) + "\n";
            status += "  History size: " + std::to_string(self.getHistory().size()) + "\n";
            return status;
        }, "Log prediction error status");

    // Reward
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward neuromodulator - reward signal)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue,
             "Get current reward value")
        .def("setValue", &Reward::setValue, py::arg("value"),
             "Set current reward value")
        .def("add", &Reward::add, py::arg("delta"),
             "Accumulate reward")
        .def("reset", &Reward::reset,
             "Reset accumulated reward")
        .def("getHistory", &Reward::getHistory,
             "Get reward history")
        .def("clearHistory", &Reward::clearHistory,
             "Clear reward history")
        .def("logStatus", [](Reward& self) {
            std::string status = "Reward Status:\n";
            status += "  Value: " + std::to_string(self.getValue()) + "\n";
            status += "  History size: " + std::to_string(self.getHistory().size()) + "\n";
            return status;
        }, "Log reward status");

    // PHONECONSTANTS: Placeholder for phase 2 implementations
    m.attr("NEUROMODULATION_PLACEHOLDER") = true;
    
    // Helper function to convert numpy-like arrays to vectors
    m.def("toVector", [](py::array_t<float> arr) {
        return std::vector<float>(arr.size(), 0.0f);
    }, py::arg("array"),
          "Convert numpy array to std::vector<float> (placeholder)");

    // Helper function to convert vectors to numpy-like arrays
    m.def("fromVector", [](const std::vector<float>& vec) {
        return py::array_t<float>(vec.size(), vec.data());
    }, py::arg("vector"),
          "Convert std::vector<float> to numpy array (placeholder)");

    // Memory system status helper functions
    m.def("formatMemoryStatus", [](const std::map<std::string, std::string>& status) {
        std::stringstream ss;
        ss << "MEMORY SYSTEMS STATUS:\n";
        for (const auto& pair : status) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
        return ss.str();
    }, py::arg("status"),
          "Format memory system status map as string");

    m.def("formatPredictionSystemStatus", [](const std::map<std::string, std::string>& status) {
        std::stringstream ss;
        ss << "PREDICTION SYSTEM STATUS:\n";
        for (const auto& pair : status) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
        return ss.str();
    }, py::arg("status"),
          "Format prediction system status map as string");

    m.def("formatCognitionSystemStatus", [](const std::map<std::string, std::string>& status) {
        std::stringstream ss;
        ss << "COGNITION SYSTEMS STATUS:\n";
        for (const auto& pair : status) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
        return ss.str();
    }, py::arg("status"),
          "Format cognition system status map as string");

    m.def("formatNeuromodulationSystemStatus", [](const std::map<std::string, std::string>& status) {
        std::stringstream ss;
        ss << "NEUROMODULATION SYSTEMS STATUS:\n";
        for (const auto& pair : status) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
        return ss.str();
    }, py::arg("status"),
          "Format neuromodulation system status map as string");

    // Error handling helper functions
    m.def("handleError", [](const std::string& message) {
        throw std::runtime_error("NLM Error: " + message);
    }, py::arg("message"),
          "Throw a runtime error with NLM prefix");

    m.def("validateVector", [](const std::vector<float>& vec, size_t expectedSize) {
        if (vec.size() != expectedSize) {
            throw std::invalid_argument("Vector size mismatch: expected " + std::to_string(expectedSize) +
                                      ", got " + std::to_string(vec.size()));
        }
        return true;
    }, py::arg("vector"), py::arg("expectedSize"),
          "Validate vector size (throw if invalid)");

    // Utility functions
    m.def("getVersion", []() {
        return std::string("NLM Python Bindings - Version 1.0 (Enhanced Memory & Cognition Systems)");
    }, "Get version string");

    m.def("getSupportedSystems", []() {
        return std::vector<std::string>({
            "Brain", "Config", "SimpleWorld", "AgentBrain",
            "AgentBody", "SensoryInput", "Vision", "Audio", "InternalSignals",
            "Action", "WorldObject", "ActionResult", "SensoryPercept",
            "NeuralWorkingMemory", "NeuralEpisodicMemory", "NeuralAssociativeMemory",
            "PredictionSystem", "NeuralPlanner", "ConceptFormation", "AttentionalSelection",
            "Neuromodulator", "Dopamine", "Curiosity", "Novelty", "PredictionError", "Reward",
            "EpisodicMemoryItem", "PlanningCandidate", "DiscoveredConcept", "ConceptInstance"
        });
    }, "Get list of all supported systems");

    // Constants and helper functions for common operations
    m.def("normalizeVector", [](const std::vector<float>& vec) {
        if (vec.empty()) return std::vector<float>();
        float norm = 0.0f;
        for (float v : vec) norm += v * v;
        norm = std::sqrt(norm);
        if (norm < 0.0001f) return std::vector<float>(vec.size(), 0.0f);
        
        std::vector<float> result(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            result[i] = vec[i] / norm;
        }
        return result;
    }, py::arg("vector"),
          "Normalize a vector to unit length (L2 norm)");

    m.def("dotProduct", [](const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size for dot product");
        }
        float result = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }, py::arg("a"), py::arg("b"),
          "Compute dot product of two vectors");

    m.def("cosineSimilarity", [](const std::vector<float>& a, const std::vector<float>& b) {
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }, py::arg("a"), py::arg("b"),
          "Compute cosine similarity between two vectors");

    m.def("clamp", [](float value, float min, float max) {
        return std::clamp(value, min, max);
    }, py::arg("value"), py::arg("min"), py::arg("max"),
          "Clamp a value between min and max");

    m.def("lerp", [](float a, float b, float t) {
        return a + (b - a) * t;
    }, py::arg("a"), py::arg("b"), py::arg("t"),
          "Linear interpolation between a and b with factor t");

    // Helper classes for Python integration
    py::class_<PyBrainContextManager>(m, "BrainContextManager", R"pbdoc(Context manager for Brain for use with 'with' statement)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("enter", &PyBrainContextManager::enter,
             "Enter the context (initialize brain)")
        .def("exit", &PyBrainContextManager::exit,
             "Exit the context (reset brain)")
        .def("getBrain", &PyBrainContextManager::getBrain,
             "Get the brain object from the context manager");

    // Function to create a context manager
    m.def("brainContext", [](std::shared_ptr<Brain> brain) {
        return PyBrainContextManager(brain);
    }, py::arg("brain"),
          "Create a brain context manager for use with 'with' statement");

    // Type conversion helper functions
    py::class_<py::object>(m, "PyObjectHelpers", R"pbdoc(Helper functions for Python object conversions)pbdoc")
        .def_static("toString", [](py::object obj) {
            return py::cast<std::string>(obj.attr("__str__")());
        }, py::arg("obj"),
            "Convert Python object to string");

    // Utility namespace for internal use
    m.def("nlm_version", []() { return "1.0"; });
}

} // namespace nlm