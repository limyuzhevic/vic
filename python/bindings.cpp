#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/WorldObject.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/MotorSystem.hpp"
#include "../src/motor/Action.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/memory/MemorySystem.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/SemanticMemory.hpp"
#include "../src/memory/ProceduralMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive Python bindings for all NLM classes including
        Brain, Neuron, Synapse, NeuralPopulation, NeuralRegion, Memory Systems,
        Plasticity Rules, Neuromodulation Systems, Cognition Systems,
        Sensory Systems, Prediction Systems, Motor Systems, Development Systems,
        Environment Systems, World Systems, Agent Systems, and Experiment Systems.
        
        This package provides a complete Python API to the NLM neural simulation
        framework, enabling users to create, simulate, and experiment with
        artificial developmental brains using Python.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Type aliases
    m.attr("MembranePotential") = py::cast(MembranePotential());
    m.attr("SynapticWeight") = py::cast(SynapticWeight());
    m.attr("FiringRate") = py::cast(FiringRate());
    m.attr("Delay") = py::cast(Delay());
    m.attr("NeuronId") = py::cast(NeuronId());
    m.attr("SynapseId") = py::cast(SynapseId());
    m.attr("RegionId") = py::cast(RegionId());
    m.attr("PopulationId") = py::cast(PopulationId());
    m.attr("Timestamp") = py::cast(Timestamp());
    m.attr("SimulationStep") = py::cast(SimulationStep());

    // Core type enumerations
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

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
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

    // Identity types
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

    // Config class
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

    // RandomGenerator class
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generator for NLM)pbdoc")
        .def(py::init<>())
        .def(py::init<int>(), py::arg("seed"))
        .def("uniform_real", &RandomGenerator::uniformReal, py::arg("min"), py::arg("max"),
             "Generate uniform random float")
        .def("uniform_int", &RandomGenerator::uniformInt, py::arg("min"), py::arg("max"),
             "Generate uniform random integer")
        .def("normal", &RandomGenerator::normal, py::arg("mean"), py::arg("std"),
             "Generate normal random float")
        .def("sample_from_distribution", &RandomGenerator::sampleFromDistribution,
             py::arg("weights"), "Sample from discrete distribution")
        .def("shuffle_list", &RandomGenerator::shuffleList, py::arg("lst"),
             "Shuffle a list")
        .def("get_seed", &RandomGenerator::getSeed,
             "Get the random seed");

    // SimulationClock class
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for NLM)pbdoc")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("timestep"))
        .def("advance", &SimulationClock::advance, py::arg("steps") = 1,
             "Advance the clock")
        .def("get_elapsed_time", &SimulationClock::getElapsedTime,
             "Get elapsed simulation time")
        .def("get_elapsed_wall_time", &SimulationClock::getElapsedWallTime,
             "Get elapsed wall-clock time")
        .def("reset", &SimulationClock::reset, "Reset the clock")
        .def("sync", &SimulationClock::sync, py::arg("external_time"),
             "Sync clock to external time")
        .def("__repr__", [](const SimulationClock& clock) {
            return "<SimulationClock step=" + std::to_string(clock.current_step) +
                   " time=" + std::to_string(clock.current_time) + "s>";
        });

    // Logger class
    py::class_<Logger>(m, "Logger", R"pbdoc(Logger for NLM)pbdoc")
        .def(py::init<>())
        .def("add_handler", &Logger::addHandler, py::arg("handler"),
             "Add a log handler")
        .def("remove_handler", &Logger::removeHandler, py::arg("handler"),
             "Remove a log handler")
        .def("log", &Logger::log, py::arg("level"), py::arg("message"),
             "Log a message")
        .def("debug", &Logger::debug, py::arg("message"), "Log debug message")
        .def("info", &Logger::info, py::arg("message"), "Log info message")
        .def("warning", &Logger::warning, py::arg("message"), "Log warning message")
        .def("error", &Logger::error, py::arg("message"), "Log error message")
        .def("critical", &Logger::critical, py::arg("message"), "Log critical message")
        .def("get_messages", [](Logger& self, py::object level) {
            if (level.is_none()) {
                return self.getMessages();
            }
            return self.getMessages(level.cast<int>());
        }, py::arg("level") = py::none(), "Get log messages")
        .def("clear", &Logger::clear, "Clear all log messages");

    // Neuron class
    py::class_<Neuron>(m, "Neuron", R"pbdoc(LIF neuron implementation)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, "Get neuron identifier")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getState", &Neuron::getState, "Get neuron state")
        .def("setMembranePotential", &Neuron::setMembranePotential,
             py::arg("potential"), "Set membrane potential")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential,
             py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"),
             "Set firing threshold")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is firing")
        .def("isRefractory", &Neuron::isRefractory,
             "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"),
             "Set firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod,
             py::arg("steps"), "Set refractory period")
        .def("decrementRefractory", &Neuron::decrementRefractory,
             "Decrement refractory period")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"),
             "Set firing rate")
        .def("setLeakConductance", &Neuron::setLeakConductance,
             py::arg("conductance"), "Set leak conductance")
        .def("getLeakConductance", &Neuron::getLeakConductance,
             "Get leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod,
             "Get refractory period")
        .def("setRestingPotential", &Neuron::setRestingPotential,
             py::arg("potential"), "Set resting potential")
        .def("getRestingPotential", &Neuron::getRestingPotential,
             "Get resting potential")
        .def("setResetPotential", &Neuron::setResetPotential,
             py::arg("potential"), "Set reset potential")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if threshold is exceeded")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime,
             "Get last spike time")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput,
             py::arg("amplitude"), "Receive excitatory input")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput,
             py::arg("amplitude"), "Receive inhibitory input")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput,
             py::arg("amplitude"), "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"),
             "Inject current")
        .def("getTotalCurrent", &Neuron::getTotalCurrent,
             "Get total synaptic current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent,
             "Clear synaptic input")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record spike")
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             "Get spike history")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory,
             "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse,
             py::arg("handle"), "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse,
             py::arg("handle"), "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses,
             "Get incoming synapses")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses,
             "Get outgoing synapses")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"),
             py::arg("stdp"), py::arg("reward_modulated"),
             "Enable plasticity mechanisms")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"),
             "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"),
             "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("stepLIF", &Neuron::stepLIF, py::arg("current_time"), py::arg("dt") = 0.001,
             "Step LIF neuron")
        .def("step", &Neuron::step, py::arg("current_time"), py::arg("dt") = 0.001,
             "Step neuron")
        .def("reset", &Neuron::reset, "Reset neuron")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"),
             "Initialize with random parameters")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId()) + " type=" +
                   std::to_string(static_cast<int>(n.getType())) + " " +
                   std::to_string(n.getMembranePotential()) + "mV>";
        });

    // Synapse class
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("dest"))
        .def("getId", &Synapse::getId, "Get synapse identifier")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to weight")
        .def("getDelay", &Synapse::getDelay, "Get transmission delay")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set transmission delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record presynaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record postsynaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get postsynaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"),
             "Enable plasticity mechanisms")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decay_rate"), "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("current_time"), "Update synapse")
        .def("reset", &Synapse::reset, "Reset synapse")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId()) + " " +
                   "source=" + std::to_string(s.getSourceNeuron()) + " " +
                   "dest=" + std::to_string(s.getDestinationNeuron()) + " " +
                   "weight=" + std::to_string(s.getWeight()) + ">";
        });

    // NeuralPopulation class
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population)pbdoc")
        .def(py::init<>(), "Create empty population")
        .def("getId", &NeuralPopulation::getId, "Get population identifier")
        .def("getType", &NeuralPopulation::getType, "Get population type")
        .def("setType", &NeuralPopulation::setType, py::arg("type"), "Set population type")
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount, "Get neuron count")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron")
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuron_id"), "Remove neuron")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("neuron_id"), "Get neuron")
        .def("getExcitatoryNeurons", &NeuralPopulation::getExcitatoryNeurons, "Get excitatory neurons")
        .def("getInhibitoryNeurons", &NeuralPopulation::getInhibitoryNeurons, "Get inhibitory neurons")
        .def("getActiveNeurons", &NeuralPopulation::getActiveNeurons, "Get active neurons")
        .def("getTotalFiringRate", &NeuralPopulation::getTotalFiringRate, "Get total firing rate")
        .def("step", &NeuralPopulation::step, py::arg("current_time"), "Step population")
        .def("reset", &NeuralPopulation::reset, "Reset population")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation id=" + std::to_string(p.getId()) + " " +
                   "neurons=" + std::to_string(p.getNeuronCount()) + ">";
        });

    // NeuralRegion class
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region)pbdoc")
        .def(py::init<>(), "Create empty region")
        .def("getId", &NeuralRegion::getId, "Get region identifier")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("getType", &NeuralRegion::getType, "Get region type")
        .def("setType", &NeuralRegion::setType, py::arg("type"), "Set region type")
        .def("getNeuronCount", &NeuralRegion::getNeuronCount, "Get neuron count")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("population"), "Add population")
        .def("removePopulation", &NeuralRegion::removePopulation, py::arg("population_id"), "Remove population")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("population_id"), "Get population")
        .def("getTotalNeurons", &NeuralRegion::getTotalNeurons, "Get total neurons")
        .def("getTotalSynapses", &NeuralRegion::getTotalSynapses, "Get total synapses")
        .def("getActiveNeurons", &NeuralRegion::getActiveNeurons, "Get active neurons")
        .def("getTotalFiringRate", &NeuralRegion::getTotalFiringRate, "Get total firing rate")
        .def("step", &NeuralRegion::step, py::arg("current_time"), "Step region")
        .def("reset", &NeuralRegion::reset, "Reset region")
        .def("initializeRandom", &NeuralRegion::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion id=" + std::to_string(r.getId()) + " " +
                   "name=" + r.getName() + ">";
        });

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, "Get all regions")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get the random generator")
        .def("logStatus", &Brain::logStatus, "Log brain status");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
              py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
              py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
              py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
              py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
              "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
              "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
              "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
              "Check if curiosity is enabled");

    // VisualizationInterface class
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface for debugging and analysis)pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize, "Initialize visualization")
        .def("update", &VisualizationInterface::update, "Update visualization data")
        .def("render", &VisualizationInterface::render, "Render current state")
        .def("close", &VisualizationInterface::close, "Close visualization")
        .def("isActive", &VisualizationInterface::isActive, "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"), "Set update rate")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate, "Get update rate")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, "Visualize network structure", py::arg("brain"))
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, "Visualize neural activity", py::arg("brain"))
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, "Visualize spike raster plot", py::arg("spikes"))
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, "Visualize synaptic weights", py::arg("brain"));

    // AgentBrain class

    // Factory functions
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

    // Visualization factory function
    m.def("createVisualization", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create a visualization interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
