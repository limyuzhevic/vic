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
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"

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

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
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
        .def("setPreSpikeHistory", &Synapse::setPreSpikeHistory, py::arg("history"))
        .def("setPostSpikeHistory", &Synapse::setPostSpikeHistory, py::arg("history"))
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags)
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("__repr__", [](const Synapse& syn) {
            return "<Synapse: " + std::to_string(syn.getId().value) + " (" + std::to_string(syn.getSourceNeuron().value) + " -> " + std::to_string(syn.getDestinationNeuron().value) + ") weight=" + std::to_string(syn.getWeight()) + ">";
        });

        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("__repr__", [](const Synapse& syn) {
            return "<Synapse: " + std::to_string(syn.getId().value) + " (" + std::to_string(syn.getSourceNeuron().value) + " -> " + std::to_string(syn.getDestinationNeuron().value) + ") weight=" + std::to_string(syn.getWeight()) + ">";
        });

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population - a group of neurons with shared properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"))
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("createNeurons", &NeuralPopulation::createNeurons, py::arg("startId"))
        .def("getNeurons", &NeuralPopulation::getNeurons)
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
        .def("recordActivity", &NeuralPopulation::recordActivity)
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential)
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential)
        .def("__repr__", [](const NeuralPopulation& pop) {
            return "<NeuralPopulation: " + std::to_string(pop.getId().value) + " size=" + std::to_string(pop.getSize()) + " type=" + std::to_string(static_cast<int>(pop.getNeuronType())) + ">";
        });

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing multiple populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("setNextNeuronIndex", &NeuralRegion::setNextNeuronIndex, py::arg("next"))
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal)
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("getAllPopulations", &NeuralRegion::getAllPopulations)
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"))
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getSynapses", &NeuralRegion::getSynapses)
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
        .def("__repr__", [](const NeuralRegion& region) {
            return "<NeuralRegion: " + std::to_string(region.getId().value) + " name=\"" + region.getName() + "\" neurons=" + std::to_string(region.getTotalNeuronCount()) + " populations=" + std::to_string(region.getPopulationCount()) + ">";
        });

        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("__repr__", [](const NeuralRegion& region) {
            return "<NeuralRegion: " + std::to_string(region.getId().value) + " name=\"" + region.getName() + "\" neurons=" + std::to_string(region.getTotalNeuronCount()) + " populations=" + std::to_string(region.getPopulationCount()) + ">";
        });

    // Neuromodulation classes
    py::class_<Neuromodulator>(m, "Neuromodulator", py::module_local(), R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    py::class_<Acetylcholine, std::shared_ptr<Acetylcholine>>(m, "Acetylcholine", R"pbdoc(Attention and memory consolidation signal)pbdoc")
        .def(py::init<>())
        .def("getName", &Acetylcholine::getName)
        .def("getLevel", &Acetylcholine::getLevel)
        .def("setLevel", &Acetylcholine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Acetylcholine::getPlasticityFactor)
        .def("update", &Acetylcholine::update, py::arg("dt"));

    py::class_<Norepinephrine, std::shared_ptr<Norepinephrine>>(m, "Norepinephrine", R"pbdoc(Arousal and vigilance signal)pbdoc")
        .def(py::init<>())
        .def("getName", &Norepinephrine::getName)
        .def("getLevel", &Norepinephrine::getLevel)
        .def("setLevel", &Norepinephrine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Norepinephrine::getPlasticityFactor)
        .def("update", &Norepinephrine::update, py::arg("dt"));

    py::class_<Serotonin, std::shared_ptr<Serotonin>>(m, "Serotonin", R"pbdoc(Mood, impulsivity, and social behavior signal)pbdoc")
        .def(py::init<>())
        .def("getName", &Serotonin::getName)
        .def("getLevel", &Serotonin::getLevel)
        .def("setLevel", &Serotonin::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Serotonin::getPlasticityFactor)
        .def("update", &Serotonin::update, py::arg("dt"));

    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator for reproducible experiments)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def("seed", &RandomGenerator::seed, py::arg("seed"))
        .def("getSeed", &RandomGenerator::getSeed)
        .def("serializeState", &RandomGenerator::serializeState)
        .def("restoreState", &RandomGenerator::restoreState, py::arg("state"))
        .def("uniformInt", [](RandomGenerator& rng, uint32_t min, uint32_t max) { return rng.uniformInt(min, max); }, py::arg("min"), py::arg("max"))
        .def("uniformInt", [](RandomGenerator& rng, uint64_t min, uint64_t max) { return rng.uniformInt(min, max); }, py::arg("min"), py::arg("max"))
        .def("uniformReal", [](RandomGenerator& rng, double min, double max) { return rng.uniformReal(min, max); }, py::arg("min"), py::arg("max"))
        .def("uniformReal", [](RandomGenerator& rng, float min, float max) { return rng.uniformReal(min, max); }, py::arg("min"), py::arg("max"))
        .def("normal", [](RandomGenerator& rng, double mean, double stddev) { return rng.normal(mean, stddev); }, py::arg("mean"), py::arg("stddev"))
        .def("normal", [](RandomGenerator& rng, float mean, float stddev) { return rng.normal(mean, stddev); }, py::arg("mean"), py::arg("stddev"))
        .def("bernoulli", &RandomGenerator::bernoulli, py::arg("p") = 0.5)
        .def("poisson", &RandomGenerator::poisson, py::arg("lambda"))
        .def("exponential", &RandomGenerator::exponential, py::arg("lambda"))
        .def("randomBool", &RandomGenerator::randomBool)
        .def("randomBit", &RandomGenerator::randomBit)
        .def("pickRandomIndex", &RandomGenerator::pickRandomIndex, py::arg("n"))
        .def("fillBytes", &RandomGenerator::fillBytes, py::arg("data"), py::arg("size"));

    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Accurate simulation time tracking)pbdoc")
        .def(py::init<double>(), py::arg("timestep"))
        .def("getTime", &SimulationClock::getTime)
        .def("getStep", &SimulationClock::getStep)
        .def("getStepTime", &SimulationClock::getStepTime)
        .def("advance", &SimulationClock::advance, py::arg("dt"))
        .def("advanceToStep", &SimulationClock::advanceToStep, py::arg("step"))
        .def("reset", &SimulationClock::reset);

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
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    // Add missing critical methods to Brain class
    .def("getSpikeSystem", &Brain::getSpikeSystem,
         py::return_value_policy::reference_internal,
         "Get the spike system for spike timing")
    .def("getSTDP", &Brain::getSTDP,
         py::return_value_policy::reference_internal,
         "Get the STDP plasticity rule")
    .def("getHebbian", &Brain::getHebbian,
         py::return_value_policy::reference_internal,
         "Get the Hebbian plasticity rule")
    .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
         py::return_value_policy::reference_internal,
         "Get the structural plasticity rule")
    .def("getWorkingMemory", &Brain::getWorkingMemory,
         py::return_value_policy::reference_internal,
         "Get the working memory system")
    .def("getEpisodicMemory", &Brain::getEpisodicMemory,
         py::return_value_policy::reference_internal,
         "Get the episodic memory system")
    .def("getAssociativeMemory", &Brain::getAssociativeMemory,
         py::return_value_policy::reference_internal,
         "Get the associative memory system")
    .def("getPredictionSystem", &Brain::getPredictionSystem,
         py::return_value_policy::reference_internal,
         "Get the prediction system")
    .def("getPlanner", &Brain::getPlanner,
         py::return_value_policy::reference_internal,
         "Get the neural planner")
    .def("getConceptFormation", &Brain::getConceptFormation,
         py::return_value_policy::reference_internal,
         "Get the concept formation system")
    .def("getAttention", &Brain::getAttention,
         py::return_value_policy::reference_internal,
         "Get the attentional selection system")
    .def("getDopamine", &Brain::getDopamine,
         py::return_value_policy::reference_internal,
         "Get the dopamine neuromodulator")
    .def("getCuriosity", &Brain::getCuriosity,
         py::return_value_policy::reference_internal,
         "Get the curiosity neuromodulator")
    .def("getNovelty", &Brain::getNovelty,
         py::return_value_policy::reference_internal,
         "Get the novelty detection system")
    .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
         py::return_value_policy::reference_internal,
         "Get the prediction error signal system")
    .def("getLearnedPreference", &Brain::getLearnedPreference,
         "Get learned preference trait")
    .def("setLearnedPreference", &Brain::setLearnedPreference, py::arg("preference"),
         "Set learned preference trait")
    .def("getNeuromodulationLevel", &Brain::getNeuromodulationLevel,
         "Get current neuromodulation level")
    .def("getCuriosityLevel", &Brain::getCuriosityLevel,
         "Get curiosity level")
    .def("getNoveltyLevel", &Brain::getNoveltyLevel,
         "Get novelty level")
    .def("getPredictionError", &Brain::getPredictionError,
         "Get prediction error")
    .def("enableRewardModulation", &Brain::enableRewardModulation, py::arg("enable"),
         "Enable/disable reward modulation")
    .def("enableStructuralPlasticity", &Brain::enableStructuralPlasticity, py::arg("enable"),
         "Enable/disable structural plasticity")
    .def("enableDevelopment", &Brain::enableDevelopment, py::arg("enable"),
         "Enable/disable development")
    .def("enableCuriosity", &Brain::enableCuriosity, py::arg("enable"),
         "Enable/disable curiosity")
    .def("isRewardModulationEnabled", &Brain::isRewardModulationEnabled,
         "Check if reward modulation is enabled")
    .def("isStructuralPlasticityEnabled", &Brain::isStructuralPlasticityEnabled,
         "Check if structural plasticity is enabled")
    .def("isDevelopmentEnabled", &Brain::isDevelopmentEnabled,
         "Check if development is enabled")
    .def("isCuriosityEnabled", &Brain::isCuriosityEnabled,
         "Check if curiosity is enabled");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Add missing critical methods to AgentBrain class
    .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
              "Get thought trace")
    .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
              "Get motor group activities")
    .def("observeOtherAgent", &AgentBrain::observeOtherAgent, py::arg("otherAgent"),
              "Observe other agent")
    .def("encodeText", &AgentBrain::encodeText, py::arg("text"),
              "Encode text")
    .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
              "Get internal firing rates")
    .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
              "Get reflex bonus count")
    .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
              "Get reflex suppress count")
    .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
              "Get WTA pulse count")
    .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
              "Get planner override count")
    .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
              "Get planner agreement count")
    .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
              "Get planner decision count")
    .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
              "Get concept bias count")
    .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
              "Get self bias count")
    .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
              "Get social bias count")
    .def("getConceptCount", &AgentBrain::getConceptCount,
              "Get concept count")
    .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
              "Get self model confidence")
    .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
              "Get social observation count");
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

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Add convenience factory methods for advanced users
    m.def("createSynapse", [](SynapseId id, NeuronId source, NeuronId destination) {
        return std::make_shared<Synapse>(id, source, destination);
    }, py::arg("id"), py::arg("source"), py::arg("destination"),
       "Create a new synapse");

    m.def("createNeuralPopulation", [](PopulationId id, size_t size) {
        return std::make_shared<NeuralPopulation>(id, size);
    }, py::arg("id"), py::arg("size"),
       "Create a new neural population");

    m.def("createNeuralRegion", [](RegionId id) {
        return std::make_shared<NeuralRegion>(id);
    }, py::arg("id"),
       "Create a new neural region");

    m.def("createDopamine", []() {
        return std::make_shared<Dopamine>();
    }, "Create a dopamine neuromodulator");

    m.def("createAcetylcholine", []() {
        return std::make_shared<Acetylcholine>();
    }, "Create an acetylcholine neuromodulator");

    m.def("createNorepinephrine", []() {
        return std::make_shared<Norepinephrine>();
    }, "Create a norepinephrine neuromodulator");

    m.def("createSerotonin", []() {
        return std::make_shared<Serotonin>();
    }, "Create a serotonin neuromodulator");

    m.def("createRandomGenerator", [](uint64_t seed) {
        return std::make_shared<RandomGenerator>(seed);
    }, py::arg("seed"),
       "Create a random number generator with seed");

    m.def("createSimulationClock", [](double timestep) {
        return std::make_shared<SimulationClock>(timestep);
    }, py::arg("timestep"),
       "Create a simulation clock");

    m.def("createLogger", []() {
        return std::make_shared<Logger>();
    }, "Create a logger");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
