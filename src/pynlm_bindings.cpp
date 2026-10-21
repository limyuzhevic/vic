// NLM Python Bindings - Core module
// Provides Python API for NLM (Neural Learning Machine) brain simulation

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM - Neural Learning Machine Python Bindings";
    m.attr("__version__") = "0.1.0";
    
    // Configure Python output for better error reporting
    py::register_ostream_redirector();
    
    // Suppress warnings during binding
    py::import("warnings").attr("filterwarnings")("ignore", category=py::import("pybind11").attr("error"));
    
    // Forward declarations
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set, 
             py::arg("key"), py::arg("value"), 
             "Set configuration value", py::arg("source") = "")
        .def("get", &nlm::Config::get<nlm::Config::ConfigValue>, 
             py::arg("key"), "Get configuration value")
        .def("getOr", &nlm::Config::getOr<nlm::Config::ConfigValue>, 
             py::arg("key"), py::arg("defaultValue"), 
             "Get configuration value with default")
        .def("has", &nlm::Config::has, py::arg("key"), 
             "Check if configuration has key")
        .def("loadFromFile", &nlm::Config::loadFromFile, 
             py::arg("filepath"), "Load configuration from file")
        .def("saveToFile", &nlm::Config::saveToFile, 
             py::arg("filepath"), "Save configuration to file")
        .def("getKeys", &nlm::Config::getKeys, 
             "Get all configuration keys");
    
    // Enums
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Internal", nlm::NeuronType::Internal)
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .export_values();
    
    py::enum_<nlm::SynapseType>(m, "SynapseType")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .export_values();
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    // Neuron class
    py::class_<nlm::Neuron>(m, "Neuron")
        .def(py::init<nlm::NeuronId>(), py::arg("id") = nlm::NeuronId())
        .def("setType", &nlm::Neuron::setType, py::arg("type"), "Set neuron type")
        .def("setMembranePotential", &nlm::Neuron::setMembranePotential, 
             py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &nlm::Neuron::addToMembranePotential, 
             py::arg("delta"), "Add to membrane potential")
        .def("setThreshold", &nlm::Neuron::setThreshold, 
             py::arg("threshold"), "Set threshold")
        .def("checkThreshold", &nlm::Neuron::checkThreshold, 
             "Check if neuron has reached threshold")
        .def("getMembranePotential", &nlm::Neuron::getMembranePotential, 
             "Get membrane potential")
        .def("getThreshold", &nlm::Neuron::getThreshold, 
             "Get threshold")
        .def("getType", &nlm::Neuron::getType, "Get neuron type")
        .def("recordSpike", &nlm::Neuron::recordSpike, 
             py::arg("timestamp"), "Record spike at timestamp")
        .def("getSpikeHistory", &nlm::Neuron::getSpikeHistory, 
             "Get spike history")
        .def("clearSpikeHistory", &nlm::Neuron::clearSpikeHistory, 
             "Clear spike history")
        .def("stepLIF", &nlm::Neuron::stepLIF, 
             py::arg("currentTime"), py::arg("dt"), 
             "Step LIF dynamics", py::arg("fired") = false)
        .def("step", &nlm::Neuron::step, 
             py::arg("currentTime"), "Step neuron")
        .def("injectCurrent", &nlm::Neuron::injectCurrent, 
             py::arg("current"), "Inject current")
        .def("reset", &nlm::Neuron::reset, "Reset neuron state")
        .def("initializeRandom", &nlm::Neuron::initializeRandom, 
             py::arg("rng"), "Initialize neuron with random values")
        .def("getId", &nlm::Neuron::getId, "Get neuron ID")
        .def("getFiringState", &nlm::Neuron::getFiringState, 
             "Get firing state")
        .def("getLastSpikeTime", &nlm::Neuron::getLastSpikeTime, 
             "Get last spike time");
    
    // Synapse class
    py::class_<nlm::Synapse>(m, "Synapse")
        .def(py::init<nlm::SynapseId, nlm::NeuronId, nlm::NeuronId>(), 
             py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &nlm::Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &nlm::Synapse::getSourceNeuron, 
             "Get source neuron ID")
        .def("getDestinationNeuron", &nlm::Synapse::getDestinationNeuron, 
             "Get destination neuron ID")
        .def("getWeight", &nlm::Synapse::getWeight, "Get weight")
        .def("setWeight", &nlm::Synapse::setWeight, 
             py::arg("weight"), "Set weight")
        .def("addToWeight", &nlm::Synapse::addToWeight, 
             py::arg("delta"), "Add to weight")
        .def("getDelay", &nlm::Synapse::getDelay, "Get delay")
        .def("setDelay", &nlm::Synapse::setDelay, 
             py::arg("delay"), "Set delay")
        .def("getType", &nlm::Synapse::getType, "Get synapse type")
        .def("setType", &nlm::Synapse::setType, 
             py::arg("type"), "Set synapse type")
        .def("isExcitatory", &nlm::Synapse::isExcitatory, 
             "Check if excitatory")
        .def("isInhibitory", &nlm::Synapse::isInhibitory, 
             "Check if inhibitory")
        .def("recordPreSpike", &nlm::Synapse::recordPreSpike, 
             py::arg("timestamp"), "Record pre-synaptic spike")
        .def("recordPostSpike", &nlm::Synapse::recordPostSpike, 
             py::arg("timestamp"), "Record post-synaptic spike")
        .def("getPreSpikeHistory", &nlm::Synapse::getPreSpikeHistory, 
             "Get pre-synaptic spike history")
        .def("getPostSpikeHistory", &nlm::Synapse::getPostSpikeHistory, 
             "Get post-synaptic spike history")
        .def("clearHistory", &nlm::Synapse::clearHistory, 
             "Clear spike history")
        .def("getEligibilityTrace", &nlm::Synapse::getEligibilityTrace, 
             "Get eligibility trace")
        .def("setEligibilityTrace", &nlm::Synapse::setEligibilityTrace, 
             py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &nlm::Synapse::decayEligibilityTrace, 
             py::arg("decayRate"), "Decay eligibility trace")
        .def("getEfficacy", &nlm::Synapse::getEfficacy, "Get efficacy")
        .def("setEfficacy", &nlm::Synapse::setEfficacy, 
             py::arg("efficacy"), "Set efficacy")
        .def("step", &nlm::Synapse::step, 
             py::arg("currentTime"), "Step synapse dynamics")
        .def("reset", &nlm::Synapse::reset, "Reset synapse state")
        .def("initializeRandom", &nlm::Synapse::initializeRandom, 
             py::arg("rng"), "Initialize synapse randomly")
        .def("enablePlasticity", &nlm::Synapse::enablePlasticity, 
             py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity mechanisms");
    
    // STDP class
    py::class_<nlm::STDP>(m, "STDP")
        .def(py::init<>())
        .def("update", &nlm::STDP::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), 
             py::arg("dt"), "Update STDP based on spike timing")
        .def("applyWeightChange", &nlm::STDP::applyWeightChange,
             py::arg("synapse"), py::arg("delta"), 
             "Apply weight change")
        .def("getName", &nlm::STDP::getName, "Get name")
        .def("getLTPWeight", &nlm::STDP::getLTPWeight, 
             "Get LTP weight")
        .def("setLTPWeight", &nlm::STDP::setLTPWeight, 
             py::arg("weight"), "Set LTP weight")
        .def("getLTDWeight", &nlm::STDP::getLTDWeight, 
             "Get LTD weight")
        .def("setLTDWeight", &nlm::STDP::setLTDWeight, 
             py::arg("weight"), "Set LTD weight")
        .def("getTimeConstant", &nlm::STDP::getTimeConstant, 
             "Get time constant")
        .def("setTimeConstant", &nlm::STDP::setTimeConstant, 
             py::arg("tau"), "Set time constant")
        .def("configure", &nlm::STDP::configure,
             py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters");
    
    // Helper functions
    m.def("createDefaultConfig", &nlm::createDefaultConfig,
          "Create default configuration");
    
    m.def("createBrain", &nlm::createBrain<nlm::Brain>,
          py::arg("config"), "Create brain with configuration");
    
    // Factory functions for convenience
    m.def("createSimpleWorld", &nlm::createSimpleWorld<nlm::SimpleWorld>,
          "Create simple world");
    
    m.def("createAgentBrain", &nlm::createAgentBrain<nlm::AgentBrain>,
          py::arg("brain"), "Create agent brain interface");
    
    // Create brain factory template
    py::class_<nlm::BrainFactory>(m, "BrainFactory")
        .def_static("create", &nlm::BrainFactory::create,
                   py::arg("config"), "Create brain using factory");
    
    // Random generator
    py::class_<nlm::RandomGenerator>(m, "RandomGenerator")
        .def(py::init<uint64_t>(), py::arg("seed"), "Create random generator")
        .def("uniformReal", &nlm::RandomGenerator::uniformReal,
             py::arg("min"), py::arg("max"), "Generate uniform random float")
        .def("uniformInt", &nlm::RandomGenerator::uniformInt,
             py::arg("min"), py::arg("max"), "Generate uniform random int")
        .def("normal", &nlm::RandomGenerator::normal,
             py::arg("mean"), py::arg("stddev"), "Generate normal random float")
        .def("setSeed", &nlm::RandomGenerator::setSeed,
             py::arg("seed"), "Set random seed");
    
    // Logger
    py::class_<nlm::Logger>(m, "Logger")
        .def_static("getInstance", &nlm::Logger::getInstance, 
                   "Get logger instance")
        .def("setLevel", &nlm::Logger::setLevel,
             py::arg("level"), "Set log level");
    
    // Performance monitoring
    m.def("getVersion", []() { return "0.1.0"; }, 
          "Get NLM Python bindings version");
    
    // Additional utility functions
    m.def("listAvailableFunctions", []() {
        py::list functions;
        // Add helper functions for advanced usage
        return functions;
    }, "List available helper functions");
    
    // Advanced configuration
    py::class_<nlm::Phase6Config>(m, "Phase6Config")
        .def(py::init<>())
        .def_readwrite("neuronCount", &nlm::Phase6Config::neuronCount)
        .def_readwrite("maxSteps", &nlm::Phase6Config::maxSteps)
        .def_readwrite("regionCount", &nlm::Phase6Config::regionCount)
        .def_readwrite("connectionProbability", 
                      &nlm::Phase6Config::connectionProbability)
        .def_readwrite("enableCheckpointing", 
                      &nlm::Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &nlm::Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &nlm::Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &nlm::Phase6Config::checkpointPath);
    
    // Integration results
    py::class_<nlm::Phase6IntegrationResult>(m, "Phase6IntegrationResult")
        .def(py::init<>())
        .def_readonly("totalReward", &nlm::Phase6IntegrationResult::totalReward)
        .def_readonly("avgFiringRate", &nlm::Phase6IntegrationResult::avgFiringRate)
        .def_readonly("memoryEpisodesStored", 
                     &nlm::Phase6IntegrationResult::memoryEpisodesStored)
        .def_readonly("dopamineLevel", &nlm::Phase6IntegrationResult::dopamineLevel)
        .def_readonly("curiosityLevel", &nlm::Phase6IntegrationResult::curiosityLevel)
        .def_readonly("noveltyLevel", &nlm::Phase6IntegrationResult::noveltyLevel)
        .def_readonly("neuromodulationIntegrated", 
                     &nlm::Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readonly("memoryWorkingMemoryIntegrated", 
                     &nlm::Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readonly("memoryEpisodicMemoryIntegrated", 
                     &nlm::Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readonly("predictionIntegrated", 
                     &nlm::Phase6IntegrationResult::predictionIntegrated)
        .def_readonly("developmentIntegrated", 
                     &nlm::Phase6IntegrationResult::developmentIntegrated)
        .def_readonly("checkpointingWorks", 
                     &nlm::Phase6IntegrationResult::checkpointingWorks)
        .def_readonly("totalWallClockTime", 
                     &nlm::Phase6IntegrationResult::totalWallClockTime);
    
    // Complete experiment runner
    py::class_<nlm::Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment")
        .def(py::init<>())
        .def("run", &nlm::Phase6IntegratedExperiment::run,
             py::arg("config"), "Run Phase 6 integration experiment")
        .def("verifyIntegration", &nlm::Phase6IntegratedExperiment::verifyIntegration,
             "Verify integration of all systems");
    
    // Error types
    py::register_exception<nlm::NLMException>(m, "NLMException", PyExc_RuntimeError);
    
    // Add __all__ to control what's exported
    m.attr("__all__") = py::list({
        "Config", "Neuron", "Synapse", "STDP", "Brain", "SimpleWorld",
        "AgentBrain", "RandomGenerator", "Logger", "Phase6Config",
        "Phase6IntegrationResult", "Phase6IntegratedExperiment",
        "NeuronType", "SynapseType", "DevelopmentalStage",
        "createDefaultConfig", "createBrain", "createSimpleWorld",
        "createAgentBrain", "BrainFactory", "getVersion"
    });
}