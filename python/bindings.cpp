#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Sensory system
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Brain components
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"

// World system
#include "../src/world/WorldObject.hpp"
#include "../src/world/ActionResult.hpp"

// Event system
#include "../src/brain/SpikeEvent.hpp"
#include "../src/brain/DelayedSpikeEvent.hpp"

// Neuromodulation system
#include "../src/motor/MotorSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

// Memory systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"

// Advanced brain systems
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"

// Utilities
#include "../src/agent/RandomGenerator.hpp"
#include "../src/core/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace nlm {

// Exception translator for std::runtime_error
void translateRuntimeError(const std::runtime_error& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception_translator(translateRuntimeError);
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

    // Core brain components
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getState", &Neuron::getState)
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"))
        .def("decrementRefractory", &Neuron::decrementRefractory)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("getRestingPotential", &Neuron::getRestingPotential)
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"))
        .def("getResetPotential", &Neuron::getResetPotential)
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"))
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform LIF neuron integration")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        .def("step", &Neuron::step, py::arg("currentTime"))
        .def("reset", &Neuron::reset)
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron: " + std::to_string(n.getId().index()) + ">";
        });

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Neural connection)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId)
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"))
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"))
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory)
        .def("clearHistory", &Synapse::clearHistory)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("reset", &Synapse::reset)
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: " + std::to_string(s.getId().index()) + ">";
        });

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neuron population)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"))
        .def("getNeurons", &NeuralPopulation::getNeurons)
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"))
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"))
        .def("step", &NeuralPopulation::step, py::arg("currentTime"))
        .def("reset", &NeuralPopulation::reset)
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation: " + std::to_string(p.getId().index()) + ">";
        });

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal)
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"))
        .def("getSynapses", &NeuralRegion::getSynapses)
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"))
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"))
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity)
        .def("step", &NeuralRegion::step, py::arg("currentTime"))
        .def("reset", &NeuralRegion::reset)
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"))
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion: " + r.getName() + ">";
        });

    // Neuromodulation system
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Attention and memory consolidation)pbdoc")
        .def(py::init<>());

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Arousal and vigilance)pbdoc")
        .def(py::init<>());

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Mood, impulsivity, and social behavior)pbdoc")
        .def(py::init<>());

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration motivation)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel, py::arg("level"))
        .def("getCuriosityLevel", &Curiosity::getCuriosityLevel)
        .def("setCuriosityLevel", &Curiosity::setCuriosityLevel, py::arg("level"));

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("getNoveltyLevel", &Novelty::getNoveltyLevel)
        .def("setNoveltyLevel", &Novelty::setNoveltyLevel, py::arg("level"));

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel)
        .def("setLevel", &PredictionError::setLevel, py::arg("level"))
        .def("getPredictionErrorLevel", &PredictionError::getPredictionErrorLevel)
        });

    // Memory systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Transient active information)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralWorkingMemory::store, py::arg("key"), py::arg("value"))
        .def("recall", &NeuralWorkingMemory::recall, py::arg("key"))
        .def("forget", &NeuralWorkingMemory::forget, py::arg("key"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("hasKey", &NeuralWorkingMemory::hasKey, py::arg("key"))
        .def("getKeys", &NeuralWorkingMemory::getKeys)
        .def("getAllData", &NeuralWorkingMemory::getAllData);

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Experience storage)pbdoc")
        .def(py::init<>())
        .def("storeExperience", &NeuralEpisodicMemory::storeExperience, py::arg("timestamp"), py::arg("percept"), py::arg("action"), py::arg("reward"))
        .def("getExperience", &NeuralEpisodicMemory::getExperience, py::arg("index"))
        .def("getRecentExperiences", &NeuralEpisodicMemory::getRecentExperiences, py::arg("count") = 10)
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("getExperienceCount", &NeuralEpisodicMemory::getExperienceCount)
        .def("getExperienceByTimestamp", &NeuralEpisodicMemory::getExperienceByTimestamp, py::arg("timestamp"));

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Pattern associations)pbdoc")
        .def(py::init<>())
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("inputPattern"), py::arg("outputPattern"), py::arg("strength"))
        .def("recallPattern", &NeuralAssociativeMemory::recallPattern, py::arg("inputPattern"))
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("inputPattern"), py::arg("outputPattern"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("inputPattern"), py::arg("outputPattern"), py::arg("newStrength"))
        .def("removeAssociation", &NeuralAssociativeMemory::removeAssociation, py::arg("inputPattern"), py::arg("outputPattern"))
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAllPatterns", &NeuralAssociativeMemory::getAllPatterns);

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

