#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <limits>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Touch.hpp"
#include "../src/sensory/Proprioception.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Add module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";

    // Register all exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    py::register_exception<std::length_error>(m, "LengthError");
    py::register_exception<std::logic_error>(m, "LogicError");

    // Register custom exception types for domain-specific errors
    py::register_exception<std::domain_error>(m, "DomainError");
    py::register_exception<std::overflow_error>(m, "OverflowError");

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

    py::class_<Touch, SensoryInput>(m, "Touch", R"pbdoc(Touch/proximity sensors)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t>(), py::arg("numSensors"))
        .def("setData", (void (Touch::*)(const std::vector<float>&)) &Touch::setData, py::arg("data"))
        .def("setData", (void (Touch::*)(float*, size_t)) &Touch::setData, py::arg("data"), py::arg("size"))
        .def("getNumSensors", &Touch::getNumSensors)
        .def("getData", &Touch::getData, py::return_value_policy::reference_internal);

    py::class_<Proprioception, SensoryInput>(m, "Proprioception", R"pbdoc(Proprioception: body position/velocity signals)pbdoc")
        .def(py::init<>())
        .def("setBodyState", (void (Proprioception::*)(float, float, float, float, float, float)) &Proprioception::setBodyState,
             py::arg("x"), py::arg("y"), py::arg("orientation"),
             py::arg("velocityX"), py::arg("velocityY"), py::arg("angularVelocity"))
        .def("setBodyState", (void (Proprioception::*)(const std::vector<float>&)) &Proprioception::setBodyState, py::arg("state"))
        .def("getX", &Proprioception::getX)
        .def("getY", &Proprioception::getY)
        .def("getOrientation", &Proprioception::getOrientation)
        .def("getVelocityX", &Proprioception::getVelocityX)
        .def("getVelocityY", &Proprioception::getVelocityY)
        .def("getAngularVelocity", &Proprioception::getAngularVelocity)
        .def("getNumSensors", &Proprioception::getNumSensors)
        .def("getData", &Proprioception::getData, py::return_value_policy::reference_internal);

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory with persistent activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", (void (NeuralWorkingMemory::*)(const std::vector<float>&, float)) &NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
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
             "Get decay rate for memory traces")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             py::return_value_policy::reference_internal,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is part of the winning population")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention through competition)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             py::return_value_policy::reference_internal,
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
             "Check if neuron is currently attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             py::return_value_policy::reference_internal,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Top-down attention (goal-directed)")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Bottom-up attention (stimulus-driven)")
        .def("reset", &AttentionalSelection::reset, "Reset attention");

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
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")

    // Neuromodulation APIs
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine: reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward for learning")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive: exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detectNovelty", (void (Novelty::*)(const std::vector<float>&, const std::vector<float>&)) &Novelty::detectNovelty,
             py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from sensory input pattern")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory, py::return_value_policy::reference_internal,
             "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getError", &PredictionError::getError, "Get error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory, py::return_value_policy::reference_internal,
             "Get error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get error magnitude");

    // Development control APIs
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system orchestrating neural development)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get stage name")
        .def("update", (void (DevelopmentSystem::*)(Brain*, SimulationStep)) &DevelopmentSystem::update,
             py::arg("brain"), py::arg("currentStep"),
             "Update development")
        .def("update", (void (DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)) &DevelopmentSystem::update,
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Get critical period info")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age");

    // Session and checkpoint management
    m.def("saveBrainSession", [](const std::string& filepath, const std::string& session_name, 
                                 std::shared_ptr<Brain> brain, bool include_config = true) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        
        // Create session metadata
        py::dict metadata;
        metadata["session_name"] = session_name;
        metadata["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        metadata["brain_version"] = "0.1.0";
        metadata["include_config"] = include_config;
        
        if (include_config) {
            auto config = brain->getConfig();
            if (config) {
                metadata["config_keys"] = py::list(config->getKeys());
            }
        }
        
        // Save brain state
        if (!brain->save(filepath)) {
            throw py::runtime_error("Failed to save brain session to file: " + filepath);
        }
        
        // Save metadata (simplified - could use JSON serialization)
        std::string metadata_file = filepath + ".meta";
        std::ofstream meta_file(metadata_file);
        if (meta_file.is_open()) {
            meta_file << "session_name: " << session_name << "\n";
            meta_file << "timestamp: " << metadata["timestamp"].cast<long long>() << "\n";
            meta_file.close();
        }
        
        return metadata;
    }, py::arg("filepath"), py::arg("session_name"), py::arg("brain"), py::arg("include_config") = true,
       "Save brain state as a session with metadata");

    m.def("loadBrainSession", [](const std::string& filepath) -> std::pair<std::shared_ptr<Brain>, py::dict> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(filepath)) {
            throw py::runtime_error("Failed to load brain session from file: " + filepath);
        }
        
        // Load metadata
        py::dict metadata;
        std::string metadata_file = filepath + ".meta";
        std::ifstream meta_file(metadata_file);
        if (meta_file.is_open()) {
            std::string line;
            while (std::getline(meta_file, line)) {
                size_t colon_pos = line.find(":");
                if (colon_pos != std::string::npos) {
                    std::string key = line.substr(0, colon_pos);
                    std::string value = line.substr(colon_pos + 1);
                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    
                    if (key == "timestamp") {
                        metadata["timestamp"] = std::stoll(value);
                    } else if (key == "session_name") {
                        metadata["session_name"] = value;
                    } else if (key == "brain_version") {
                        metadata["brain_version"] = value;
                    }
                }
            }
            meta_file.close();
        }
        
        return std::make_pair(brain, metadata);
    }, py::arg("filepath"), "Load brain session with metadata");

    m.def("listBrainSessions", [](const std::string& directory = ".") {
        py::list sessions;
        
        DIR* dir = opendir(directory.c_str());
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;
                if (name.length() > 4 && name.substr(name.length() - 4) == ".bin") {
                    std::string session_name = name.substr(0, name.length() - 4);
                    std::string filepath = directory + "/" + name;
                    
                    py::dict session_info;
                    session_info["filename"] = name;
                    session_info["session_name"] = session_name;
                    
                    // Try to load metadata
                    std::string metadata_file = filepath + ".meta";
                    std::ifstream meta_file(metadata_file);
                    if (meta_file.is_open()) {
                        std::string line;
                        while (std::getline(meta_file, line)) {
                            size_t colon_pos = line.find(":");
                            if (colon_pos != std::string::npos) {
                                std::string key = line.substr(0, colon_pos);
                                std::string value = line.substr(colon_pos + 1);
                                if (key == "timestamp") {
                                    session_info["timestamp"] = std::stoll(value);
                                }
                            }
                        }
                        meta_file.close();
                    }
                    
                    sessions.append(session_info);
                }
            }
            closedir(dir);
        }
        
        return sessions;
    }, py::arg("directory") = ".", "List available brain sessions");

    // Performance and profiling utilities
    m.def("profileBrain", [](std::shared_ptr<Brain> brain, int num_steps, 
                             bool include_memory = true, bool include_plasticity = true) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        brain->initialize();
        
        // Track metrics
        py::dict metrics;
        std::vector<double> step_times;
        
        for (int step = 0; step < num_steps; ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            brain->step(step);
            auto step_end = std::chrono::high_resolution_clock::now();
            
            double step_time = std::chrono::duration<double, std::milli>(step_end - step_start).count();
            step_times.push_back(step_time);
            
            if (include_memory) {
                auto wm = brain->getWorkingMemory();
                if (wm) {
                    metrics["working_memory_capacity"] = wm->getCapacity();
                    metrics["active_memory_traces"] = wm->getActiveTraces();
                }
            }
            
            if (include_plasticity) {
                auto stdp = brain->getSTDP();
                if (stdp) {
                    metrics["stdp_ltp_weight"] = stdp->getLTPWeight();
                    metrics["stdp_ltd_weight"] = stdp->getLTDWeight();
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double total_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        // Calculate statistics
        double avg_step_time = 0.0, min_step_time = std::numeric_limits<double>::max(), 
               max_step_time = 0.0;
        for (double t : step_times) {
            avg_step_time += t;
            min_step_time = std::min(min_step_time, t);
            max_step_time = std::max(max_step_time, t);
        }
        avg_step_time /= step_times.size();
        
        metrics["total_time_ms"] = total_time;
        metrics["steps_per_second"] = static_cast<double>(num_steps) / (total_time / 1000.0);
        metrics["average_step_time_ms"] = avg_step_time;
        metrics["min_step_time_ms"] = min_step_time;
        metrics["max_step_time_ms"] = max_step_time;
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        metrics["final_firing_rate"] = brain->getAverageFiringRate();
        metrics["neurons_active"] = brain->getActiveNeuronCount();
        metrics["neurons_firing"] = brain->getFiringNeuronCount();
        
        return metrics;
    }, py::arg("brain"), py::arg("num_steps"), py::arg("include_memory") = true, 
       py::arg("include_plasticity") = true, "Profile brain performance over many steps");

    m.def("exportPerformanceReport", [](std::shared_ptr<Brain> brain, const std::string& filepath, 
                                        int benchmark_steps = 100) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        
        // Run benchmark
        auto start = std::chrono::high_resolution_clock::now();
        brain->initialize();
        for (int step = 0; step < benchmark_steps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Create report
        std::ofstream report_file(filepath);
        if (!report_file.is_open()) {
            throw py::runtime_error("Cannot open report file for writing: " + filepath);
        }
        
        report_file << "NLM Brain Performance Report" << std::endl;
        report_file << "==============================" << std::endl;
        report_file << std::endl;
        report_file << "Benchmark Parameters:" << std::endl;
        report_file << "  Steps: " << benchmark_steps << std::endl;
        report_file << "  Total Time: " << duration.count() << " ms" << std::endl;
        report_file << "  Steps per Second: " << static_cast<double>(benchmark_steps) / 
                    (duration.count() / 1000.0) << std::endl;
        report_file << std::endl;
        report_file << "Brain State:" << std::endl;
        report_file << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        report_file << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        report_file << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        report_file << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        report_file << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        report_file << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        report_file << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        report_file << std::endl;
        
        auto wm = brain->getWorkingMemory();
        if (wm) {
            report_file << "Working Memory:" << std::endl;
            report_file << "  Capacity: " << wm->getCapacity() << std::endl;
            report_file << "  Active Traces: " << wm->getActiveTraces() << std::endl;
        }
        
        auto dev_sys = brain->getDevelopmentSystem();
        if (dev_sys) {
            report_file << "Development System:" << std::endl;
            report_file << "  Stage: " << static_cast<int>(dev_sys->getStage()) << std::endl;
            report_file << "  Developmental Age: " << dev_sys->getDevelopmentalAge() << " ms" << std::endl;
        }
        
        report_file.close();
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), py::arg("benchmark_steps") = 100,
       "Export detailed performance report to file");

    // Advanced neuroscience utilities
    m.def("computeNetworkMetrics", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        
        py::dict metrics;
        
        // Basic connectivity metrics
        size_t total_neurons = brain->getTotalNeuronCount();
        size_t total_synapses = brain->getTotalSynapseCount();
        
        metrics["neuron_count"] = total_neurons;
        metrics["synapse_count"] = total_synapses;
        metrics["connection_density"] = total_neurons > 0 ? 
            static_cast<double>(total_synapses) / (total_neurons * total_neurons) : 0.0;
        
        // Activity metrics
        metrics["active_neurons"] = brain->getActiveNeuronCount();
        metrics["firing_neurons"] = brain->getFiringNeuronCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        
        // Memory metrics
        auto wm = brain->getWorkingMemory();
        if (wm) {
            metrics["working_memory_capacity"] = wm->getCapacity();
            metrics["working_memory_decay_rate"] = wm->getDecayRate();
            metrics["active_memory_traces"] = wm->getActiveTraces();
            metrics["memory_activity_level"] = wm->getMemoryActivity();
        }
        
        // Neuromodulation metrics
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            metrics["dopamine_level"] = dopamine->getCurrentLevel();
        }
        
        auto curiosity = brain->getCuriosity();
        if (curiosity) {
            metrics["curiosity_level"] = curiosity->getLevel();
        }
        
        auto novelty = brain->getNovelty();
        if (novelty) {
            metrics["novelty_level"] = novelty->getLevel();
        }
        
        auto pred_error = brain->getPredictionErrorSignal();
        if (pred_error) {
            metrics["prediction_error"] = pred_error->getError();
            metrics["prediction_error_magnitude"] = pred_error->getMagnitude();
        }
        
        // Development metrics
        auto dev_sys = brain->getDevelopmentSystem();
        if (dev_sys) {
            metrics["developmental_stage"] = static_cast<int>(dev_sys->getStage());
            metrics["plasticity_modifier"] = dev_sys->getPlasticityModifier();
            metrics["critical_period_active"] = dev_sys->isCriticalPeriod();
            metrics["developmental_age"] = dev_sys->getDevelopmentalAge();
        }
        
        // Region metrics
        auto regions = brain->getRegions();
        metrics["region_count"] = regions.size();
        
        // Calculate network efficiency (simplified)
        if (total_neurons > 1) {
            double efficiency = 1.0 / (1.0 + static_cast<double>(total_synapses) / total_neurons);
            metrics["network_efficiency"] = efficiency;
        }
        
        return metrics;
    }, py::arg("brain"), "Compute comprehensive network metrics");

    m.def("analyzeBrainTopography", [](std::shared_ptr<Brain> brain, const py::dict& regions_of_interest = py::dict()) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        
        py::dict topography;
        
        // Get all regions
        auto regions = brain->getRegions();
        size_t total_neurons = brain->getTotalNeuronCount();
        
        // Analyze each region
        for (size_t i = 0; i < regions.size(); ++i) {
            auto region = regions[i];
            auto neurons = region->getNeurons();
            auto populations = region->getPopulations();
            
            std::string region_name = "Region_" + std::to_string(i);
            
            // Get region-specific metrics
            py::dict region_metrics;
            region_metrics["neuron_count"] = neurons.size();
            region_metrics["population_count"] = populations.size();
            
            // Calculate activity density
            size_t active_count = 0;
            for (auto neuron : neurons) {
                if (neuron && neuron->getState().firing) {
                    active_count++;
                }
            }
            region_metrics["activity_density"] = neurons.size() > 0 ? 
                static_cast<double>(active_count) / neurons.size() : 0.0;
            
            // Get population type distribution
            std::map<NeuronType, size_t> type_distribution;
            for (auto pop : populations) {
                NeuronType type = pop->getNeuronType();
                type_distribution[type]++;
            }
            
            py::dict type_dist_py;
            for (const auto& pair : type_distribution) {
                type_dist_py[py::cast(static_cast<int>(pair.first))] = pair.second;
            }
            region_metrics["type_distribution"] = type_dist_py;
            
            // Check if region is in regions of interest
            if (!regions_of_interest.is_none()) {
                auto it = regions_of_interest.find(py::cast(region_name));
                if (it != regions_of_interest.end()) {
                    region_metrics["is_of_interest"] = true;
                    region_metrics["interest_weight"] = py::cast<float>(it->second);
                } else {
                    region_metrics["is_of_interest"] = false;
                }
            } else {
                region_metrics["is_of_interest"] = false;
            }
            
            topography[py::cast(region_name)] = region_metrics;
        }
        
        // Calculate network centrality (simplified)
        py::dict network_metrics;
        network_metrics["total_regions"] = regions.size();
        network_metrics["total_neurons"] = total_neurons;
        
        // Calculate average activity across all regions
        double total_activity = 0.0;
        size_t region_count = 0;
        for (const auto& pair : topography) {
            const auto& metrics = py::cast<py::dict>(pair.second);
            if (metrics.contains("activity_density")) {
                total_activity += py::cast<double>(metrics["activity_density"]);
                region_count++;
            }
        }
        network_metrics["average_region_activity"] = region_count > 0 ? 
            total_activity / region_count : 0.0;
        
        // Add topography summary
        py::dict summary;
        summary["topography"] = topography;
        summary["network_metrics"] = network_metrics;
        
        return summary;
    }, py::arg("brain"), py::arg("regions_of_interest") = py::dict(), 
       "Analyze brain topographic organization");

    m.def("createLearningExperiment", [](const std::string& experiment_name, 
                                         std::shared_ptr<Brain> brain_template,
                                         const py::dict& environment_config,
                                         int max_steps,
                                         const py::dict& metrics_config = py::dict()) {
        if (!brain_template) {
            throw py::value_error("Brain template cannot be null");
        }
        
        // Create experiment configuration
        py::dict experiment_config;
        experiment_config["name"] = experiment_name;
        experiment_config["brain_template"] = brain_template;
        experiment_config["environment_config"] = environment_config;
        experiment_config["max_steps"] = max_steps;
        experiment_config["metrics_config"] = metrics_config;
        experiment_config["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        
        // Save experiment configuration
        std::string config_file = experiment_name + "_config.json";
        std::ofstream config_out(config_file);
        if (config_out.is_open()) {
            // Simplified JSON serialization for experiment config
            config_out << "{\"name\": \"" << experiment_name << "\", \"max_steps\": " << max_steps << "}";
            config_out.close();
        }
        
        return experiment_config;
    }, py::arg("experiment_name"), py::arg("brain_template"), 
       py::arg("environment_config"), py::arg("max_steps"),
       py::arg("metrics_config") = py::dict(), "Create a learning experiment configuration");

    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain_template,
                                      const py::dict& environment_config,
                                      int num_episodes,
                                      int max_steps_per_episode) {
        if (!brain_template) {
            throw py::value_error("Brain template cannot be null");
        }
        
        py::dict results;
        results["experiment_name"] = "LearningExperiment";
        results["num_episodes"] = num_episodes;
        results["max_steps_per_episode"] = max_steps_per_episode;
        results["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        
        std::vector<py::dict> episode_results;
        
        for (int episode = 0; episode < num_episodes; ++episode) {
            py::dict episode_result;
            episode_result["episode"] = episode;
            episode_result["start_time"] = std::chrono::system_clock::now().time_since_epoch().count();
            
            // Create fresh brain instance
            auto config = std::make_shared<Config>();
            // Copy configuration from template
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run episode
            for (int step = 0; step < max_steps_per_episode; ++step) {
                brain->step(step);
                
                // Apply environment dynamics (simplified)
                if (environment_config.contains("reward_decay")) {
                    double decay = py::cast<double>(environment_config["reward_decay"]);
                    // Simulate reward decay
                }
                
                // Record metrics
                episode_result["steps_completed"] = step + 1;
                episode_result["final_spike_count"] = brain->getTotalSpikeCount();
                episode_result["final_firing_rate"] = brain->getAverageFiringRate();
            }
            
            episode_result["end_time"] = std::chrono::system_clock::now().time_since_epoch().count();
            episode_result["total_time_ms"] = episode_result["end_time"].cast<long long>() - 
                                            episode_result["start_time"].cast<long long>();
            
            episode_results.append(episode_result);
        }
        
        results["episode_results"] = episode_results;
        
        // Calculate statistics
        py::dict statistics;
        int total_steps = 0;
        double total_time = 0.0;
        int total_spikes = 0;
        
        for (const auto& episode : episode_results) {
            total_steps += py::cast<int>(episode["steps_completed"]);
            total_time += py::cast<double>(episode["total_time_ms"]);
            total_spikes += py::cast<int>(episode["final_spike_count"]);
        }
        
        if (num_episodes > 0) {
            statistics["average_steps_per_episode"] = static_cast<double>(total_steps) / num_episodes;
            statistics["average_time_per_episode_ms"] = total_time / num_episodes;
            statistics["total_spikes_per_episode"] = static_cast<double>(total_spikes) / num_episodes;
            statistics["steps_per_second"] = static_cast<double>(total_steps) / (total_time / 1000.0);
        }
        
        results["statistics"] = statistics;
        results["completed"] = true;
        
        return results;
    }, py::arg("brain_template"), py::arg("environment_config"), 
       py::arg("num_episodes"), py::arg("max_steps_per_episode"),
       "Run a learning experiment with multiple episodes");

    // Visualization APIs
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface for debugging and analysis)pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize,
             "Initialize visualization")
        .def("update", &VisualizationInterface::update,
             "Update visualization data")
        .def("render", &VisualizationInterface::render,
             "Render current state")
        .def("close", &VisualizationInterface::close,
             "Close visualization")
        .def("isActive", &VisualizationInterface::isActive,
             "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"),
             "Set update rate")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate,
             "Get update rate")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"),
             "Network visualization")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"),
             "Activity visualization")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"),
             "Spike raster plot")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"),
             "Weight matrix visualization");

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

    m.def("createExperimentRunner", []() -> std::shared_ptr<ExperimentRunner> {
        return std::make_shared<ExperimentRunner>();
    }, "Create a new experiment runner");

    m.def("createVisualizationInterface", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create a new visualization interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
