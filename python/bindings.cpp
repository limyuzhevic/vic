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

py::enum_<NeuromodulatorType>(m, "NeuromodulatorType", R"pbdoc(Neuromodulator type enumeration)pbdoc")
        .value("Dopamine", NeuromodulatorType::Dopamine)
        .value("Curiosity", NeuromodulatorType::Curiosity)
        .value("Novelty", NeuromodulatorType::Novelty)
        .value("PredictionError", NeuromodulatorType::PredictionError)
        .value("Reward", NeuromodulatorType::Reward)
        .value("Acetylcholine", NeuromodulatorType::Acetylcholine)
        .value("Norepinephrine", NeuromodulatorType::Norepinephrine)
        .value("Serotonin", NeuromodulatorType::Serotonin)
        .export_values();

    // Add Neuromodulator base class
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(
        m, "Neuromodulator", R"pbdoc(Base neuromodulator class)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    // Add all neuromodulator implementations
    py::class_<Dopamine, std::shared_ptr<Dopamine>, py::class_<Neuromodulator>>(
        m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"))
        .def("setPeak", &Dopamine::setPeak, py::arg("peak"))
        .def("setDecayRate", &Dopamine::setDecayRate, py::arg("rate"))
        .def("setReleaseRate", &Dopamine::setReleaseRate, py::arg("rate"));

    py::class_<Curiosity, std::shared_ptr<Curiosity>, py::class_<Neuromodulator>>(
        m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("getLevel", &Curiosity::getLevel)
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("getRecentDrive", &Curiosity::getRecentDrive)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &Curiosity::reset);

    py::class_<Novelty, std::shared_ptr<Novelty>, py::class_<Neuromodulator>>(
        m, "Novelty", R"pbdoc(Novelty neuromodulator for novelty detection)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"))
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("observation"), py::arg("previousObservation"))
        .def("detectNoveltyPattern", &Novelty::detectNovelty, py::arg("currentPattern"), py::arg("previousPattern"))
        .def("update", &Novelty::update, py::arg("dt"))
        .def("getHistory", &Novelty::getHistory)
        .def("clearHistory", &Novelty::clearHistory)
        .def("reset", &Novelty::reset);

    py::class_<PredictionError, std::shared_ptr<PredictionError>, py::class_<Neuromodulator>>(
        m, "PredictionError", R"pbdoc(Prediction error neuromodulator for learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"))
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude)
        .def("reset", &PredictionError::reset);

    py::class_<Reward, std::shared_ptr<Reward>, py::class_<Neuromodulator>>(
        m, "Reward", R"pbdoc(Reward neuromodulator for reinforcement signals)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("computeReward", &Reward::computeReward, py::arg("observation"))
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory);

    // Add prediction system
    py::class_<PredictionSystem, std::shared_ptr<PredictionSystem>>(
        m, "PredictionSystem", R"pbdoc(Prediction system for forecasting future states)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"))
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"))
        .def("train", &PredictionSystem::train, py::arg("observation"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("getErrorHistory", &PredictionSystem::getErrorHistory)
        .def("clearHistory", &PredictionSystem::clearHistory)
        .def("getAverageError", &PredictionSystem::getAverageError)
        .def("getMaxError", &PredictionSystem::getMaxError);

    // Add enhanced memory systems
    py::class_<WorkingMemory, std::shared_ptr<WorkingMemory>>(
        m, "WorkingMemory", R"pbdoc(Working memory for active information processing)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"))
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"))
        .def("contains", &WorkingMemory::contains, py::arg("neuron"))
        .def("clear", &WorkingMemory::clear)
        .def("getCapacity", &WorkingMemory::getCapacity)
        .def("getCurrentSize", &WorkingMemory::getCurrentSize)
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"))
        .def("getAverageActivation", &WorkingMemory::getAverageActivation)
        .def("getActiveNeurons", &WorkingMemory::getActiveNeurons)
        .def("getTotalActivation", &WorkingMemory::getTotalActivation);

    py::class_<EpisodicMemory, std::shared_ptr<EpisodicMemory>>(
        m, "EpisodicMemory", R"pbdoc(Episodic memory for event and experience storage)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"))
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("clear", &EpisodicMemory::clear)
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"))
        .def("getAllEpisodes", &EpisodicMemory::getAllEpisodes)
        .def("getRecentEpisodeCount", &EpisodicMemory::getRecentEpisodeCount)
        .def("getOldestEpisode", &EpisodicMemory::getOldestEpisode)
        .def("getMostRecentEpisode", &EpisodicMemory::getMostRecentEpisode)
        .def("reset", &EpisodicMemory::reset);

    py::class_<SemanticMemory, std::shared_ptr<SemanticMemory>>(
        m, "SemanticMemory", R"pbdoc(Semantic memory for facts and general knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"))
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"))
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"))
        .def("getAllFacts", &SemanticMemory::getAllFacts)
        .def("clear", &SemanticMemory::clear)
        .def("getFactCount", &SemanticMemory::getFactCount)
        .def("getFactByKey", &SemanticMemory::getFactByKey, py::arg("key"))
        .def("findFactsByValue", &SemanticMemory::findFactsByValue, py::arg("substring"))
        .def("removeFact", &SemanticMemory::removeFact, py::arg("key"));

    py::class_<ProceduralMemory, std::shared_ptr<ProceduralMemory>>(
        m, "ProceduralMemory", R"pbdoc(Procedural memory for skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"))
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"))
        .def("getAllSkills", &ProceduralMemory::getAllSkills)
        .def("updateProficiency", &ProceduralMemory::updateProficiency,
             py::arg("name"), py::arg("delta"))
        .def("clear", &ProceduralMemory::clear)
        .def("getSkillCount", &ProceduralMemory::getSkillCount)
        .def("getSkillByName", &ProceduralMemory::getSkillByName, py::arg("name"))
        .def("deleteSkill", &ProceduralMemory::deleteSkill, py::arg("name"))
        .def("hasSkill", &ProceduralMemory::hasSkill, py::arg("name"));

    py::class_<AssociativeMemory, std::shared_ptr<AssociativeMemory>>(
        m, "AssociativeMemory", R"pbdoc(Associative memory for relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate,
             py::arg("a"), py::arg("b"), py::arg("strength"))
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"))
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength,
             py::arg("a"), py::arg("b"))
        .def("updateAssociation", &AssociativeMemory::updateAssociation,
             py::arg("a"), py::arg("b"), py::arg("delta"))
        .def("clear", &AssociativeMemory::clear)
        .def("getAssociationCount", &AssociativeMemory::getAssociationCount)
        .def("getAssociativePartners", &AssociativeMemory::getAssociativePartners, py::arg("neuron"))
        .def("removeAssociation", &AssociativeMemory::removeAssociation,
             py::arg("a"), py::arg("b"));

    // Add advanced configuration options
    py::class_<BrainStats>(m, "BrainStats", R"pbdoc(Brain statistics and metrics)pbdoc")
        .def(py::init<>())
        .def("getTotalNeuronCount", &BrainStats::getTotalNeuronCount)
        .def("getTotalSynapseCount", &BrainStats::getTotalSynapseCount)
        .def("getActiveNeuronCount", &BrainStats::getActiveNeuronCount)
        .def("getFiringNeuronCount", &BrainStats::getFiringNeuronCount)
        .def("getAverageFiringRate", &BrainStats::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &BrainStats::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &BrainStats::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &BrainStats::getPendingSpikeEventCount)
        .def("getMemoryStats", &BrainStats::getMemoryStats)
        .def("getNeuromodulationStats", &BrainStats::getNeuromodulationStats)
        .def("getPredictionStats", &BrainStats::getPredictionStats)
        .def("getDevelopmentStats", &BrainStats::getDevelopmentStats);

    // Add experimental functions for advanced usage
    m.def("runAdvancedSimulation", &runAdvancedSimulation,
          py::arg("config"), py::arg("numSteps"), py::arg("worldEnabled") = true,
          "Run an advanced simulation with all subsystems enabled");

    m.def("runLearningExperiment", &runLearningExperiment,
          py::arg("config"), py::arg("experimentType"), py::arg("duration"),
          "Run a learning experiment with specified parameters");

    m.def("runDevelopmentExperiment", &runDevelopmentExperiment,
          py::arg("config"), py::arg("numStages"), py::arg("stepsPerStage"),
          "Run a developmental stage experiment");

    m.def("createDevelopmentConfig", &createDevelopmentConfig,
          py::arg("stages"), py::arg("plasticityRates"),
          "Create a development-specific configuration");

    m.def("createLearningConfig", &createLearningConfig,
          py::arg("enableReward"), py::arg("enableCuriosity"), py::arg("enableNovelty"),
          "Create a learning-optimized configuration");

    m.def("createExplorationConfig", &createExplorationConfig,
          py::arg("enableCuriosity"), py::arg("enableNovelty"), py::arg("enablePredictionError"),
          "Create an exploration-focused configuration");

    // Add statistics functions
    m.def("calculateBrainHealth", &calculateBrainHealth,
          py::arg("brain"), py::arg("timeWindow") = 100,
          "Calculate overall brain health score");

    m.def("analyzeLearningProgress", &analyzeLearningProgress,
          py::arg("brain"), py::arg("agent"), py::arg("epochs") = 10,
          "Analyze learning progress over time");

    m.def("compareBrains", &compareBrains,
          py::arg("brain1"), py::arg("brain2"), py::arg("metrics") = std::vector<std::string>(),
          "Compare two brains using specified metrics");

    // Add utility functions
    m.def("generateRandomConfig", &generateRandomConfig,
          py::arg("seed") = std::random_device{}(),
          "Generate a random configuration for experimentation");

    m.def("benchmarkPerformance", &benchmarkPerformance,
          py::arg("config"), py::arg("duration"), py::arg("numTrials") = 5,
          "Benchmark performance of the brain system");

    m.def("exportBrainState", &exportBrainState,
          py::arg("brain"), py::arg("filepath"), py::arg("format") = "json",
          "Export brain state to file");

    m.def("importBrainState", &importBrainState,
          py::arg("filepath"), py::arg("format") = "json",
          "Import brain state from file");

    m.def("validateConfiguration", &validateConfiguration,
          py::arg("config"), py::arg("strict") = false,
          "Validate configuration parameters");

    m.def("getSystemInfo", &getSystemInfo,
          "Get information about the NLM system configuration and capabilities");

    m.def("getVersionInfo", &getVersionInfo,
          "Get version information for NLM and its components");

    // Add helper functions for advanced users
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Apply development configuration
        config->set("development.enabled", true);
        config->set("development.maxStages", 5);
        config->set("development.startStage", "critical_period");
        
        // Enable learning subsystems
        config->set("neuromodulation.reward.enable", true);
        config->set("neuromodulation.curiosity.enable", true);
        config->set("neuromodulation.novelty.enable", true);
        config->set("neuromodulation.prediction_error.enable", true);
        
        // Enable plasticity
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        // Set reasonable defaults
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        
        return config;
    }, "Create a default configuration with development and learning enabled");

    m.def("createLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Focus on learning capabilities
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.2);
        config->set("brain.connection_probability", 0.1);
        
        // Enable all learning subsystems
        config->set("neuromodulation.reward.enable", true);
        config->set("neuromodulation.curiosity.enable", true);
        config->set("neuromodulation.novelty.enable", true);
        config->set("neuromodulation.prediction_error.enable", true);
        
        // Enhanced plasticity
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        // Memory systems
        config->set("memory.working.capacity", 500);
        config->set("memory.episodic.max_episodes", 2000);
        config->set("memory.associative.max_associations", 5000);
        
        // Development
        config->set("development.enabled", true);
        config->set("development.maxStages", 10);
        
        return config;
    }, "Create a learning-optimized configuration");

    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Focus on exploration and curiosity
        config->set("brain.neuron_count", 3000);
        config->set("brain.synapse_density", 0.15);
        config->set("brain.connection_probability", 0.08);
        
        // Curiosity and novelty focused
        config->set("neuromodulation.reward.enable", false);
        config->set("neuromodulation.curiosity.enable", true);
        config->set("neuromodulation.curiosity.novelty_weight", 0.7f);
        config->set("neuromodulation.curiosity.prediction_error_weight", 0.3f);
        config->set("neuromodulation.novelty.enable", true);
        config->set("neuromodulation.prediction_error.enable", true);
        
        // STDP with curiosity modulation
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.curiosity_modulation", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        // Less development plasticity
        config->set("development.enabled", true);
        config->set("development.maxStages", 5);
        config->set("development.startStage", "maturation");
        
        return config;
    }, "Create an exploration-focused configuration");

    // Add advanced configuration methods for Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class with enhanced features)pbdoc")
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
        })
        // Advanced configuration methods
        .def("setFloat", [](Config& self, const std::string& key, float value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set a float configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set an integer configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set a string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set a boolean configuration value")
        .def("setVector", [](Config& self, const std::string& key, const std::vector<float>& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set a vector configuration value")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<float>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a float configuration value with default")
        .def("getInt", [](Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get an integer configuration value with default")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a string configuration value with default")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a boolean configuration value with default")
        .def("getVector", [](Config& self, const std::string& key, const std::vector<float>& defaultValue) {
            auto value = self.get<std::vector<float>>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a vector configuration value with default")
        .def("setSource", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set a configuration value with specific source")
        .def("getSource", &Config::getSource, py::arg("key"),
             "Get the source of a configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getAllSources", &Config::getAllSources,
             "Get all configuration sources")
        .def("validate", &Config::validate, py::arg("schema"),
             "Validate configuration against a schema")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration")
        .def("clone", &Config::clone,
             "Clone the configuration");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
