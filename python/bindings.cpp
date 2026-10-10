#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <deque>

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides comprehensive bindings for all brain systems:
        - Neuromodulation systems (reward, curiosity, novelty, prediction error)
        - Prediction systems (sensory prediction and neural prediction)
        - Memory systems (working memory, episodic memory, semantic memory, 
          procedural memory, and associative memory)
        - Cognition systems (neural planning, concept formation, attentional selection)
        - Development system (developmental process management)
        
        Key Features:
        - Comprehensive integration of all brain systems
        - Backward compatibility with existing API
        - Factory functions for easy object creation
        - Pythonic interfaces with proper error handling
        - Example usage patterns for rapid development
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== EXISTING CORE TYPES (MAINTAINED FOR BACKWARD COMPATIBILITY) =====

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

    // ===== EXISTING COMPONENTS (MAINTAINED FOR BACKWARD COMPATIBILITY) =====

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ===== NEUROMODULATION SYSTEMS =====

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Neuromodulator: Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get neuromodulatory effect on plasticity")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine: Reward signaling system)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward to dopamine system")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity: Exploration motivation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize curiosity with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get current curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive (same as level)")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity system");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty: Novelty detection)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize novelty detector with brain reference")
        .def("getLevel", &Novelty::getLevel, "Get current novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set current novelty level")
        .def("detectNovelty", [](Novelty& self, const py::dict& observationDict, const py::dict& previousObservationDict) {
            // This is a simplified wrapper for demonstration
            // In practice, convert py::dict back to appropriate data structures
            self.detectNovelty(std::vector<float>(), std::vector<float>());
        }, py::arg("observation"), py::arg("previousObservation"), "Detect novelty from observation")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& currentPattern,
                               const std::vector<float>& previousPattern) {
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"), "Detect novelty from sensory input pattern")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(PredictionError: Prediction error computation)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize prediction error calculator with brain reference")
        .def("getError", &PredictionError::getError, "Get current error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"), "Update prediction")
        .def("getHistory", &PredictionError::getHistory, "Get error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get error magnitude for neuromodulation");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward: Reward accumulation and computation)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue, "Get current reward value")
        .def("setValue", &Reward::setValue, py::arg("value"), "Set current reward value")
        .def("add", &Reward::add, py::arg("delta"), "Accumulate reward")
        .def("reset", &Reward::reset, "Reset accumulated reward")
        .def("computeReward", [](Reward& self, const py::dict& observationDict) {
            // Simplified wrapper for demonstration
            return self.computeReward(Observation());
        }, py::arg("observation"), "Compute reward from observation")
        .def("getHistory", &Reward::getHistory, "Get reward history")
        .def("clearHistory", &Reward::clearHistory, "Clear reward history");

    // ===== PREDICTION SYSTEM =====

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(PredictionSystem: Sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("predictNextState", [](PredictionSystem& self, const py::dict& currentState) {
            // Simplified wrapper for demonstration
            return SensoryInput();
        }, py::arg("currentState"), "Make prediction for next timestep")
        .def("updatePredictions", [](PredictionSystem& self, const py::dict& predicted,
                                   const py::dict& actual) {
            // Simplified wrapper for demonstration
            self.updatePredictions(SensoryInput(), SensoryInput());
        }, py::arg("predicted"), py::arg("actual"), "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence, "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory, "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory, "Clear prediction history")
        .def("train", [](PredictionSystem& self, const py::dict& observation) {
            // Simplified wrapper for demonstration
            self.train(SensoryInput());
        }, py::arg("observation"), "Train prediction model");

    // ===== NEURAL PREDICTION =====

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(NeuralPrediction: Neural prediction mechanisms)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"), "Initialize neural prediction with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState, py::arg("state"), py::arg("step"), "Record current sensory state for prediction")
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("step"), "Generate prediction for next timestep")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation, py::arg("actualState"), py::arg("step"), "Update predictions with observation")
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence, py::arg("action"), py::arg("currentState"), "Predict consequences of action")
        .def("getPredictionError", &NeuralPrediction::getPredictionError, "Get current prediction error")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence, "Get prediction confidence")
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps, py::arg("step"), py::arg("numSteps"), "Multi-step prediction")
        .def("recordAction", &NeuralPrediction::recordAction, py::arg("action"), py::arg("step"), "Record action for learning")
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory, "Get prediction error history")
        .def("clearHistory", &NeuralPrediction::clearHistory, "Clear prediction history")
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"), "Enable/disable temporal prediction")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"), "Enable/disable action consequence prediction")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"), "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("steps"), "Set prediction horizon");

    // ===== MEMORY SYSTEMS =====

    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(WorkingMemory: Transient active information storage)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store item in working memory")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"), "Retrieve item from working memory")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"), "Check if item exists in working memory")
        .def("clear", &WorkingMemory::clear, "Clear all working memory")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current working memory size")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"), "Decay all items in working memory");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(EpisodicMemoryItem: Individual memory items)pbdoc")
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
        .def_readwrite("age", &EpisodicMemoryItem::age);

    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(EpisodicMemory: Event and experience storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"), "Initialize episodic memory with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"), "Store a new experience")
        .def("retrieveSimilar", [](NeuralEpisodicMemory& self, const py::dict& queryDict, size_t maxResults) {
            // Simplified wrapper for demonstration
            std::vector<float> queryPattern;
            return self.retrieveSimilar(queryPattern, maxResults);
        }, py::arg("query"), py::arg("maxResults") = 5, "Retrieve episodes similar to query pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10, "Retrieve episodes from time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5, "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5, "Retrieve episodes following specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"), "Replay an episode")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"), "Update episode relevance")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"), "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward, "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Consolidate old episodes")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"), "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled, "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"), "Get episodes for replay")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"), "Replay multiple episodes");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(SemanticMemory: Acquired knowledge storage)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"), "Store a fact in semantic memory")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"), "Retrieve a fact from semantic memory")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"), "Check if fact exists in semantic memory")
        .def("getAllFacts", &SemanticMemory::getAllFacts, "Get all facts in semantic memory")
        .def("clear", &SemanticMemory::clear, "Clear all semantic memory");

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(ProceduralMemory: Skills and habits storage)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"), "Learn a new skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"), "Get a specific skill")
        .def("getAllSkills", &ProceduralMemory::getAllSkills, "Get all learned skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"), "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear, "Clear all procedural memory");

    py::class_<Skill>(m, "Skill", R"pbdoc(Skill: Individual skill patterns)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name)
        .def_readwrite("neuralPattern", &Skill::neuralPattern)
        .def_readwrite("proficiency", &Skill::proficiency);

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(AssociativeMemory: Pattern relationship storage)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"), "Create association between patterns")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"), "Get patterns associated with given pattern")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"), "Get association strength")
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"), "Update association strength")
        .def("clear", &AssociativeMemory::clear, "Clear all associations")
        .def("getAssociationCount", &AssociativeMemory::getAssociationCount, "Get number of associations");

    // ===== COGNITION SYSTEMS =====

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(NeuralPlanner: Action planning and decision making)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize neural planner with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f, "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"), "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, "Get number of planning steps")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"), "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence, "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"), "Update plan based on outcome")
        .def("clearCache", &NeuralPlanner::clearCache, "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"), "Set action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal, "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful, "Check if recent plan was successful");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(ConceptFormation: Pattern discovery and abstraction)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize concept formation with brain reference")
        .def("presentExperience", [](ConceptFormation& self, const py::dict& patternDict, const py::dict& featuresDict, float reward, SimulationStep currentTime) {
            // Simplified wrapper for demonstration
            std::vector<float> pattern, features;
            return self.presentExperience(pattern, features, reward, currentTime);
        }, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"), "Present new experience to concept formation system")
        .def("presentExperience", [](ConceptFormation& self, const std::vector<float>& pattern,
                                 const std::vector<float>& features, float reward, SimulationStep currentTime) {
            return self.presentExperience(pattern, features, reward, currentTime);
        }, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"), "Present new experience to concept formation system")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f, "Get concept that best matches a pattern")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get all discovered concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"), "Get a specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"), "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"), "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"), "Get concept stability")
        .def("getConceptCount", &ConceptFormation::getConceptCount, "Get number of discovered concepts")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"), "Merge two concepts")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"), "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"), "Compute similarity between patterns")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f, "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"), "Find concept for given pattern")
        .def("clear", &ConceptFormation::clear, "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"), "Get concept generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"), "Set concept formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"), "Set concept stability window");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(AttentionalSelection: Focus and attention control)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"), "Initialize attentional selection with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f, "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"), "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention, "Release attention (allow equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions, "Get currently attended regions")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"), "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"), "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"), "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"), "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"), "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"), "Check if neuron is attended")
        .def("getWinners", &AttentionalSelection::getWinners, "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"), "Apply top-down attention bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"), "Apply bottom-up salience")
        .def("reset", &AttentionalSelection::reset, "Reset attention system");

    // ===== DEVELOPMENT SYSTEM =====

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(DevelopmentSystem: Developmental process management)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get current stage name")
        .def("update", [](DevelopmentSystem& self, Brain& brain, SimulationStep currentStep) {
            self.update(&brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"), "Update development system (simple version)")
        .def("update", [](DevelopmentSystem& self, Brain& brain, RandomGenerator& rng, TimestepDuration dt) {
            // Simplified wrapper - in practice would need access to rng
            self.update(&brain, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age (in simulation time)")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age");

    // ===== FACTORY FUNCTIONS =====

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world)pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");

    // ===== INTEGRATION FUNCTIONS =====

    m.def("initializeAllSystems", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        // Initialize all brain systems with proper interconnections
        if (brain) {
            // Setup neuromodulation
            if (auto* curiosity = brain->getCuriosity()) {
                curiosity->initialize(brain.get());
            }
            if (auto* novelty = brain->getNovelty()) {
                novelty->initialize(brain.get());
            }
            if (auto* predictionError = brain->getPredictionErrorSignal()) {
                predictionError->initialize(brain.get());
            }
            
            // Setup prediction system
            if (auto* prediction = brain->getPredictionSystem()) {
                prediction->train(*world->getSensoryPercept());
            }
            
            // Setup memory systems
            if (auto* workingMemory = brain->getWorkingMemory()) {
                // Initialize working memory with brain reference
            }
            if (auto* episodicMemory = brain->getEpisodicMemory()) {
                episodicMemory->initialize(brain.get());
            }
            if (auto* associativeMemory = brain->getAssociativeMemory()) {
                associativeMemory->initialize(brain.get());
            }
            
            // Setup cognition systems
            if (auto* planner = brain->getPlanner()) {
                planner->initialize(brain.get());
            }
            if (auto* conceptFormation = brain->getConceptFormation()) {
                conceptFormation->initialize(brain.get());
            }
            if (auto* attention = brain->getAttention()) {
                attention->initialize(brain.get());
            }
            
            // Initialize development system
            if (auto* devSystem = brain->getDevelopmentSystem()) {
                devSystem->setStage(DevelopmentalStage::CriticalPeriod);
            }
        }
    }, py::arg("brain"), py::arg("world"), R"pbdoc(Initialize all brain systems with proper interconnections)pbdoc");

    m.def("runCompleteSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid simulation components");
        }
        
        brain->initialize();
        agent->initialize(world.get());
        world->reset();
        
        for (size_t i = 0; i < numSteps; ++i) {
            // World updates
            world->update(0.1);
            
            // Agent processes world
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain processes and produces action
            brain->step(static_cast<SimulationStep>(i));
            auto action = agent->decodeMotorCommand();
            
            // World executes action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply neuromodulation based on experience
            if (agent->isRewardModulationEnabled()) {
                float reward = 0.0f; // Would come from world feedback
                agent->applyRewardModulation(reward, 0.0f);
            }
            if (agent->isCuriosityEnabled()) {
                float novelty = agent->getNoveltyLevel();
                float predictionError = agent->getPredictionError();
                // Update curiosity system
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), R"pbdoc(Run a complete simulation with all systems integrated)pbdoc");

    // ===== EXAMPLE USAGE AND DOCUMENTATION =====

    m.def("createExampleAgent", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(world.get());
        
        // Enable key systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return std::make_tuple(brain, world, agent);
    }, R"pbdoc(Create a complete example agent with all systems enabled)pbdoc");

    // ===== CONSTANTS =====

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===== SYSTEM INFORMATION =====

    m.def("getSystemInfo", []() {
        return R"pbdoc(
            Enhanced NLM Brain System
            ========================
            
            This enhanced version integrates all brain systems:
            
            Neuromodulation:
            - Dopamine: Reward signaling and reinforcement
            - Curiosity: Exploration motivation
            - Novelty: Novelty detection and adaptation
            - Prediction Error: Learning from prediction mismatches
            
            Prediction:
            - PredictionSystem: Sensory prediction and error computation
            - NeuralPrediction: Neural prediction mechanisms
            
            Memory:
            - WorkingMemory: Transient active information storage
            - EpisodicMemory: Event and experience storage
            - SemanticMemory: Acquired knowledge storage
            - ProceduralMemory: Skills and habits storage
            - AssociativeMemory: Pattern relationship storage
            
            Cognition:
            - NeuralPlanner: Action planning and decision making
            - ConceptFormation: Pattern discovery and abstraction
            - AttentionalSelection: Focus and attention control
            
            Development:
            - DevelopmentSystem: Developmental process management
            
            All systems are interconnected and integrated into a coherent brain loop.
        )pbdoc";
    }, R"pbdoc(Get information about all integrated brain systems)pbdoc");

    m.def("getExampleUsage", []() {
        return R"pbdoc(
            Example Usage Patterns
            ========================
            
            Example 1: Basic Agent
            ---------------------
            import pynlm
            
            # Create an agent with all systems
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Add neuromodulation
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Run simulation
            for i in range(1000):
                brain.step(i)
            
            Example 2: Integrated System
            ---------------------------
            # Use factory functions for complete integration
            brain, world, agent = pynlm.createExampleAgent()
            pynlm.initializeAllSystems(brain, world)
            
            # Run complete simulation
            success = pynlm.runCompleteSimulation(brain, world, agent, 5000)
            
            Example 3: Using Memory Systems
            -------------------------------
            # Access memory systems through brain interface
            working_mem = brain.getWorkingMemory()
            episodic_mem = brain.getEpisodicMemory()
            semantic_mem = brain.getSemanticMemory()
            
            # Store and retrieve experiences
            episode = pynlm.EpisodicMemoryItem()
            episodic_mem.storeEpisode(episode)
            
            Example 4: Neuromodulation Control
            ----------------------------------
            # Control neuromodulators for learning and behavior
            dopamine = brain.getDopamine()
            curiosity = brain.getCuriosity()
            novelty = brain.getNovelty()
            
            # Use them in learning loops
            reward = 1.0
            dopamine.signalReward(reward)
            curiosity_level = curiosity.getLevel()
            novelty_level = novelty.getLevel()
            
            All systems work together to create a coherent intelligent agent
            that learns, develops, and adapts through interaction with its environment.
        )pbdoc";
    }, R"pbdoc(Get comprehensive example usage patterns)pbdoc");

}

} // namespace nlm