// Memory systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Transient active information)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralWorkingMemory::store, py::arg("key"), py::arg("value"))
        .def("recall", &NeuralWorkingMemory::recall, py::arg("key"))
        .def("forget", &NeuralWorkingMemory::forget, py::arg("key"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("hasKey", &NeuralWorkingMemory::hasKey, py::arg("key"))
        .def("getKeys", &NeuralWorkingMemory::getKeys)
        .def("getAllData", &NeuralWorkingMemory::getAllData);

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Experience storage)pbdoc")
        .def(py::init<>())
        .def("storeExperience", &NeuralEpisodicMemory::storeExperience, py::arg("timestamp"), py::arg("percept"), py::arg("action"), py::arg("reward"))
        .def("getExperience", &NeuralEpisodicMemory::getExperience, py::arg("index"))
        .def("getRecentExperiences", &NeuralEpisodicMemory::getRecentExperiences, py::arg("count") = 10)
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("getExperienceCount", &NeuralEpisodicMemory::getExperienceCount)
        .def("getExperienceByTimestamp", &NeuralEpisodicMemory::getExperienceByTimestamp, py::arg("timestamp"));

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Pattern associations)pbdoc")
        .def(py::init<>())
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("inputPattern"), py::arg("outputPattern"), py::arg("strength"))
        .def("recallPattern", &NeuralAssociativeMemory::recallPattern, py::arg("inputPattern"))
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("inputPattern"), py::arg("outputPattern"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("inputPattern"), py::arg("outputPattern"), py::arg("newStrength"))
        .def("removeAssociation", &NeuralAssociativeMemory::removeAssociation, py::arg("inputPattern"), py::arg("outputPattern"))
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAllPatterns", &NeuralAssociativeMemory::getAllPatterns);

    // Event system for spike handling and callbacks
    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event queue for spike processing)pbdoc")
        .def(py::init<>())
        .def("queueSpike", &EventQueue::queueSpike, py::arg("event"), "Queue a spike event")
        .def("queueDelayedSpike", &EventQueue::queueDelayedSpike, py::arg("event"), "Queue a delayed spike event")
        .def("processSpikes", &EventQueue::processSpikes, py::arg("currentStep"), "Process all queued spikes for current step")
        .def("processDelayedSpikes", &EventQueue::processDelayedSpikes, py::arg("currentStep"), py::arg("currentTime"), "Process pending delayed spikes")
        .def("registerSpikeHandler", &EventQueue::registerHandler, py::arg("handler"), "Register spike handler callback")
        .def("registerDelayedSpikeHandler", &EventQueue::registerDelayedHandler, py::arg("handler"), "Register delayed spike handler callback")
        .def("getSpikeHistory", &EventQueue::getSpikeHistory, "Get spike history")
        .def("getPendingSpikeCount", &EventQueue::getPendingSpikeCount, "Get pending spike count")
        .def("getPendingDelayedCount", &EventQueue::getPendingDelayedCount, "Get pending delayed spike count")
        .def("getAverageSpikeRate", &EventQueue::getAverageSpikeRate, "Get average spike rate")
        .def("clear", &EventQueue::clear, "Clear all events");

    // Visualization interface for debugging and analysis
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface for debugging and analysis)pbdoc")
        .def("initialize", &VisualizationInterface::initialize, "Initialize visualization")
        .def("update", &VisualizationInterface::update, "Update visualization data")
        .def("render", &VisualizationInterface::render, "Render current state")
        .def("close", &VisualizationInterface::close, "Close visualization")
        .def("isActive", &VisualizationInterface::isActive, "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"), "Set update rate")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate, "Get update rate")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"), "Network visualization")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"), "Activity visualization")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"), "Spike raster plot")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"), "Weight matrix visualization");

    // Performance monitoring and profiling
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance monitoring and profiling)pbdoc")
        .def(py::init<>())
        .def("startProfile", &PerformanceProfiler::startProfile, "Start performance profiling")
        .def("stopProfile", &PerformanceProfiler::stopProfile, "Stop performance profiling")
        .def("getMetrics", &PerformanceProfiler::getMetrics, "Get performance metrics")
        .def("recordMetric", &PerformanceProfiler::recordMetric, py::arg("name"), py::arg("value"), "Record a performance metric")
        .def("getFPS", &PerformanceProfiler::getFPS, "Get current FPS")
        .def("getAverageFPS", &PerformanceProfiler::getAverageFPS, "Get average FPS")
        .def("getMemoryUsage", &PerformanceProfiler::getMemoryUsage, "Get memory usage in MB")
        .def("clear", &PerformanceProfiler::clear, "Clear all performance metrics");

    // Advanced neural operations (batch operations, vectorization)
    m.def("batchStep", &Brain::batchStep, py::arg("brain"), py::arg("steps"), "Perform multiple simulation steps in batch")
        .def("vectorizeNeuronActivity", &Brain::vectorizeNeuronActivity, py::arg("brain"), "Vectorize neuron activity data")
        .def("optimizeConnectivity", &Brain::optimizeConnectivity, py::arg("brain"), py::arg("targetDensity"), "Optimize connectivity patterns")
        .def("computeNeuralStats", &Brain::computeNeuralStats, py::arg("brain"), "Compute neural network statistics")
        .def("normalizeWeights", &Brain::normalizeWeights, py::arg("brain"), "Normalize synaptic weights");

    // Custom neural network building utilities
    py::class_<NeuralNetworkBuilder>(m, "NeuralNetworkBuilder", R"pbdoc(Custom neural network building utilities)pbdoc")
        .def(py::init<>())
        .def("createLayeredNetwork", &NeuralNetworkBuilder::createLayeredNetwork, py::arg("numLayers"), py::arg("neuronsPerLayer"), "Create a layered neural network")
        .def("createAutoencoder", &NeuralNetworkBuilder::createAutoencoder, py::arg("encodingDim"), "Create an autoencoder network")
        .def("createRNN", &NeuralNetworkBuilder::createRNN, py::arg("hiddenSize"), "Create a recurrent neural network")
        .def("createLSTM", &NeuralNetworkBuilder::createLSTM, py::arg("hiddenSize"), "Create an LSTM network")
        .def("connectRegions", &NeuralNetworkBuilder::connectRegions, py::arg("fromRegion"), py::arg("toRegion"), py::arg("connectionProbability"), "Connect two brain regions")
        .def("initializeTopology", &NeuralNetworkBuilder::initializeTopology, py::arg("brain"), "Initialize custom network topology");

    // Experiment tracking and management
    py::class_<ExperimentTracker>(m, "ExperimentTracker", R"pbdoc(Experiment tracking and management)pbdoc")
        .def(py::init<>())
        .def("createExperiment", &ExperimentTracker::createExperiment, py::arg("name"), "Create a new experiment")
        .def("startExperiment", &ExperimentTracker::startExperiment, py::arg("experiment"), "Start an experiment")
        .def("endExperiment", &ExperimentTracker::endExperiment, py::arg("experiment"), "End an experiment")
        .def("recordData", &ExperimentTracker::recordData, py::arg("experiment"), py::arg("metric"), py::arg("value"), "Record experiment data")
        .def("getExperimentResults", &ExperimentTracker::getExperimentResults, py::arg("experiment"), "Get experiment results")
        .def("getAllExperiments", &ExperimentTracker::getAllExperiments, "Get all experiments")
        .def("saveResults", &ExperimentTracker::saveResults, py::arg("filepath"), "Save experiment results to file")
        .def("loadResults", &ExperimentTracker::loadResults, py::arg("filepath"), "Load experiment results from file")
        .def("clear", &ExperimentTracker::clear, "Clear all experiment data");

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

    m.def("createBrainWithDefaultConfig", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("simulation.steps_per_second", 100);
        config->set("brain.region_count", 1);
        config->set("brain.population_size", 100);
        config->set("brain.initial_excitatory_ratio", 0.8f);
        config->set("brain.initial_inhibitory_ratio", 0.2f);
        config->set("brain.synaptic_weight_mean", 0.5f);
        config->set("brain.synaptic_weight_std", 0.1f);
        config->set("brain.connection_probability", 0.1f);
        config->set("brain.enable_structural_plasticity", true);
        config->set("brain.enable_reward_modulation", true);
        config->set("brain.enable_curiosity", true);
        config->set("brain.enable_novelty_detection", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a brain with default configuration and initialize it");

    m.def("createCompleteAgentSetup", [](std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        config->set("simulation.steps_per_second", 100);
        config->set("brain.region_count", 1);
        config->set("brain.population_size", 100);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("world"), "Create a complete agent setup with brain, world, and agent brain");

    m.def("createWorldWithSetup", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        
        // Add some default resources
        auto resource = std::make_shared<WorldObject>();
        resource->x = 50.0f;
        resource->y = 50.0f;
        resource->type = WorldObjectType::Resource;
        resource->value = 100.0f;
        resource->radius = 0.5f;
        resource->active = true;
        world->addObject(resource);
        
        // Add some default hazards
        auto hazard = std::make_shared<WorldObject>();
        hazard->x = 200.0f;
        hazard->y = 150.0f;
        hazard->type = WorldObjectType::Hazard;
        hazard->value = -50.0f;
        hazard->radius = 0.5f;
        hazard->active = true;
        world->addObject(hazard);
        
        return world;
    }, py::arg("width") = 300, py::arg("height") = 300,
        py::arg("visionWidth") = 30, py::arg("visionHeight") = 30,
        "Create a world with default setup and configuration");

    m.def("createSensoryPipeline", [](std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        config->set("sensory.vision.enabled", true);
        config->set("sensory.vision.width", 30);
        config->set("sensory.vision.height", 30);
        config->set("sensory.vision.channels", 3);
        
        config->set("sensory.audio.enabled", true);
        config->set("sensory.audio.sampleRate", 44100);
        config->set("sensory.audio.numSamples", 1024);
        
        config->set("sensory.internal.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("world"), "Create a sensory processing pipeline with vision, audio, and internal signals");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
