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

    // ========== ADVANCED SYSTEM ACCESSORS FOR POWER USERS ==========
    
    // Memory system statistics
        .def("getWorkingMemoryStats", [](const Brain& self) {
            auto* wm = self.getWorkingMemory();
            if (!wm) return py::dict("error", "Working memory not available");
            
            return py::dict(
                "capacity", wm->getCapacity(),
                "active_traces", wm->getActiveTraces(),
                "trace_efficiency", wm->getActiveTraces() / std::max(1.0f, wm->getCapacity()),
                "memory_utilization", wm->getMemoryUtilization(),
                "decay_rate", wm->getTraceDecay(),
                "competition_strength", wm->getCompetitionStrength()
            );
        })
        .def("getEpisodicMemoryStats", [](const Brain& self) {
            auto* em = self.getEpisodicMemory();
            if (!em) return py::dict("error", "Episodic memory not available");
            
            return py::dict(
                "max_episodes", em->getMaxEpisodes(),
                "current_episodes", em->getEpisodeCount(),
                "memory_efficiency", em->getMemoryEfficiency(),
                "replay_frequency", em->getReplayFrequency(),
                "episode_strength", em->getEpisodeStrength(),
                "consolidation_rate", em->getConsolidationRate()
            );
        })
        .def("getAssociativeMemoryStats", [](const Brain& self) {
            auto* am = self.getAssociativeMemory();
            if (!am) return py::dict("error", "Associative memory not available");
            
            return py::dict(
                "similarity_threshold", am->getSimilarityThreshold(),
                "association_strength", am->getAssociationStrength(),
                "pattern_count", am->getPatternCount(),
                "learning_rate", am->getLearningRate(),
                "abstraction_depth", am->getAbstractionDepth(),
                "pattern_stability", am->getPatternStability()
            );
        })
        
        // Neuromodulation system statistics
        .def("getDopamineStats", [](const Brain& self) {
            auto* dopamine = self.getDopamine();
            if (!dopamine) return py::dict("error", "Dopamine system not available");
            
            return py::dict(
                "level", dopamine->getLevel(),
                "plasticity_factor", dopamine->getPlasticityFactor(),
                "learning_modulation", dopamine->getLearningModulation(),
                "reward_prediction_error", dopamine->getPredictionError(),
                "reward_sensitivity", dopamine->getRewardSensitivity(),
                "salience_detection", dopamine->getSalienceDetection()
            );
        })
        .def("getCuriosityStats", [](const Brain& self) {
            auto* curiosity = self.getCuriosity();
            if (!curiosity) return py::dict("error", "Curiosity system not available");
            
            return py::dict(
                "level", curiosity->getLevel(),
                "exploration_drive", curiosity->getExplorationDrive(),
                "novelty_sensitivity", curiosity->getNoveltySensitivity(),
                "curiosity_decay", curiosity->getCuriosityDecay(),
                "uncertainty_handling", curiosity->getUncertaintyHandling(),
                "intrinsic_motivation", curiosity->getIntrinsicMotivation()
            );
        })
        .def("getNoveltyStats", [](const Brain& self) {
            auto* novelty = self.getNovelty();
            if (!novelty) return py::dict("error", "Novelty system not available");
            
            return py::dict(
                "level", novelty->getLevel(),
                "detection_threshold", novelty->getDetectionThreshold(),
                "adaptation_rate", novelty->getAdaptationRate(),
                "surprise_sensitivity", novelty->getSurpriseSensitivity(),
                "pattern_detection", novelty->getPatternDetection(),
                "change_sensitivity", novelty->getChangeSensitivity()
            );
        })
        
        // Development system statistics
        .def("getDevelopmentStats", [](const Brain& self) {
            auto* dev = self.getDevelopmentSystem();
            if (!dev) return py::dict("error", "Development system not available");
            
            return py::dict(
                "stage", static_cast<int>(self.getDevelopmentalStage()),
                "plasticity_rate", dev->getPlasticityRate(),
                "age", dev->getAge(),
                "critical_period_end", dev->getCriticalPeriodEnd(),
                "maturation_progress", dev->getMaturationProgress(),
                "developmental_stability", dev->getDevelopmentalStability(),
                "adaptive_capacity", dev->getAdaptiveCapacity()
            );
        })
        .def("getDevelopmentStageName", [](const Brain& self) {
            switch (self.getDevelopmentalStage()) {
                case DevelopmentalStage::Initial: return "Initial";
                case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
                case DevelopmentalStage::Maturation: return "Maturation";
                case DevelopmentalStage::Adult: return "Adult";
                case DevelopmentalStage::Aging: return "Aging";
                default: return "Unknown";
            }
        })
        
        // Cognition system statistics
        .def("getPlannerStats", [](const Brain& self) {
            auto* planner = self.getPlanner();
            if (!planner) return py::dict("error", "Planner system not available");
            
            return py::dict(
                "planning_depth", planner->getPlanningDepth(),
                "action_selection_strength", planner->getActionSelectionStrength(),
                "temporal_horizon", planner->getTemporalHorizon(),
                "uncertainty_handling", planner->getUncertaintyHandling(),
                "goal_formation_strength", planner->getGoalFormationStrength(),
                "cognitive_flexibility", planner->getCognitiveFlexibility()
            );
        })
        .def("getConceptFormationStats", [](const Brain& self) {
            auto* cf = self.getConceptFormation();
            if (!cf) return py::dict("error", "Concept formation system not available");
            
            return py::dict(
                "concept_count", cf->getConceptCount(),
                "abstraction_level", cf->getAbstractionLevel(),
                "pattern_complexity", cf->getPatternComplexity(),
                "learning_capacity", cf->getLearningCapacity(),
                "concept_stability", cf->getConceptStability(),
                "pattern_recall_efficiency", cf->getPatternRecallEfficiency()
            );
        })
        .def("getAttentionStats", [](const Brain& self) {
            auto* attention = self.getAttention();
            if (!attention) return py::dict("error", "Attention system not available");
            
            return py::dict(
                "focus_strength", attention->getFocusStrength(),
                "inhibition_strength", attention->getInhibitionStrength(),
                "excitation_strength", attention->getExcitationStrength(),
                "attentional_biases", attention->getAttentionalBiases(),
                "selection_efficiency", attention->getSelectionEfficiency(),
                "gating_strength", attention->getGatingStrength()
            );
        })
        
        // Prediction system statistics
        .def("getPredictionStats", [](const Brain& self) {
            auto* prediction = self.getPredictionSystem();
            if (!prediction) return py::dict("error", "Prediction system not available");
            
            return py::dict(
                "prediction_accuracy", prediction->getAccuracy(),
                "forward_model_confidence", prediction->getConfidence(),
                "error_correction_rate", prediction->getErrorCorrectionRate(),
                "prediction_horizon", prediction->getPredictionHorizon(),
                "predictive_efficiency", prediction->getPredictiveEfficiency(),
                "model_stability", prediction->getModelStability()
            );
        })
        
        // Composite system analysis
        .def("getBrainHealthMetrics", [](const Brain& self) {
            auto metrics = py::dict();
            
            // Memory systems
            metrics["working_memory_integrated"] = (self.getWorkingMemory() != nullptr);
            metrics["episodic_memory_integrated"] = (self.getEpisodicMemory() != nullptr);
            metrics["associative_memory_integrated"] = (self.getAssociativeMemory() != nullptr);
            
            // Neuromodulation systems
            metrics["dopamine_active"] = (self.getDopamine() != nullptr);
            metrics["curiosity_active"] = (self.getCuriosity() != nullptr);
            metrics["novelty_active"] = (self.getNovelty() != nullptr);
            
            // Cognition systems
            metrics["planner_active"] = (self.getPlanner() != nullptr);
            metrics["concept_formation_active"] = (self.getConceptFormation() != nullptr);
            metrics["attention_active"] = (self.getAttention() != nullptr);
            
            // Other systems
            metrics["development_active"] = (self.getDevelopmentSystem() != nullptr);
            metrics["prediction_active"] = (self.getPredictionSystem() != nullptr);
            
            // Physical metrics
            metrics["total_neurons"] = self.getTotalNeuronCount();
            metrics["total_synapses"] = self.getTotalSynapseCount();
            metrics["avg_firing_rate"] = self.getAverageFiringRate();
            metrics["e_i_ratio"] = self.getExcitationInhibitionRatio();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            metrics["region_count"] = self.getRegionCount();
            
            // Calculate overall health score
            int system_count = 10; // Total systems to check
            int active_systems = std::count_if(metrics.begin(), metrics.end(),
                [](const auto& pair) {
                    return pair.second.cast<bool>().value();
                });
            metrics["overall_health_score"] = (active_systems * 100) / system_count;
            
            return metrics;
        })
        
        .def("analyzeSystemIntegration", [](const Brain& self) {
            auto analysis = py::dict();
            
            // Memory integration
            auto* wm = self.getWorkingMemory();
            auto* em = self.getEpisodicMemory();
            auto* am = self.getAssociativeMemory();
            
            analysis["memory_system_strength"] = (wm ? wm->getEfficiency() : 0.0f) * 0.3 +
                                                (em ? em->getMemoryEfficiency() : 0.0f) * 0.4 +
                                                (am ? am->getAssociationStrength() : 0.0f) * 0.3;
            
            // Neuromodulation integration
            auto* dopamine = self.getDopamine();
            auto* curiosity = self.getCuriosity();
            auto* novelty = self.getNovelty();
            
            float neuromodulation_strength = 0.0f;
            if (dopamine) neuromodulation_strength += dopamine->getLevel() * 0.4f;
            if (curiosity) neuromodulation_strength += curiosity->getLevel() * 0.3f;
            if (novelty) neuromodulation_strength += novelty->getLevel() * 0.3f;
            analysis["neuromodulation_strength"] = neuromodulation_strength;
            
            // Cognitive integration
            auto* planner = self.getPlanner();
            auto* cf = self.getConceptFormation();
            auto* attention = self.getAttention();
            
            float cognitive_strength = 0.0f;
            if (planner) cognitive_strength += planner->getActionSelectionStrength() * 0.4f;
            if (cf) cognitive_strength += cf->getAbstractionLevel() * 0.3f;
            if (attention) cognitive_strength += attention->getFocusStrength() * 0.3f;
            analysis["cognitive_strength"] = cognitive_strength;
            
            // Development integration
            auto* dev = self.getDevelopmentSystem();
            analysis["developmental_maturation"] = dev ? dev->getMaturationProgress() : 0.0f;
            
            // Overall system coherence
            float coherence = (analysis["memory_system_strength"].cast<float>().value() * 0.3f +
                              analysis["neuromodulation_strength"].cast<float>().value() * 0.3f +
                              analysis["cognitive_strength"].cast<float>().value() * 0.3f +
                              analysis["developmental_maturation"].cast<float>().value() * 0.1f);
            analysis["system_coherence"] = coherence;
            
            // Integration quality indicators
            analysis["integration_quality"] = "Excellent";
            if (coherence < 0.3f) analysis["integration_quality"] = "Poor";
            else if (coherence < 0.6f) analysis["integration_quality"] = "Fair";
            else if (coherence < 0.8f) analysis["integration_quality"] = "Good";
            
            return analysis;
        })
        
        .def("exportAnalysisReport", [](const Brain& self, const std::string& format) {
            auto metrics = self.getBrainHealthMetrics();
            auto analysis = self.analyzeSystemIntegration();
            
            if (format == "json") {
                std::ostringstream json;
                json << "{\n";
                json << "  \"brain_metrics\": {\n";
                for (auto it = metrics.begin(); it != metrics.end(); ++it) {
                    if (it != metrics.begin()) json << ",\n";
                    json << "    \"" << it->first << "\": ";
                    if (it->second.is_none()) {
                        json << "null";
                    } else if (it->second.is_boolean()) {
                        json << (it->second.cast<bool>().value() ? "true" : "false");
                    } else if (it->second.is_floating_point()) {
                        json << it->second.cast<float>().value();
                    } else if (it->second.is_integer()) {
                        json << it->second.cast<int>().value();
                    } else {
                        json << \"\" << it->second.cast<std::string>().value() << "\"\";
                    }
                }
                json << "\n  },\n";
                json << "  \"system_analysis\": {\n";
                for (auto it = analysis.begin(); it != analysis.end(); ++it) {
                    if (it != analysis.begin()) json << ",\n";
                    json << "    \"" << it->first << "\": ";
                    if (it->second.is_string()) {
                        json << "\"" << it->second.cast<std::string>().value() << "\"";
                    } else if (it->second.is_floating_point()) {
                        json << it->second.cast<float>().value();
                    } else {
                        json << "null";
                    }
                }
                json << "\n  },\n";
                json << "  \"summary\" : \"Phase 6 integrated artificial brain with " << metrics["total_neurons"].cast<int>().value() << " neurons and " << metrics["total_synapses"].cast<int>().value() << " synapses. Health score: " << metrics["overall_health_score"].cast<int>().value() << "%.\"\n";
                json << "}\n";
                return json.str();
                
            } else if (format == "text") {
                std::ostringstream text;
                text << "=== NLM Brain Analysis Report ===\n\n";
                text << "Brain Health Metrics:\n";
                for (auto it = metrics.begin(); it != metrics.end(); ++it) {
                    text << "  " << it->first << ": ";
                    if (it->second.is_none()) {
                        text << "N/A\n";
                    } else if (it->second.is_boolean()) {
                        text << (it->second.cast<bool>().value() ? "Active" : "Inactive") << "\n";
                    } else if (it->second.is_floating_point()) {
                        text << std::fixed << std::setprecision(2) << it->second.cast<float>().value() << "\n";
                    } else if (it->second.is_integer()) {
                        text << it->second.cast<int>().value() << "\n";
                    }
                }
                
                text << "\nSystem Integration Analysis:\n";
                for (auto it = analysis.begin(); it != analysis.end(); ++it) {
                    text << "  " << it->first << ": ";
                    if (it->second.is_string()) {
                        text << it->second.cast<std::string>().value() << "\n";
                    } else if (it->second.is_floating_point()) {
                        text << std::fixed << std::setprecision(2) << it->second.cast<float>().value() << "\n";
                    } else {
                        text << "N/A\n";
                    }
                }
                
                text << "\nOverall Integration Quality: " << analysis["integration_quality"].cast<std::string>().value() << "\n";
                text << "System Coherence Score: " << std::fixed << std::setprecision(1) << analysis["system_coherence"].cast<float>().value() * 10.0f << "/10\n";
                text << "\nRecommendation: ";
                if (analysis["system_coherence"].cast<float>().value() < 0.5f) {
                    text << "System integration needs improvement. Consider adjusting neuromodulation parameters or enhancing memory system connections.\n";
                } else {
                    text << "System integration is healthy and well-coordinated. Continue with current configuration.\n";
                }
                
                return text.str();
            }
            
            return std::string("Unsupported format: " + format);
        }, py::arg("format") = "text")

    // ========== ADVANCED BRAIN ACCESSORS FOR POWER USERS ==========
    
    // Memory system accessors with advanced statistics
        .def("getWorkingMemoryStats", [](const Brain& self) {
            auto* wm = self.getWorkingMemory();
            return py::dict(
                "capacity", wm ? wm->getCapacity() : 0,
                "active_traces", wm ? wm->getActiveTraces() : 0,
                "efficiency", wm ? wm->getEfficiency() : 0.0f,
                "trace_decay", wm ? wm->getTraceDecay() : 0.0f
            );
        }, "Get working memory statistics")
        .def("getEpisodicMemoryStats", [](const Brain& self) {
            auto* em = self.getEpisodicMemory();
            return py::dict(
                "max_episodes", em ? em->getMaxEpisodes() : 0,
                "current_episodes", em ? em->getEpisodeCount() : 0,
                "memory_strength", em ? em->getMemoryStrength() : 0.0f,
                "replay_frequency", em ? em->getReplayFrequency() : 0.0f
            );
        }, "Get episodic memory statistics")
        .def("getAssociativeMemoryStats", [](const Brain& self) {
            auto* am = self.getAssociativeMemory();
            return py::dict(
                "similarity_threshold", am ? am->getSimilarityThreshold() : 0.0f,
                "association_strength", am ? am->getAssociationStrength() : 0.0f,
                "pattern_count", am ? am->getPatternCount() : 0,
                "learning_rate", am ? am->getLearningRate() : 0.0f
            );
        }, "Get associative memory statistics")
        
        // Neuromodulation system accessors
        .def("getDopamineStats", [](const Brain& self) {
            auto* dopamine = self.getDopamine();
            return py::dict(
                "level", dopamine ? dopamine->getLevel() : 0.0f,
                "plasticity_factor", dopamine ? dopamine->getPlasticityFactor() : 1.0f,
                "learning_modulation", dopamine ? dopamine->getLearningModulation() : 1.0f,
                "reward_prediction_error", dopamine ? dopamine->getPredictionError() : 0.0f
            );
        }, "Get dopamine neuromodulation statistics")
        .def("getCuriosityStats", [](const Brain& self) {
            auto* curiosity = self.getCuriosity();
            return py::dict(
                "level", curiosity ? curiosity->getLevel() : 0.0f,
                "exploration_drive", curiosity ? curiosity->getExplorationDrive() : 0.0f,
                "novelty_sensitivity", curiosity ? curiosity->getNoveltySensitivity() : 0.0f,
                "curiosity_decay", curiosity ? curiosity->getCuriosityDecay() : 0.0f
            );
        }, "Get curiosity neuromodulation statistics")
        .def("getNoveltyStats", [](const Brain& self) {
            auto* novelty = self.getNovelty();
            return py::dict(
                "level", novelty ? novelty->getLevel() : 0.0f,
                "detection_threshold", novelty ? novelty->getDetectionThreshold() : 0.0f,
                "adaptation_rate", novelty ? novelty->getAdaptationRate() : 0.0f,
                "surprise_sensitivity", novelty ? novelty->getSurpriseSensitivity() : 0.0f
            );
        }, "Get novelty neuromodulation statistics")
        
        // Development system accessors
        .def("getDevelopmentStats", [](const Brain& self) {
            auto* dev = self.getDevelopmentSystem();
            return py::dict(
                "stage", static_cast<int>(self.getDevelopmentalStage()),
                "plasticity_rate", dev ? dev->getPlasticityRate() : 1.0f,
                "age", dev ? dev->getAge() : 0,
                "critical_period_end", dev ? dev->getCriticalPeriodEnd() : 0,
                "maturation_progress", dev ? dev->getMaturationProgress() : 0.0f
            );
        }, "Get development system statistics")
        
        // Cognition system accessors
        .def("getPlannerStats", [](const Brain& self) {
            auto* planner = self.getPlanner();
            return py::dict(
                "planning_depth", planner ? planner->getPlanningDepth() : 0,
                "action_selection_strength", planner ? planner->getActionSelectionStrength() : 0.0f,
                "temporal_horizon", planner ? planner->getTemporalHorizon() : 0.0f,
                "uncertainty_handling", planner ? planner->getUncertaintyHandling() : 0.0f
            );
        }, "Get neural planner statistics")
        .def("getConceptFormationStats", [](const Brain& self) {
            auto* cf = self.getConceptFormation();
            return py::dict(
                "concept_count", cf ? cf->getConceptCount() : 0,
                "abstraction_level", cf ? cf->getAbstractionLevel() : 0.0f,
                "pattern_complexity", cf ? cf->getPatternComplexity() : 0.0f,
                "learning_capacity", cf ? cf->getLearningCapacity() : 0.0f
            );
        }, "Get concept formation statistics")
        .def("getAttentionStats", [](const Brain& self) {
            auto* attention = self.getAttention();
            return py::dict(
                "focus_strength", attention ? attention->getFocusStrength() : 0.0f,
                "inhibition_strength", attention ? attention->getInhibitionStrength() : 0.0f,
                "excitation_strength", attention ? attention->getExcitationStrength() : 0.0f,
                "attentional_biases", attention ? attention->getAttentionalBiases() : std::vector<float>()
            );
        }, "Get attentional selection statistics")
        
        // Prediction system accessors
        .def("getPredictionStats", [](const Brain& self) {
            auto* prediction = self.getPredictionSystem();
            return py::dict(
                "prediction_accuracy", prediction ? prediction->getAccuracy() : 0.0f,
                "forward_model_confidence", prediction ? prediction->getConfidence() : 0.0f,
                "error_correction_rate", prediction ? prediction->getErrorCorrectionRate() : 0.0f,
                "prediction_horizon", prediction ? prediction->getPredictionHorizon() : 0.0f
            );
        }, "Get prediction system statistics")
        
        // Composite system analysis
        .def("getBrainHealthMetrics", [](const Brain& self) {
            return py::dict(
                "memory_integration", (self.getWorkingMemory() != nullptr && self.getEpisodicMemory() != nullptr),
                "neuromodulation_active", (self.getDopamine() != nullptr && self.getCuriosity() != nullptr),
                "cognition_integrated", (self.getPlanner() != nullptr && self.getConceptFormation() != nullptr),
                "development_active", (self.getDevelopmentSystem() != nullptr),
                "prediction_functional", (self.getPredictionSystem() != nullptr),
                "total_neurons", self.getTotalNeuronCount(),
                "total_synapses", self.getTotalSynapseCount(),
                "avg_firing_rate", self.getAverageFiringRate(),
                "e_i_ratio", self.getExcitationInhibitionRatio(),
                "total_spikes", self.getTotalSpikeCount()
            );
        }, "Get comprehensive brain health metrics")
        
        .def("analyzeSystemIntegration", [](const Brain& self) {
            std::unordered_map<std::string, bool> integration_status;
            integration_status["memory_working"] = (self.getWorkingMemory() != nullptr);
            integration_status["memory_episodic"] = (self.getEpisodicMemory() != nullptr);
            integration_status["neuromodulation_dopamine"] = (self.getDopamine() != nullptr);
            integration_status["neuromodulation_curiosity"] = (self.getCuriosity() != nullptr);
            integration_status["neuromodulation_novelty"] = (self.getNovelty() != nullptr);
            integration_status["prediction"] = (self.getPredictionSystem() != nullptr);
            integration_status["planner"] = (self.getPlanner() != nullptr);
            integration_status["concept_formation"] = (self.getConceptFormation() != nullptr);
            integration_status["attention"] = (self.getAttention() != nullptr);
            integration_status["development"] = (self.getDevelopmentSystem() != nullptr);
            
            return py::dict(
                "working_memory_integrated", integration_status["memory_working"],
                "episodic_memory_integrated", integration_status["memory_episodic"],
                "neuromodulation_active", (integration_status["neuromodulation_dopamine"] && integration_status["neuromodulation_curiosity"]),
                "cognition_integrated", (integration_status["planner"] && integration_status["concept_formation"]),
                "development_active", integration_status["development"],
                "prediction_functional", integration_status["prediction"],
                "overall_health_score", 
                    (static_cast<float>(std::count_if(integration_status.begin(), integration_status.end(),
                        [](const auto& pair) { return pair.second; })) / integration_status.size()) * 100.0f
            );
        }, "Analyze overall system integration and health")
        
        .def("getConnectivityAnalysis", [](const Brain& self) {
            return py::dict(
                "total_neurons", self.getTotalNeuronCount(),
                "total_synapses", self.getTotalSynapseCount(),
                "connection_density", self.getTotalNeuronCount() > 0 ? 
                    static_cast<float>(self.getTotalSynapseCount()) / self.getTotalNeuronCount() : 0.0f,
                "region_count", self.getRegionCount(),
                "avg_neurons_per_region", self.getRegionCount() > 0 ? 
                    static_cast<float>(self.getTotalNeuronCount()) / self.getRegionCount() : 0.0f,
                "sensory_neurons", std::count_if(self.getRegions().begin(), self.getRegions().end(),
                    [](const auto& region) {
                        // This is a simplified check - actual implementation would need region type access
                        return true; // placeholder
                    }),
                "motor_neurons", 0, // placeholder
                "internal_neurons", self.getTotalNeuronCount() // placeholder
            );
        }, "Get connectivity analysis and statistics")
        
        .def("exportAnalysisReport", [](const Brain& self, const std::string& format) {
            auto metrics = self.getBrainHealthMetrics();
            std::ostringstream report;
            
            if (format == "json") {
                report << "{\n";
                report << "  \"brain_metrics\": {\n";
                for (auto it = metrics.begin(); it != metrics.end(); ++it) {
                    report << "    \"" << it->first << "\": " << std::boolalpha << it->second << ",\n";
                }
                report << "  },\n";
                report << "  \"summary\" : \"Phase 6 integrated artificial brain with " << self.getTotalNeuronCount() << " neurons and " << self.getTotalSynapseCount() << " synapses.\n";
                report << "}\n";
            } else if (format == "text") {
                report << "=== NLM Brain Analysis Report ===\n";
                report << "Brain Health Metrics:\n";
                for (auto it = metrics.begin(); it != metrics.end(); ++it) {
                    report << "  " << it->first << ": " << std::boolalpha << it->second << "\n";
                }
                report << "\nIntegration Status: " << static_cast<int>(py::cast<>(metrics["overall_health_score"]) / 20.0f) << "% healthy\n";
            }
            
            return report.str();
        }, py::arg("format") = "text", "Generate comprehensive analysis report in specified format")

    // ========== ADVANCED BRAIN CONTROLS FOR POWER USERS ==========
    
    // Custom plasticity rules
    .def("applyCustomPlasticity", &Brain::applyCustomPlasticity,
              py::arg("rule_type"), py::arg("parameters"),
              "Apply custom plasticity rule (STDP, Hebbian, Reward-modulated)")
        .def("setPlasticityParameters", &Brain::setPlasticityParameters,
              py::arg("stdp_ltp"), py::arg("stdp_ltd"), py::arg("hebbian_strength") = 0.01f,
              "Set custom plasticity parameters")
        
        // Advanced simulation controls
        .def("runSimulationLoop", &Brain::runSimulationLoop,
              py::arg("start_step"), py::arg("end_step"), py::arg("max_time_ms") = 1000.0,
              "Run complete simulation loop with progress monitoring")
        .def("pauseSimulation", &Brain::pauseSimulation,
              "Pause ongoing simulation")
        .def("resumeSimulation", &Brain::resumeSimulation,
              "Resume paused simulation")
        .def("isSimulationRunning", &Brain::isSimulationRunning,
              "Check if simulation is currently running")
        
        // Memory management and checkpointing
        .def("createCheckpoint", &Brain::save,
              py::arg("checkpoint_path"),
              "Create brain checkpoint for state saving")
        .def("loadCheckpoint", &Brain::load,
              py::arg("checkpoint_path"),
              "Load brain state from checkpoint")
        .def("getCheckpointInfo", &Brain::getCheckpointInfo,
              "Get information about latest checkpoint")
        .def("cleanupCheckpoints", &Brain::cleanupCheckpoints,
              py::arg("keep_count") = 5,
              "Clean up old checkpoint files")
        
        // Advanced monitoring and analytics
        .def("getPerformanceMetrics", &Brain::getPerformanceMetrics,
              "Get comprehensive performance metrics")
        .def("getNeuralActivityStats", &Brain::getNeuralActivityStats,
              "Get statistics about neural activity patterns")
        .def("analyzeSpikingPatterns", &Brain::analyzeSpikingPatterns,
              py::arg("window_steps"),
              "Analyze spiking patterns over time window")
        .def("detectAnomalies", &Brain::detectAnomalies,
              "Detect anomalous activity patterns")
        
        // Export and import utilities
        .def("exportBrainState", &Brain::exportBrainState,
              py::arg("format") = "binary",
              "Export brain state in various formats (binary, json, text)")
        .def("importBrainState", &Brain::importBrainState,
              py::arg("data"), py::arg("format") = "binary",
              "Import brain state from various formats")
        .def("serializeBrainState", &Brain::serializeBrainState,
              "Serialize brain state to string for transmission")
        .def("deserializeBrainState", &Brain::deserializeBrainState,
              py::arg("serialized_data"),
              "Deserialize brain state from string")
        
        // Development and debugging tools
        .def("getDevelopmentalStats", &Brain::getDevelopmentalStats,
              "Get current developmental statistics")
        .def("setDevelopmentStage", &Brain::setDevelopmentStage,
              py::arg("stage"), py::arg("force_immediate") = false,
              "Set development stage with optional immediate effects")
        .def("getNeuromodulationState", &Brain::getNeuromodulationState,
              "Get current neuromodulation state")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
              py::arg("modulator_type"), py::arg("level"),
              "Apply neuromodulatory signal (dopamine, curiosity, novelty)")
        
        // Batch processing and parallel operations
        .def("processBatchStimulus", &Brain::processBatchStimulus,
              py::arg("stimuli_list"),
              "Process multiple stimulus patterns in batch")
        .def("runMultiRegionSimulation", &Brain::runMultiRegionSimulation,
              py::arg("region_configs"),
              "Run synchronized simulation across multiple regions")
        .def("getCrossRegionConnectivity", &Brain::getCrossRegionConnectivity,
              "Get connectivity statistics between regions")
        
        // Advanced configuration and scripting
        .def("getConfigurationSchema", &Brain::getConfigurationSchema,
              "Get configuration schema and validation rules")
        .def("validateConfiguration", &Brain::validateConfiguration,
              py::arg("config_dict"),
              "Validate configuration dictionary against schema")
        .def("applyConfigurationTemplate", &Brain::applyConfigurationTemplate,
              py::arg("template_name"),
              "Apply predefined configuration template")
        
        // Real-time control and monitoring
        .def("startRealTimeMonitoring", &Brain::startRealTimeMonitoring,
              py::arg("sampling_rate"), py::arg("callback_func"),
              "Start real-time monitoring with callback function")
        .def("stopRealTimeMonitoring", &Brain::stopRealTimeMonitoring,
              "Stop real-time monitoring")
        .def("getMonitoringData", &Brain::getMonitoringData,
              py::arg("duration_ms"),
              "Get monitoring data for specified duration")
        .def("setSimulationSpeed", &Brain::setSimulationSpeed,
              py::arg("speed_factor"),
              "Set simulation speed multiplier (0.1x to 10x)")
        
        // Learning and adaptation controls
        .def("enableLearning", &Brain::enableLearning,
              py::arg("enable"), py::arg("learning_rate") = 1.0f,
              "Enable/disable learning with custom learning rate")
        .def("getLearningStats", &Brain::getLearningStats,
              "Get statistics about learning progress")
        .def("resetLearning", &Brain::resetLearning,
              "Reset learning state and forget all adaptations")
        .def("applyTransferLearning", &Brain::applyTransferLearning,
              py::arg("source_brain"), py::arg("adaptation_strength"),
              "Apply transfer learning from another brain instance")

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
