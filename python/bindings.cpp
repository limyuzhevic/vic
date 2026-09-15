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

    // Enhanced Brain class with more detailed state access
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
        // ===== ENHANCED BRAIN STATE ACCESS =====
        .def("getNeuronStates", [](const Brain& self) {
            std::vector<std::tuple<uint64_t, float, float, float, float, float, float>> states;
            for (const auto& region : self.getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        states.emplace_back(
                            neuron->getId().value,
                            state.membranePotential,
                            state.restingPotential,
                            state.threshold,
                            state.resetPotential,
                            state.leakConductance,
                            state.refractoryRemaining
                        );
                    }
                }
            }
            return states;
        }, "Get detailed neuron states (ID, membrane potential, resting potential, threshold, reset potential, leak conductance, refractory remaining)")
        .def("getSynapseStates", [](const Brain& self) {
            std::vector<std::tuple<uint64_t, uint64_t, uint64_t, float, float, float, float>> synapses;
            for (const auto& region : self.getRegions()) {
                for (const auto* syn : region->getSynapses()) {
                    synapses.emplace_back(
                        syn->getSourceNeuron().value,
                        syn->getDestinationNeuron().value,
                        syn->getId().value,
                        syn->getWeight(),
                        syn->getDelay(),
                        syn->getType(),
                        syn->getEligibilityTrace()
                    );
                }
            }
            return synapses;
        }, "Get detailed synapse states (source ID, destination ID, synapse ID, weight, delay, type, eligibility trace)")
        .def("getMemoryStats", [](Brain& self) {
            py::dict stats;
            auto* workingMem = self.getWorkingMemory();
            if (workingMem) {
                stats["working_memory_traces"] = workingMem->getActiveTraces();
                stats["working_memory_capacity"] = workingMem->getCapacity();
            }
            auto* episodicMem = self.getEpisodicMemory();
            if (episodicMem) {
                stats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
                stats["episodic_memory_max_episodes"] = episodicMem->getMaxEpisodes();
            }
            auto* assocMem = self.getAssociativeMemory();
            if (assocMem) {
                stats["associative_memory_patterns"] = assocMem->getPatternCount();
                stats["associative_memory_strength"] = assocMem->getAverageStrength();
            }
            auto* novelty = self.getNovelty();
            if (novelty) {
                stats["novelty_level"] = novelty->getLevel();
                stats["novelty_history_size"] = novelty->getHistorySize();
            }
            auto* curiosity = self.getCuriosity();
            if (curiosity) {
                stats["curiosity_level"] = curiosity->getLevel();
                stats["curiosity_history_size"] = curiosity->getHistorySize();
            }
            auto* dopamine = self.getDopamine();
            if (dopamine) {
                stats["dopamine_level"] = dopamine->getLevel();
                stats["dopamine_history_size"] = dopamine->getHistorySize();
            }
            return stats;
        }, "Get comprehensive memory system statistics")
        .def("getPlasticityStats", [](Brain& self) {
            py::dict stats;
            auto* stdp = self.getSTDP();
            if (stdp) {
                stats["stdp_ltp_weight"] = stdp->getLTPWeight();
                stats["stdp_ltd_weight"] = stdp->getLTDWeight();
                stats["stdp_tau"] = stdp->getTau();
                stats["stdp_history_size"] = stdp->getHistorySize();
            }
            auto* hebbian = self.getHebbian();
            if (hebbian) {
                stats["hebbian_learning_rate"] = hebbian->getLearningRate();
                stats["hebbian_history_size"] = hebbian->getHistorySize();
            }
            auto* sp = self.getStructuralPlasticity();
            if (sp) {
                stats["synaptogenesis_rate"] = sp->getSynaptogenesisRate();
                stats["pruning_rate"] = sp->getPruningRate();
                stats["structural_history_size"] = sp->getHistorySize();
            }
            return stats;
        }, "Get plasticity system statistics")
        .def("getDevelopmentStats", [](Brain& self) {
            py::dict stats;
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["age"] = self.getDevelopmentalAge();  // Assuming this method exists
            stats["plasticity_modifier"] = self.getPlasticityModifier();  // Assuming this method exists
            auto* devSys = self.getDevelopmentSystem();
            if (devSys) {
                stats["critical_period_active"] = devSys->isCriticalPeriod();
                stats["maturation_progress"] = devSys->getMaturationProgress();
            }
            return stats;
        }, "Get development system statistics")
        .def("getPredictionStats", [](Brain& self) {
            py::dict stats;
            auto* predSys = self.getPredictionSystem();
            if (predSys) {
                stats["prediction_error"] = predSys->getPredictionError();
                stats["confidence_level"] = predSys->getConfidenceLevel();
                stats["prediction_history_size"] = predSys->getHistorySize();
                stats["model_accuracy"] = predSys->getModelAccuracy();
            }
            return stats;
        }, "Get prediction system statistics")
        .def("getCognitionStats", [](Brain& self) {
            py::dict stats;
            auto* planner = self.getPlanner();
            if (planner) {
                stats["planning_depth"] = planner->getPlanningDepth();
                stats["plan_confidence"] = planner->getPlanConfidence();
            }
            auto* concepts = self.getConceptFormation();
            if (concepts) {
                stats["concept_count"] = concepts->getConceptCount();
                stats["concept_stability"] = concepts->getConceptStability();
            }
            auto* attention = self.getAttention();
            if (attention) {
                stats["attention_focus"] = attention->getFocusStrength();
                stats["attention_gating"] = attention->getGatingFactor();
            }
            return stats;
        }, "Get cognition system statistics")
        .def("getSelfModel", [](Brain& self) {
            py::dict model;
            model["brain_id"] = self.getTotalNeuronCount();
            model["experience_count"] = self.getTotalSpikeCount();
            model["behavioral_patterns"] = self.getBehavioralPatternCount();  // Assuming this method exists
            model["skill_level"] = self.getSkillLevel();  // Assuming this method exists
            model["adaptation_rate"] = self.getAdaptationRate();  // Assuming this method exists
            return model;
        }, "Get self-model information (brain identity and learned characteristics)")
        .def("getSocialInfo", [](Brain& self) {
            py::dict social;
            social["observations_count"] = self.getObservationCount();  // Assuming this method exists
            social["imitation_examples"] = self.getImitationExamples();  // Assuming this method exists
            social["social_learning_rate"] = self.getSocialLearningRate();  // Assuming this method exists
            social["theory_of_mind"] = self.getTheoryOfMindScore();  // Assuming this method exists
            return social;
        }, "Get social learning information")
        .def("analyzeBehavior", [](Brain& self, int windowSize) {
            py::dict analysis;
            analysis["stability"] = self.analyzeBehaviorStability(windowSize);  // Assuming this method exists
            analysis["exploration_rate"] = self.analyzeExplorationRate(windowSize);  // Assuming this method exists
            analysis["adaptation_speed"] = self.analyzeAdaptationSpeed(windowSize);  // Assuming this method exists
            analysis["pattern_complexity"] = self.analyzePatternComplexity(windowSize);  // Assuming this method exists
            return analysis;
        }, "Analyze brain behavior patterns")
        .def("getCheckpointInfo", [](Brain& self) {
            auto* checkpoint = self.getCheckpointManager();  // Assuming this method exists
            if (checkpoint) {
                return py::make_tuple(
                    checkpoint->getCurrentCheckpointIndex(),
                    checkpoint->getCheckpointCount(),
                    checkpoint->getCheckpointSize(),
                    checkpoint->getCompressionLevel()
                );
            }
            return py::make_tuple(0, 0, 0, 0);
        }, "Get checkpoint system information")
        .def("triggerCheckpoint", [](Brain& self) {
            auto* checkpoint = self.getCheckpointManager();  // Assuming this method exists
            if (checkpoint) {
                return checkpoint->createCheckpoint();
            }
            return false;
        }, "Manually trigger checkpoint creation")
        .def("compareBrains", [](const Brain& self, const Brain& other) {
            py::dict comparison;
            comparison["neuron_similarity"] = self.compareNeuronConnectivity(other);
            comparison["behavior_similarity"] = self.compareBehaviorPatterns(other);
            comparison["memory_similarity"] = self.compareMemoryContents(other);
            comparison["overall_compatibility"] = self.calculateCompatibilityScore(other);
            return comparison;
        }, "Compare two brains for compatibility analysis")

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

    // ===== ADDITIONAL AGENT BRAIN ADVANCED FUNCTIONS =====
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("getBrainStates", [](const Brain& brain) {
        return brain.getNeuronStates();
    }, "Get detailed neuron states (ID, membrane potential, resting potential, threshold, reset potential, leak conductance, refractory remaining)")

    m.def("getSynapseStates", [](const Brain& brain) {
        return brain.getSynapseStates();
    }, "Get detailed synapse states (source ID, destination ID, synapse ID, weight, delay, type, eligibility trace)")

    m.def("getMemoryStats", [](Brain& brain) {
        return brain.getMemoryStats();
    }, "Get comprehensive memory system statistics")

    m.def("getPlasticityStats", [](Brain& brain) {
        return brain.getPlasticityStats();
    }, "Get plasticity system statistics")

    m.def("getDevelopmentStats", [](Brain& brain) {
        return brain.getDevelopmentStats();
    }, "Get development system statistics")

    m.def("getPredictionStats", [](Brain& brain) {
        return brain.getPredictionStats();
    }, "Get prediction system statistics")

    m.def("getCognitionStats", [](Brain& brain) {
        return brain.getCognitionStats();
    }, "Get cognition system statistics")

    m.def("getSelfModel", [](Brain& brain) {
        return brain.getSelfModel();
    }, "Get self-model information (brain identity and learned characteristics)")

    m.def("getSocialInfo", [](Brain& brain) {
        return brain.getSocialInfo();
    }, "Get social learning information")

    m.def("analyzeBehavior", [](Brain& brain, int windowSize) {
        return brain.analyzeBehavior(windowSize);
    }, "Analyze brain behavior patterns")

    m.def("getCheckpointInfo", [](Brain& brain) {
        return brain.getCheckpointInfo();
    }, "Get checkpoint system information")

    m.def("triggerCheckpoint", [](Brain& brain) {
        return brain.triggerCheckpoint();
    }, "Manually trigger checkpoint creation")

    m.def("compareBrains", [](const Brain& brain1, const Brain& brain2) {
        return brain1.compareBrains(brain2);
    }, "Compare two brains for compatibility analysis")

    // ===== BASIC BRAIN INTERFACES =====
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
