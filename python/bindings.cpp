#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

// Include all the core headers
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Brain.hpp"

#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"

#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"

#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/performance/SparseConnectivity.hpp"
#include "../src/performance/OptimizedBrain.hpp"

#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

#include "../src/motor/Action.hpp"
#include "../src/motor/MotorSystem.hpp"

#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

#include "../src/world/SimpleWorld.hpp"
#include "../src/world/Observation.hpp"

#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

namespace py = pybind11;
namespace nlm {

// Factory function for creating Brain with config
std::shared_ptr<Brain> createBrainWithConfig(const std::shared_ptr<Config>& config) {
    return std::make_shared<Brain>(config);
}

// Factory function for creating default Config
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

// Factory function for creating AgentBrain
std::shared_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
    return std::make_shared<AgentBrain>(brain);
}

// Factory function for creating SimpleWorld
std::shared_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_shared<SimpleWorld>();
}

// Factory function for creating NeuralRegion
std::shared_ptr<NeuralRegion> createNeuralRegion(const std::string& name) {
    return std::make_shared<NeuralRegion>(name);
}

// Factory function for creating STDP rule
std::shared_ptr<STDP> createSTDPRule(float ltpWeight = 0.01f, float ltdWeight = 0.01f, float tau = 20.0f) {
    auto stdp = std::make_shared<STDP>();
    stdp->configure(ltpWeight, ltdWeight, tau);
    return stdp;
}

// Factory function for creating Hebbian rule
std::shared_ptr<HebbianRule> createHebbianRule(float learningRate = 0.01f) {
    auto hebbian = std::make_shared<HebbianRule>();
    hebbian->setLearningRate(learningRate);
    return hebbian;
}

// Factory function for creating RewardModulatedSTDP
std::shared_ptr<RewardModulatedSTDP> createRewardModulatedSTDP() {
    return std::make_shared<RewardModulatedSTDP>();
}

// Factory function for creating WorkingMemory
std::shared_ptr<NeuralWorkingMemory> createWorkingMemory(size_t capacity = 1000) {
    return std::make_shared<NeuralWorkingMemory>(capacity);
}

// Factory function for creating EpisodicMemory
std::shared_ptr<NeuralEpisodicMemory> createEpisodicMemory(size_t capacity = 100) {
    return std::make_shared<NeuralEpisodicMemory>(capacity);
}

// Factory function for creating Reward modulator
std::shared_ptr<Reward> createRewardModulator(float initialValue = 0.0f, float learningRate = 0.01f) {
    auto reward = std::make_shared<Reward>();
    reward->setCurrentValue(initialValue);
    reward->setLearningRate(learningRate);
    return reward;
}

// Factory function for creating Curiosity module
std::shared_ptr<Curiosity> createCuriosityModule(float initialLevel = 0.5f, float decayRate = 0.01f) {
    auto curiosity = std::make_shared<Curiosity>();
    curiosity->setCurrentLevel(initialLevel);
    curiosity->setDecayRate(decayRate);
    return curiosity;
}

// Factory function for creating PredictionSystem
std::shared_ptr<PredictionSystem> createPredictionSystem() {
    return std::make_shared<PredictionSystem>();
}

PYBIND11_MODULE(_pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Comprehensive Python bindings for the NLM C++ neural simulation framework.
        
        This module provides access to:
        - Core neural components (neurons, synapses, regions, populations)
        - Plasticity rules (STDP, Hebbian, reward-modulated learning)
        - Memory systems (working, episodic, associative memory)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction and cognition systems
        - Development and aging mechanisms
        - Performance optimizations (memory pools, event queues, checkpoints)
        - Advanced simulation controls and utilities
        
        Usage:
            import pynlm
            config = pynlm.create_default_config()
            brain = pynlm.create_brain(config)
            world = pynlm.create_simple_world()
            agent_brain = pynlm.create_agent_brain(brain)
            
            # Configure and run simulation...
            brain.initialize()
            world.configure(100, 100, 50, 50)
        
        The brain operates as a closed-loop system:
        WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
        → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION → ACTION SELECTION
        → MOTOR OUTPUT → WORLD CONSEQUENCE → REWARD/SURPRISE/ERROR
        → PLASTICITY → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Register all the core types first
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

    // Core classes
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

    // Neuron class
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Leaky Integrate-and-Fire neuron)pbdoc")
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
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"))
        .def("getRestingPotential", &Neuron::getRestingPotential)
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"))
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"))
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"))
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"))
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"))
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("getTotalCurrent", &Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent)
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"))
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"))
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses)
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags)
        .def("getPlasticityFlagsRef", &Neuron::getPlasticityFlags)
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"))
        .def("getRegionId", &Neuron::getRegionId)
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"))
        .def("getPopulationId", &Neuron::getPopulationId)
        .def("reset", &Neuron::reset)
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"))
        .def("step", &Neuron::step, py::arg("currentTime"))
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron: " + std::to_string(n.getId().value) + " type=" + 
                   std::to_string(static_cast<int>(n.getType())) + ">";
        });

    // Synapse class
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
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags)
        .def("getPlasticityFlagsRef", &Synapse::getPlasticityFlags)
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("reset", &Synapse::reset)
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: " + std::to_string(s.getId().value) + 
                   " (" + std::to_string(s.getSourceNeuron().value) + " -> " +
                   std::to_string(s.getDestinationNeuron().value) + ")";
        });

    // NeuralRegion class
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Container for neural populations)pbdoc")
        .def(py::init<>(), "Default constructor")
        .def(py::init<std::string>(), py::arg("name"), "Constructor with name")
        .def("getId", &NeuralRegion::getId)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("getName", &NeuralRegion::getName)
        .def("getRegionId", &NeuralRegion::getRegionId)
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("population"))
        .def("removePopulation", &NeuralRegion::removePopulation, py::arg("id"))
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulationIds", &NeuralRegion::getPopulationIds)
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getTotalSynapseCount", &NeuralRegion::getTotalSynapseCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getRefCount", &NeuralRegion::refcount)
        .def("reset", &NeuralRegion::reset)
        .def("__repr__", [](const NeuralRegion& region) {
            return "<NeuralRegion: " + region.getName() + " (" + 
                   std::to_string(region.getPopulationCount()) + " populations, " +
                   std::to_string(region.getTotalNeuronCount()) + " neurons)>";
        });

    // NeuralPopulation class
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Collection of neurons with shared properties)pbdoc")
        .def(py::init<>(), "Default constructor")
        .def(py::init<PopulationId>(), py::arg("id"), "Constructor with ID")
        .def("getId", &NeuralPopulation::getId)
        .def("getRegionId", &NeuralPopulation::getRegionId)
        .def("setRegionId", &NeuralPopulation::setRegionId, py::arg("regionId"))
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount)
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuronId"))
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("neuronId"))
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        .def("getNeurons", &NeuralPopulation::getNeurons)
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount)
        .def("getTotalSynapticConnections", &NeuralPopulation::getTotalSynapticConnections)
        .def("getPopulationType", &NeuralPopulation::getPopulationType)
        .def("setPopulationType", &NeuralPopulation::setPopulationType, py::arg("type"))
        .def("getMaxFiringRate", &NeuralPopulation::getMaxFiringRate)
        .def("setMaxFiringRate", &NeuralPopulation::setMaxFiringRate, py::arg("rate"))
        .def("reset", &NeuralPopulation::reset)
        .def("__repr__", [](const NeuralPopulation& pop) {
            return "<NeuralPopulation: " + std::to_string(pop.getId().value) + 
                   " (" + std::to_string(pop.getNeuronCount()) + " neurons)>";
        });

    // PlasticityRule base class
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for plasticity rules)pbdoc")
        .def("update", &PlasticityRule::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &PlasticityRule::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &PlasticityRule::getName)
        .def("isEnabled", &PlasticityRule::isEnabled)
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"))
        .def("__repr__", [](const PlasticityRule& rule) {
            return "<PlasticityRule: " + std::string(rule.getName()) + ">";
        });

    // HebbianRule class
    py::class_<HebbianRule>(m, "HebbianRule", R"pbdoc(Hebbian plasticity rule: 'neurons that fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("update", &HebbianRule::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &HebbianRule::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &HebbianRule::getName)
        .def("setLearningRate", &HebbianRule::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &HebbianRule::getLearningRate)
        .def("__repr__", [](const HebbianRule& rule) {
            return "<HebbianRule: learning_rate=" + std::to_string(rule.getLearningRate()) + ">";
        });

    // STDP class
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &STDP::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &STDP::getName)
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"))
        .def("getLTPWeight", &STDP::getLTPWeight)
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"))
        .def("getLTDWeight", &STDP::getLTDWeight)
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"))
        .def("getTimeConstant", &STDP::getTimeConstant)
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"))
        .def("__repr__", [](const STDP& stdp) {
            return "<STDP: ltp_weight=" + std::to_string(stdp.getLTPWeight()) + ", ltd_weight=" + 
                   std::to_string(stdp.getLTDWeight()) + ", tau=" + std::to_string(stdp.getTimeConstant()) + ">";
        });

    // RewardModulatedSTDP class
    py::class_<RewardModulatedSTDP>(m, "RewardModulatedSTDP", R"pbdoc(Reward-modulated STDP (R-STDP))pbdoc")
        .def(py::init<>())
        .def("update", &RewardModulatedSTDP::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &RewardModulatedSTDP::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &RewardModulatedSTDP::getName)
        .def("__repr__", [](const RewardModulatedSTDP& rstdp) {
            return "<RewardModulatedSTDP>";
        });

    // TripletSTDP class
    py::class_<TripletSTDP>(m, "TripletSTDP", R"pbdoc(Spike-timing dependent plasticity with triplet interactions)pbdoc")
        .def(py::init<>())
        .def("update", &TripletSTDP::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &TripletSTDP::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &TripletSTDP::getName)
        .def("__repr__", [](const TripletSTDP& triplet) {
            return "<TripletSTDP>";
        });

    // StructuralPlasticity class
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for synapse creation and elimination)pbdoc")
        .def(py::init<>())
        .def("update", &StructuralPlasticity::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &StructuralPlasticity::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &StructuralPlasticity::getName)
        .def("__repr__", [](const StructuralPlasticity& structPlast) {
            return "<StructuralPlasticity>";
        });

    // Memory base class
    py::class_<Memory>(m, "Memory", R"pbdoc(Abstract base class for memory systems)pbdoc")
        .def("store", &Memory::store, py::arg("key"), py::arg("value"))
        .def("retrieve", &Memory::retrieve, py::arg("key"))
        .def("has", &Memory::has, py::arg("key"))
        .def("remove", &Memory::remove, py::arg("key"))
        .def("clear", &Memory::clear)
        .def("getCapacity", &Memory::getCapacity)
        .def("getSize", &Memory::getSize)
        .def("getUtilization", &Memory::getUtilization)
        .def("reset", &Memory::reset)
        .def("__repr__", [](const Memory& memory) {
            return "<Memory: size=" + std::to_string(memory.getSize()) + "/" + 
                   std::to_string(memory.getCapacity()) + ", utilization=" +
                   std::to_string(memory.getUtilization()) + ">";
        });

    // NeuralWorkingMemory class
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Short-term working memory with competitive decay)pbdoc")
        .def(py::init<size_t>(), py::arg("capacity") = 1000)
        .def("store", &NeuralWorkingMemory::store, py::arg("key"), py::arg("value"))
        .def("retrieve", &NeuralWorkingMemory::retrieve, py::arg("key"))
        .def("has", &NeuralWorkingMemory::has, py::arg("key"))
        .def("remove", &NeuralWorkingMemory::remove, py::arg("key"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("getSize", &NeuralWorkingMemory::getSize)
        .def("getUtilization", &NeuralWorkingMemory::getUtilization)
        .def("getAllKeys", &NeuralWorkingMemory::getAllKeys)
        .def("update", [](NeuralWorkingMemory& memory, const std::vector<std::pair<std::string, float>>& updates) {
            for (const auto& update : updates) {
                memory.store(update.first, update.second);
            }
        }, py::arg("updates"))
        .def("reset", &NeuralWorkingMemory::reset)
        .def("__repr__", [](const NeuralWorkingMemory& memory) {
            return "<NeuralWorkingMemory: size=" + std::to_string(memory.getSize()) + "/" +
                   std::to_string(memory.getCapacity()) + ", utilization=" +
                   std::to_string(memory.getUtilization()) + ">";
        });

    // NeuralEpisodicMemory class
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Long-term episodic memory with replay capability)pbdoc")
        .def(py::init<size_t>(), py::arg("capacity") = 100)
        .def("store", &NeuralEpisodicMemory::store, py::arg("key"), py::arg("value"))
        .def("retrieve", &NeuralEpisodicMemory::retrieve, py::arg("key"))
        .def("has", &NeuralEpisodicMemory::has, py::arg("key"))
        .def("remove", &NeuralEpisodicMemory::remove, py::arg("key"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("getCapacity", &NeuralEpisodicMemory::getCapacity)
        .def("getSize", &NeuralEpisodicMemory::getSize)
        .def("getUtilization", &NeuralEpisodicMemory::getUtilization)
        .def("getAllKeys", &NeuralEpisodicMemory::getAllKeys)
        .def("replay", &NeuralEpisodicMemory::replay, py::arg("callback"))
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("importanceThreshold"))
        .def("getConsolidationCount", &NeuralEpisodicMemory::getConsolidationCount)
        .def("reset", &NeuralEpisodicMemory::reset)
        .def("__repr__", [](const NeuralEpisodicMemory& memory) {
            return "<NeuralEpisodicMemory: size=" + std::to_string(memory.getSize()) + "/" +
                   std::to_string(memory.getCapacity()) + ", utilization=" +
                   std::to_string(memory.getUtilization()) + ", consolidated=" +
                   std::to_string(memory.getConsolidationCount()) + ">";
        });

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulators)pbdoc")
        .def("update", &Neuromodulator::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("getCurrentLevel", &Neuromodulator::getCurrentLevel)
        .def("setCurrentLevel", &Neuromodulator::setCurrentLevel, py::arg("level"))
        .def("getLearningRate", &Neuromodulator::getLearningRate)
        .def("setLearningRate", &Neuromodulator::setLearningRate, py::arg("rate"))
        .def("getDecayRate", &Neuromodulator::getDecayRate)
        .def("setDecayRate", &Neuromodulator::setDecayRate, py::arg("rate"))
        .def("reset", &Neuromodulator::reset)
        .def("__repr__", [](const Neuromodulator& neuromodulator) {
            return "<Neuromodulator: level=" + std::to_string(neuromodulator.getCurrentLevel()) + ">";
        });

    // Reward neuromodulator
    py::class_<Reward>(m, "Reward", R"pbdoc(Dopamine-based reward modulator)pbdoc")
        .def(py::init<>())
        .def("update", &Reward::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("setCurrentValue", &Reward::setCurrentValue, py::arg("value"))
        .def("getCurrentValue", &Reward::getCurrentValue)
        .def("getPredictionError", &Reward::getPredictionError, py::arg("predictedReward"))
        .def("setLearningRate", &Reward::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &Reward::getLearningRate)
        .def("reset", &Reward::reset)
        .def("__repr__", [](const Reward& reward) {
            return "<Reward: value=" + std::to_string(reward.getCurrentValue()) + ", prediction_error=" +
                   std::to_string(reward.getPredictionError(0.0f)) + ">";
        });

    // Curiosity neuromodulator
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration-driven curiosity modulator)pbdoc")
        .def(py::init<>())
        .def("update", &Curiosity::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("setCurrentLevel", &Curiosity::setCurrentLevel, py::arg("level"))
        .def("getCurrentLevel", &Curiosity::getCurrentLevel)
        .def("getNoveltySignal", &Curiosity::getNoveltySignal, py::arg("sensoryInput"))
        .def("setDecayRate", &Curiosity::setDecayRate, py::arg("rate"))
        .def("getDecayRate", &Curiosity::getDecayRate)
        .def("reset", &Curiosity::reset)
        .def("__repr__", [](const Curiosity& curiosity) {
            return "<Curiosity: level=" + std::to_string(curiosity.getCurrentLevel()) + ">";
        });

    // Novelty neuromodulator
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Detects novel sensory patterns)pbdoc")
        .def(py::init<>())
        .def("update", &Novelty::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("sensoryInput"))
        .def("getCurrentNovelty", &Novelty::getCurrentNovelty)
        .def("setThreshold", &Novelty::setThreshold, py::arg("threshold"))
        .def("getThreshold", &Novelty::getThreshold)
        .def("reset", &Novelty::reset)
        .def("__repr__", [](const Novelty& novelty) {
            return "<Novelty: current_novelty=" + std::to_string(novelty.getCurrentNovelty()) + ">";
        });

    // PredictionError neuromodulator
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionError::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("calculateError", &PredictionError::calculateError, py::arg("predictedValue"), py::arg("actualValue"))
        .def("getCurrentError", &PredictionError::getCurrentError)
        .def("setLearningRate", &PredictionError::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &PredictionError::getLearningRate)
        .def("reset", &PredictionError::reset)
        .def("__repr__", [](const PredictionError& predError) {
            return "<PredictionError: current_error=" + std::to_string(predError.getCurrentError()) + ">";
        });

    // DevelopmentSystem class
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Manages developmental stages and plasticity changes)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage)
        .def("setCurrentStage", &DevelopmentSystem::setCurrentStage, py::arg("stage"))
        .def("getStageProgress", &DevelopmentSystem::getStageProgress)
        .def("getStageDuration", &DevelopmentSystem::getStageDuration)
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate)
        .def("setPlasticityRate", &DevelopmentSystem::setPlasticityRate, py::arg("rate"))
        .def("isInCriticalPeriod", &DevelopmentSystem::isInCriticalPeriod)
        .def("reset", &DevelopmentSystem::reset)
        .def("__repr__", [](const DevelopmentSystem& devSystem) {
            return "<DevelopmentSystem: stage=" + std::to_string(static_cast<int>(devSystem.getCurrentStage())) + ">";
        });

    // Maturation class
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation and synaptic strengthening)pbdoc")
        .def(py::init<>())
        .def("update", &Maturation::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("getMaturationRate", &Maturation::getMaturationRate)
        .def("setMaturationRate", &Maturation::setMaturationRate, py::arg("rate"))
        .def("getPruningThreshold", &Maturation::getPruningThreshold)
        .def("setPruningThreshold", &Maturation::setPruningThreshold, py::arg("threshold"))
        .def("reset", &Maturation::reset)
        .def("__repr__", [](const Maturation& maturation) {
            return "<Maturation: maturation_rate=" + std::to_string(maturation.getMaturationRate()) + ">";
        });

    // Synaptogenesis class
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Formation of new synapses during development)pbdoc")
        .def(py::init<>())
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("getFormationRate", &Synaptogenesis::getFormationRate)
        .def("setFormationRate", &Synaptogenesis::setFormationRate, py::arg("rate"))
        .def("getPruningThreshold", &Synaptogenesis::getPruningThreshold)
        .def("setPruningThreshold", &Synaptogenesis::setPruningThreshold, py::arg("threshold"))
        .def("reset", &Synaptogenesis::reset)
        .def("__repr__", [](const Synaptogenesis& synaptogenesis) {
            return "<Synaptogenesis: formation_rate=" + std::to_string(synaptogenesis.getFormationRate()) + ">";
        });

    // Pruning class
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Abolition of weak or unused synapses)pbdoc")
        .def(py::init<>())
        .def("update", &Pruning::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("getPruningRate", &Pruning::getPruningRate)
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"))
        .def("getThreshold", &Pruning::getThreshold)
        .def("setThreshold", &Pruning::setThreshold, py::arg("threshold"))
        .def("getMinConnections", &Pruning::getMinConnections)
        .def("setMinConnections", &Pruning::setMinConnections, py::arg("minConnections"))
        .def("reset", &Pruning::reset)
        .def("__repr__", [](const Pruning& pruning) {
            return "<Pruning: pruning_rate=" + std::to_string(pruning.getPruningRate()) + ", threshold=" + 
                   std::to_string(pruning.getThreshold()) + ">";
        });

    // Performance classes
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Efficient memory allocation for neural data)pbdoc")
        .def(py::init<size_t>(), py::arg("initialCapacity") = 1000)
        .def("allocateNeuron", &MemoryPool::allocateNeuron)
        .def("allocateSynapse", &MemoryPool::allocateSynapse)
        .def("deallocateNeuron", &MemoryPool::deallocateNeuron, py::arg("neuronId"))
        .def("deallocateSynapse", &MemoryPool::deallocateSynapse, py::arg("synapseId"))
        .def("getUsedCapacity", &MemoryPool::getUsedCapacity)
        .def("getTotalCapacity", &MemoryPool::getTotalCapacity)
        .def("getUtilization", &MemoryPool::getUtilization)
        .def("reset", &MemoryPool::reset)
        .def("__repr__", [](const MemoryPool& pool) {
            return "<MemoryPool: used=" + std::to_string(pool.getUsedCapacity()) + "/" +
                   std::to_string(pool.getTotalCapacity()) + ", utilization=" +
                   std::to_string(pool.getUtilization()) + ">";
        });

    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event-driven computation system)pbdoc")
        .def(py::init<>())
        .def("pushEvent", &EventQueue::pushEvent, py::arg("event"))
        .def("popEvent", &EventQueue::popEvent)
        .def("size", &EventQueue::size)
        .def("empty", &EventQueue::empty)
        .def("processEvents", &EventQueue::processEvents, py::arg("processor"))
        .def("clear", &EventQueue::clear)
        .def("__repr__", [](const EventQueue& queue) {
            return "<EventQueue: size=" + std::to_string(queue.size()) + ">";
        });

    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Persistence and checkpoint management)pbdoc")
        .def(py::init<>())
        .def("save", &CheckpointSystem::save, py::arg("filepath"), py::arg("brain"))
        .def("load", &CheckpointSystem::load, py::arg("filepath"), py::arg("brain"))
        .def("hasCheckpoint", &CheckpointSystem::hasCheckpoint, py::arg("filepath"))
        .def("getAvailableCheckpoints", &CheckpointSystem::getAvailableCheckpoints)
        .def("setAutoSave", &CheckpointSystem::setAutoSave, py::arg("interval"))
        .def("getAutoSaveInterval", &CheckpointSystem::getAutoSaveInterval)
        .def("clear", &CheckpointSystem::clear)
        .def("__repr__", [](const CheckpointSystem& checkpoint) {
            return "<CheckpointSystem: auto_save_interval=" + std::to_string(checkpoint.getAutoSaveInterval()) + ">";
        });

    py::class_<SparseConnectivity>(m, "SparseConnectivity", R"pbdoc(Efficient sparse neural connectivity)pbdoc")
        .def(py::init<size_t>(), py::arg("maxConnections") = 1000000)
        .def("addConnection", &SparseConnectivity::addConnection, py::arg("source"), py::arg("destination"))
        .def("removeConnection", &SparseConnectivity::removeConnection, py::arg("source"), py::arg("destination"))
        .def("hasConnection", &SparseConnectivity::hasConnection, py::arg("source"), py::arg("destination"))
        .def("getOutgoingConnections", &SparseConnectivity::getOutgoingConnections, py::arg("source"))
        .def("getIncomingConnections", &SparseConnectivity::getIncomingConnections, py::arg("destination"))
        .def("getConnectionCount", &SparseConnectivity::getConnectionCount)
        .def("getMaxConnections", &SparseConnectivity::getMaxConnections)
        .def("getUtilization", &SparseConnectivity::getUtilization)
        .def("clear", &SparseConnectivity::clear)
        .def("__repr__", [](const SparseConnectivity& sparse) {
            return "<SparseConnectivity: connections=" + std::to_string(sparse.getConnectionCount()) + "/" +
                   std::to_string(sparse.getMaxConnections()) + ", utilization=" +
                   std::to_string(sparse.getUtilization()) + ">";
        });

    py::class_<OptimizedBrain>(m, "OptimizedBrain", R"pbdoc(Performance-optimized brain implementation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getOriginalBrain", &OptimizedBrain::getOriginalBrain)
        .def("enableMemoryPool", &OptimizedBrain::enableMemoryPool, py::arg("capacity"))
        .def("enableSparseConnectivity", &OptimizedBrain::enableSparseConnectivity, py::arg("maxConnections"))
        .def("enableEventQueue", &OptimizedBrain::enableEventQueue)
        .def("enableCheckpointSystem", &OptimizedBrain::enableCheckpointSystem, py::arg("interval"))
        .def("getPerformanceStats", &OptimizedBrain::getPerformanceStats)
        .def("__repr__", [](const OptimizedBrain& optimized) {
            return "<OptimizedBrain: performance_optimized>";
        });

    // Cognition classes
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning and action sequence evaluation)pbdoc")
        .def(py::init<>())
        .def("update", &NeuralPlanner::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("evaluateActionSequence", &NeuralPlanner::evaluateActionSequence, py::arg("sequence"))
        .def("getCurrentPlan", &NeuralPlanner::getCurrentPlan)
        .def("setPlanningHorizon", &NeuralPlanner::setPlanningHorizon, py::arg("horizon"))
        .def("getPlanningHorizon", &NeuralPlanner::getPlanningHorizon)
        .def("reset", &NeuralPlanner::reset)
        .def("__repr__", [](const NeuralPlanner& planner) {
            return "<NeuralPlanner: horizon=" + std::to_string(planner.getPlanningHorizon()) + ">";
        });

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
        .def(py::init<>())
        .def("update", &ConceptFormation::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("learnConcept", &ConceptFormation::learnConcept, py::arg("pattern"), py::arg("label"))
        .def("recognizeConcept", &ConceptFormation::recognizeConcept, py::arg("pattern"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("reset", &ConceptFormation::reset)
        .def("__repr__", [](const ConceptFormation& concept) {
            return "<ConceptFormation: concepts=" + std::to_string(concept.getConceptCount()) + ">";
        });

    // Environment classes
    py::class_<Environment>(m, "Environment", R"pbdoc(Abstract base class for environments)pbdoc")
        .def("reset", &Environment::reset)
        .def("step", &Environment::step, py::arg("action"))
        .def("getObservation", &Environment::getObservation)
        .def("isDone", &Environment::isDone)
        .def("getReward", &Environment::getReward)
        .def("getInfo", &Environment::getInfo)
        .def("__repr__", [](const Environment& env) {
            return "<Environment>";
        });

    // Experiment classes
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Abstract base class for experiments)pbdoc")
        .def("run", &Experiment::run)
        .def("reset", &Experiment::reset)
        .def("getMetrics", &Experiment::getMetrics)
        .def("getName", &Experiment::getName)
        .def("getDescription", &Experiment::getDescription)
        .def("__repr__", [](const Experiment& experiment) {
            return "<Experiment: " + experiment.getName() + ">";
        });

    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Runs experiments and collects metrics)pbdoc")
        .def(py::init<>())
        .def("addExperiment", &ExperimentRunner::addExperiment, py::arg("experiment"))
        .def("removeExperiment", &ExperimentRunner::removeExperiment, py::arg("name"))
        .def("runExperiment", &ExperimentRunner::runExperiment, py::arg("name"))
        .def("runAllExperiments", &ExperimentRunner::runAllExperiments)
        .def("getResults", &ExperimentRunner::getResults)
        .def("getExperimentNames", &ExperimentRunner::getExperimentNames)
        .def("clear", &ExperimentRunner::clear)
        .def("__repr__", [](const ExperimentRunner& runner) {
            return "<ExperimentRunner: experiments=" + std::to_string(runner.getExperimentNames().size()) + ">";
        });

    // Core utilities
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Accurate simulation timing)pbdoc")
        .def(py::init<>())
        .def("getTime", &SimulationClock::getTime)
        .def("getStep", &SimulationClock::getStep)
        .def("step", &SimulationClock::step, py::arg("dt"))
        .def("setTimeScale", &SimulationClock::setTimeScale, py::arg("scale"))
        .def("getTimeScale", &SimulationClock::getTimeScale)
        .def("reset", &SimulationClock::reset)
        .def("__repr__", [](const SimulationClock& clock) {
            return "<SimulationClock: time=" + std::to_string(clock.getTime()) + ", step=" + 
                   std::to_string(clock.getStep()) + ">";
        });

    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generation for simulations)pbdoc")
        .def(py::init<uint64_t>(), py::arg("seed") = std::random_device{}())
        .def("randomFloat", &RandomGenerator::randomFloat, py::arg("min") = 0.0f, py::arg("max") = 1.0f)
        .def("randomInt", &RandomGenerator::randomInt, py::arg("min") = 0, py::arg("max") = std::numeric_limits<int>::max())
        .def("randomNeuronId", &RandomGenerator::randomNeuronId)
        .def("randomSynapseId", &RandomGenerator::randomSynapseId)
        .def("randomNormal", &RandomGenerator::randomNormal, py::arg("mean") = 0.0f, py::arg("stddev") = 1.0f)
        .def("resetSeed", &RandomGenerator::resetSeed, py::arg("seed"))
        .def("__repr__", [](const RandomGenerator& rng) {
            return "<RandomGenerator: seed=???>";
        });

    py::class_<Logger>(m, "Logger", R"pbdoc(Logging and debugging utilities)pbdoc")
        .def(py::init<>())
        .def("log", &Logger::log, py::arg("message"))
        .def("debug", &Logger::debug, py::arg("message"))
        .def("info", &Logger::info, py::arg("message"))
        .def("warn", &Logger::warn, py::arg("message"))
        .def("error", &Logger::error, py::arg("message"))
        .def("setLevel", &Logger::setLevel, py::arg("level"))
        .def("getLevel", &Logger::getLevel)
        .def("clear", &Logger::clear)
        .def("__repr__", [](const Logger& logger) {
            return "<Logger: level=???>";
        });

    // Sensory/Motor classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType)
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    py::class_<Vision>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", &Vision::setData, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" + std::to_string(vision.getHeight()) + ">";
        });

    py::class_<Audio>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", &Audio::setData, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + "Hz, " + std::to_string(audio.getNumSamples()) + " samples>";
        });

    py::class_<InternalSignals>(m, "InternalSignals", R"pbdoc(Internal neural signals)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getSignals)
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: size=" + std::to_string(signals.getSignals().size()) + ">";
        });

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " type=" + std::to_string(static_cast<int>(action.getType())) + ">";
        });

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept>";
        });

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
        .def("reset", &AgentBody::reset)
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: x=" + std::to_string(body.x) + ", y=" + std::to_string(body.y) + ">";
        });

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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," + std::to_string(obj.y) + 
                   " type=" + std::to_string(static_cast<int>(obj.type)) + ", radius=" +
                   std::to_string(obj.radius) + ">";
        });

    // World classes
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + ">";
        });

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("__repr__", [](const AgentBrain& agentBrain) {
            return "<AgentBrain>";
        });

    // Brain class (already existing, but we'll update the bindings)
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

    // World classes (already existing)
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

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(System for predictive coding and error signals)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionSystem::update, py::arg("brain"), py::arg("world"), py::arg("dt"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &PredictionSystem::getLearningRate)
        .def("reset", &PredictionSystem::reset)
        .def("__repr__", [](const PredictionSystem& predSys) {
            return "<PredictionSystem: error=" + std::to_string(predSys.getPredictionError()) + ">";
        });

    // Factory functions
    m.def("create_default_config", &createDefaultConfig,
          R"pbdoc(Create a default configuration)pbdoc");

    m.def("create_brain", &createBrainWithConfig, py::arg("config"),
          R"pbdoc(Create a new brain with configuration)pbdoc");

    m.def("create_agent_brain", &createAgentBrain, py::arg("brain"),
          R"pbdoc(Create a new agent brain interface)pbdoc");

    m.def("create_simple_world", &createSimpleWorld,
          R"pbdoc(Create a new simple world)pbdoc");

    m.def("create_neural_region", &createNeuralRegion, py::arg("name"),
          R"pbdoc(Create a new neural region)pbdoc");

    m.def("create_stdp_rule", &createSTDPRule,
          py::arg("ltpWeight") = 0.01f, py::arg("ltdWeight") = 0.01f, py::arg("tau") = 20.0f,
          R"pbdoc(Create an STDP plasticity rule)pbdoc");

    m.def("create_hebbian_rule", &createHebbianRule,
          py::arg("learningRate") = 0.01f,
          R"pbdoc(Create a Hebbian plasticity rule)pbdoc");

    m.def("create_reward_modulated_stdp", &createRewardModulatedSTDP,
          R"pbdoc(Create a reward-modulated STDP rule)pbdoc");

    m.def("create_working_memory", &createWorkingMemory,
          py::arg("capacity") = 1000,
          R"pbdoc(Create a working memory system)pbdoc");

    m.def("create_episodic_memory", &createEpisodicMemory,
          py::arg("capacity") = 100,
          R"pbdoc(Create an episodic memory system)pbdoc");

    m.def("create_reward_modulator", &createRewardModulator,
          py::arg("initialValue") = 0.0f, py::arg("learningRate") = 0.01f,
          R"pbdoc(Create a reward modulator)pbdoc");

    m.def("create_curiosity_module", &createCuriosityModule,
          py::arg("initialLevel") = 0.5f, py::arg("decayRate") = 0.01f,
          R"pbdoc(Create a curiosity module)pbdoc");

    m.def("create_prediction_system", &createPredictionSystem,
          R"pbdoc(Create a prediction system)pbdoc");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Python-style functions and utilities
    m.def("initialize_brain", [](std::shared_ptr<Brain> brain, const std::shared_ptr<SimpleWorld>& world) {
        brain->initialize();
        if (world) {
            world->configure(100, 100, 50, 50);
        }
        return true;
    }, py::arg("brain"), py::arg("world") = nullptr,
        R"pbdoc(Initialize a brain, optionally configuring a world)pbdoc");

    m.def("run_simulation_step", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   const SimulationClock& clock) {
        brain->step(clock.getStep(), clock.getTime());
        if (world) {
            world->update(1.0);  // timestep of 1.0
        }
        return true;
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("clock"),
        R"pbdoc(Perform a single simulation step)pbdoc");

    m.def("save_brain_state", &CheckpointSystem::save,
          py::arg("system"), py::arg("filepath"), py::arg("brain"),
          R"pbdoc(Save brain state to file)pbdoc");

    m.def("load_brain_state", &CheckpointSystem::load,
          py::arg("system"), py::arg("filepath"), py::arg("brain"),
          R"pbdoc(Load brain state from file)pbdoc");

    // Error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    // Version info
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__doc__") = R"pbdoc(
        NLM (熙然) — An Experimental Artificial Developmental Brain
        
        This is a comprehensive Python interface to the NLM C++ neural simulation framework.
        The bindings provide access to core neural components, plasticity rules, memory systems,
        neuromodulation, prediction and cognition systems, development mechanisms,
        performance optimizations, and advanced simulation controls.
        
        The architecture follows a closed-loop brain model:
        WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
        → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION → ACTION SELECTION
        → MOTOR OUTPUT → WORLD CONSEQUENCE → REWARD/SURPRISE/ERROR
        → PLASTICITY → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
    )pbdoc";
}

} // namespace nlm