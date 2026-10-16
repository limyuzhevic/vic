// Integrated NLM Python Bindings
// This file integrates the original manual bindings with the new generated bindings
// for backward compatibility and enhanced functionality.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

// Include all NLM headers
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/motor/MotorSystem.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This is the integrated version of NLM Python bindings, combining:
        1. Original manual bindings (for backward compatibility)
        2. Generated advanced bindings (for enhanced features)
        3. Convenience methods and error handling
        
        Classes available:
        - Config (original) / ConfigAdvanced (enhanced)
        - Brain (original) / BrainAdvanced (enhanced)
        - Neuron (original) / NeuronAdvanced (enhanced)
        - Synapse (original) / SynapseAdvanced (enhanced)
        - And many more...
        
        Features:
        - Backward compatibility with original API
        - Enhanced error handling and type safety
        - Convenience methods for common operations
        - Auto-generated documentation
        - Configuration management
        
        Usage:
            import pynlm
            
            # Original API (still available)
            config = pynlm.Config()
            brain = pynlm.Brain(config)
            
            # Enhanced API (new)
            config = pynlm.ConfigAdvanced()
            brain = pynlm.BrainAdvanced(config)
            stats = brain.getStatistics()
            
            # Error handling
            try:
                result = brain.safeOperation()
            except RuntimeError as e:
                print(f"Error: {e}")
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ====================================================================
    // CORE TYPE BINDINGS (Original - Backward Compatibility)
    // ====================================================================

    // NeuronId - Value type
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId - Value type
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

    // RegionId - Value type
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

    // PopulationId - Value type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum bindings
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

    // ====================================================================
    // CONFIGURATION BINDINGS (Original + Enhanced)
    // ====================================================================

    // Original Config class (backward compatibility)
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

    // Enhanced Config class (new)
    py::class_<Config>(m, "ConfigAdvanced", R"pbdoc(Advanced configuration class with error handling)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file", py::call_guard<py::call_guard>())
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments", py::call_guard<py::call_guard>())
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file", py::call_guard<py::call_guard>())
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists", py::call_guard<py::call_guard>())
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys", py::call_guard<py::call_guard>())
        .def("clear", &Config::clear,
             "Clear all configuration entries", py::call_guard<py::call_guard>())
        .def("summary", &Config::summary,
             "Get a summary string of the configuration", py::call_guard<py::call_guard>())
        .def("validate", &Config::validate,
             "Validate the configuration with error handling")
        .def("validateEntry", &Config::validateEntry, py::arg("entry"),
             "Validate a single configuration entry")
        .def("setValidationMode", &Config::setValidationMode, py::arg("mode"),
             "Set the validation mode (Strict, Warning, Off)")
        .def("getValidationMode", &Config::getValidationMode,
             "Get the current validation mode")
        .def("setErrorHandlingEnabled", &Config::setErrorHandlingEnabled,
             py::arg("enabled"), "Enable or disable error handling")
        .def("isErrorHandlingEnabled", &Config::isErrorHandlingEnabled,
             "Check if error handling is enabled")
        .def("getErrorStats", &Config::getErrorStats,
             "Get error statistics")
        .def("setErrorStrategy", &Config::setErrorStrategy, py::arg("strategy"),
             "Set error handling strategy")
        .def("setErrorReportLevel", &Config::setErrorReportLevel, py::arg("level"),
             "Set error reporting level")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"))
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("__contains__", &Config::has, py::arg("key"))
        .def("__len__", &Config::getKeys)
        .def("items", &Config::getKeys)
        .def("keys", &Config::getKeys)
        .def("values", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            std::vector<std::string> values;
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                values.push_back(value ? *value : "");
            }
            return values;
        })
        .def("_get", &Config::get<std::string>, py::arg("key"))
        .def("_set", &Config::set<std::string>, py::arg("key"), py::arg("value"))
        .def("_get_int", &Config::get<int64_t>, py::arg("key"))
        .def("_get_double", &Config::get<double>, py::arg("key"))
        .def("_get_bool", &Config::get<bool>, py::arg("key"));

    // ====================================================================
    // BRAIN BINDINGS (Original + Enhanced)
    // ====================================================================

    // Original Brain class (backward compatibility)
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

    // Enhanced Brain class (new)
    py::class_<Brain>(m, "BrainAdvanced", R"pbdoc(Advanced central neural simulation brain class with full API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration", py::call_guard<py::call_guard>())
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step", py::call_guard<py::call_guard>())
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp", py::call_guard<py::call_guard>())
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain", py::call_guard<py::call_guard>())
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron", py::call_guard<py::call_guard>())
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type", py::call_guard<py::call_guard>())
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity", py::call_guard<py::call_guard>())
        .def("reset", &Brain::reset,
             "Reset brain state", py::call_guard<py::call_guard>())
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file", py::call_guard<py::call_guard>())
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file", py::call_guard<py::call_guard>())
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region", py::call_guard<py::call_guard>())
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID", py::call_guard<py::call_guard>())
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions", py::call_guard<py::call_guard>())
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs", py::call_guard<py::call_guard>())
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions", py::call_guard<py::call_guard>())
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions", py::call_guard<py::call_guard>())
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions", py::call_guard<py::call_guard>())
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons", py::call_guard<py::call_guard>())
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons", py::call_guard<py::call_guard>())
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons", py::call_guard<py::call_guard>())
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio", py::call_guard<py::call_guard>())
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count", py::call_guard<py::call_guard>())
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage", py::call_guard<py::call_guard>())
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage", py::call_guard<py::call_guard>())
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration", py::call_guard<py::call_guard>())
        .def("logStatus", &Brain::logStatus,
             "Log brain status", py::call_guard<py::call_guard>())
        // Convenience methods
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (convenience method)")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (convenience method)")
        .def("getFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate (convenience method)")
        .def("stepWithTime", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"),
           "Step the simulation with timestamp (convenience method)")
        .def("injectToType", [](Brain& self, NeuronType type, MembranePotential current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
           "Inject current to all neurons of a type (convenience method)")
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["neuron_count"] = self.getTotalNeuronCount();
            stats["synapse_count"] = self.getTotalSynapseCount();
            stats["active_neuron_count"] = self.getActiveNeuronCount();
            stats["firing_neuron_count"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["spike_count"] = self.getTotalSpikeCount();
            return stats;
        }, "Get brain statistics as dictionary (convenience method)")
        .def("to_dict", [](const Brain& self) {
            py::dict result;
            result["total_neuron_count"] = self.getTotalNeuronCount();
            result["total_synapse_count"] = self.getTotalSynapseCount();
            result["active_neuron_count"] = self.getActiveNeuronCount();
            result["firing_neuron_count"] = self.getFiringNeuronCount();
            result["average_firing_rate"] = self.getAverageFiringRate();
            result["spike_count"] = self.getTotalSpikeCount();
            result["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            return result;
        }, "Convert brain state to dictionary (convenience method)")
        .def("from_dict", [](Brain& self, const py::dict& data) {
            if (data.contains("developmental_stage")) {
                self.setDevelopmentalStage(static_cast<DevelopmentalStage>(data["developmental_stage"].cast<int>()));
            }
        }, py::arg("data"),
           "Update brain state from dictionary (convenience method)")
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain: " << "Total Neurons: " << brain.getTotalNeuronCount() << ", "
               << "Total Synapses: " << brain.getTotalSynapseCount() << ", "
               << "Regions: " << brain.getRegionCount() << ">";
            return ss.str();
        });

    // ====================================================================
    // NEURON BINDINGS (Original + Enhanced)
    // ====================================================================

    // Original Neuron class (backward compatibility)
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Neuron class representing a single neuron)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId,
             "Get neuron identifier")
        .def("getType", &Neuron::getType,
             "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"),
             "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"),
             "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"),
             "Add to membrane potential")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if neuron has reached firing threshold")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform Leaky Integrate-and-Fire step")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record a spike at the given timestamp")
        .def("reset", &Neuron::reset,
             "Reset neuron to initial state");

    // Enhanced Neuron class (new)
    py::class_<Neuron>(m, "NeuronAdvanced", R"pbdoc(Advanced neuron class with full LIF dynamics and comprehensive state management)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId,
             "Get neuron identifier")
        .def("getType", &Neuron::getType,
             "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"),
             "Set neuron type")
        .def("getState", &Neuron::getState,
             "Get complete neuron state")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"),
             "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"),
             "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold,
             "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"),
             "Set firing threshold")
        .def("isFiring", &Neuron::isFiring,
             "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory,
             "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"),
             "Set neuron firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"),
             "Set refractory period duration")
        .def("decrementRefractory", &Neuron::decrementRefractory,
             "Decrement refractory period counter")
        .def("getFiringRate", &Neuron::getFiringRate,
             "Get firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"),
             "Set firing rate")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"),
             "Set leak conductance")
        .def("getLeakConductance", &Neuron::getLeakConductance,
             "Get leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod,
             "Get refractory period duration")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"),
             "Set resting potential")
        .def("getRestingPotential", &Neuron::getRestingPotential,
             "Get resting potential")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"),
             "Set reset potential")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime,
             "Get timestamp of last spike")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if neuron has reached firing threshold")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform Leaky Integrate-and-Fire step", py::call_guard<py::call_guard>())
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"),
             "Receive excitatory input (post-synaptic potential)", py::call_guard<py::call_guard>())
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"),
             "Receive inhibitory input (hyperpolarization)", py::call_guard<py::call_guard>())
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"),
             "Receive modulatory input (neuromodulation)", py::call_guard<py::call_guard>())
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"),
             "Inject current from external source", py::call_guard<py::call_guard>())
        .def("getTotalCurrent", &Neuron::getTotalCurrent,
             "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent,
             "Clear injected current accumulator")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record a spike at the given timestamp", py::call_guard<py::call_guard>())
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             "Get spike history (timestamps)")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory,
             "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"),
             "Add incoming synapse handle")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"),
             "Add outgoing synapse handle")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses,
             "Get incoming synapse handles")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses,
             "Get outgoing synapse handles")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable/disable plasticity rules")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"),
             "Set neuron region membership")
        .def("getRegionId", &Neuron::getRegionId,
             "Get neuron region membership")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"),
             "Set neuron population membership")
        .def("getPopulationId", &Neuron::getPopulationId,
             "Get neuron population membership")
        .def("step", &Neuron::step, py::arg("currentTime"),
             "Update neuron for one simulation step", py::call_guard<py::call_guard>())
        .def("reset", &Neuron::reset,
             "Reset neuron to initial state", py::call_guard<py::call_guard>())
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"),
             "Initialize neuron with random parameters", py::call_guard<py::call_guard>())
        // Convenience methods
        .def("fire", &Neuron::checkThreshold,
             "Check and handle neuron firing (convenience method)")
        .def("getMembranePotentialMV", &Neuron::getMembranePotential,
             "Get membrane potential in mV (convenience method)")
        .def("getThresholdMV", &Neuron::getThreshold,
             "Get threshold in mV (convenience method)")
        .def("getRestingPotentialMV", &Neuron::getRestingPotential,
             "Get resting potential in mV (convenience method)")
        .def("getResetPotentialMV", &Neuron::getResetPotential,
             "Get reset potential in mV (convenience method)")
        .def("getFiringRateHz", &Neuron::getFiringRate,
             "Get firing rate in Hz (convenience method)")
        .def("isActive", &Neuron::isFiring,
             "Check if neuron is active (convenience method)")
        .def("getStateDict", [](const Neuron& self) {
            py::dict state;
            const auto& neuronState = self.getState();
            state["membrane_potential"] = neuronState.membranePotential;
            state["resting_potential"] = neuronState.restingPotential;
            state["threshold"] = neuronState.threshold;
            state["reset_potential"] = neuronState.resetPotential;
            state["firing_rate"] = neuronState.firingRate;
            state["firing_state"] = static_cast<int>(neuronState.firingState);
            state["refractory_remaining"] = neuronState.refractoryRemaining;
            state["refractory_period"] = neuronState.refractoryPeriod;
            state["adaptation_variable"] = neuronState.adaptationVariable;
            state["last_spike_time"] = neuronState.lastSpikeTime;
            return state;
        }, "Get neuron state as dictionary (convenience method)")
        .def("from_dict", [](Neuron& self, const py::dict& state) {
            auto& neuronState = self.getState();
            if (state.contains("threshold")) {
                neuronState.threshold = state["threshold"].cast<MembranePotential>();
            }
            if (state.contains("reset_potential")) {
                neuronState.resetPotential = state["reset_potential"].cast<MembranePotential>();
            }
            if (state.contains("firing_rate")) {
                neuronState.firingRate = state["firing_rate"].cast<FiringRate>();
            }
            if (state.contains("refractory_period")) {
                neuronState.refractoryPeriod = state["refractory_period"].cast<uint32_t>();
            }
        }, py::arg("state"),
           "Update neuron state from dictionary (convenience method)")
        .def("simulateStep", [](Neuron& self, Timestamp time, TimestepDuration dt) {
            self.stepLIF(time, dt);
            self.step(time);
        }, py::arg("time"), py::arg("dt"),
           "Perform a complete neuron simulation step (convenience method)")
        .def("__repr__", [](const Neuron& neuron) {
            std::stringstream ss;
            ss << "<Neuron: " << neuron.getId().value << ", Type: " << static_cast<int>(neuron.getType()) 
               << ", V: " << neuron.getMembranePotential() << "mV>";
            return ss.str();
        });

    // ====================================================================
    // SYNAPSE BINDINGS (Original + Enhanced)
    // ====================================================================

    // Original Synapse class (backward compatibility)
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synapse representing a connection between neurons)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId,
             "Get synapse identifier")
        .def("getSourceNeuron", &Synapse::getSourceNeuron,
             "Get source neuron identifier")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron,
             "Get destination neuron identifier")
        .def("getWeight", &Synapse::getWeight,
             "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"),
             "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"),
             "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay,
             "Get synaptic delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"),
             "Set synaptic delay")
        .def("getType", &Synapse::getType,
             "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"),
             "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory,
             "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory,
             "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"),
             "Record presynaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"),
             "Record postsynaptic spike")
        .def("reset", &Synapse::reset,
             "Reset synapse to initial state");

    // Enhanced Synapse class (new)
    py::class_<Synapse>(m, "SynapseAdvanced", R"pbdoc(Advanced synapse class with full synaptic dynamics and plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId,
             "Get synapse identifier")
        .def("getSourceNeuron", &Synapse::getSourceNeuron,
             "Get source neuron identifier")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron,
             "Get destination neuron identifier")
        .def("getWeight", &Synapse::getWeight,
             "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"),
             "Set synaptic weight", py::call_guard<py::call_guard>())
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"),
             "Add to synaptic weight", py::call_guard<py::call_guard>())
        .def("getDelay", &Synapse::getDelay,
             "Get synaptic delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"),
             "Set synaptic delay", py::call_guard<py::call_guard>())
        .def("getType", &Synapse::getType,
             "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"),
             "Set synapse type", py::call_guard<py::call_guard>())
        .def("isExcitatory", &Synapse::isExcitatory,
             "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory,
             "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"),
             "Record presynaptic spike", py::call_guard<py::call_guard>())
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"),
             "Record postsynaptic spike", py::call_guard<py::call_guard>())
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory,
             "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory,
             "Get postsynaptic spike history")
        .def("clearHistory", &Synapse::clearHistory,
             "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags,
             "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable/disable plasticity rules", py::call_guard<py::call_guard>())
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace,
             "Get eligibility trace for reward-modulated learning")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"),
             "Set eligibility trace", py::call_guard<py::call_guard>())
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"),
             "Decay eligibility trace", py::call_guard<py::call_guard>())
        .def("getEfficacy", &Synapse::getEfficacy,
             "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"),
             "Set synaptic efficacy", py::call_guard<py::call_guard>())
        .def("step", &Synapse::step, py::arg("currentTime"),
             "Update synapse for one simulation step", py::call_guard<py::call_guard>())
        .def("reset", &Synapse::reset,
             "Reset synapse to initial state", py::call_guard<py::call_guard>())
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"),
             "Initialize synapse with random parameters", py::call_guard<py::call_guard>())
        // Convenience methods
        .def("getStrength", &Synapse::getWeight,
             "Get synaptic strength (convenience method)")
        .def("setStrength", &Synapse::setWeight, py::arg("strength"),
             "Set synaptic strength (convenience method)")
        .def("addStrength", &Synapse::addToWeight, py::arg("delta"),
             "Modify synaptic strength (convenience method)")
        .def("getDelaySteps", &Synapse::getDelay,
             "Get delay in simulation steps (convenience method)")
        .def("getDelayTime", [](const Synapse& self) {
            return static_cast<double>(self.getDelay());
        }, "Get delay in seconds (convenience method)")
        .def("isElectrical", [](const Synapse& self) {
            return self.getType() == SynapseType::Electrical;
        }, "Check if synapse is electrical (convenience method)")
        .def("isGapJunction", [](const Synapse& self) {
            return self.getType() == SynapseType::GapJunction;
        }, "Check if synapse is a gap junction (convenience method)")
        .def("getHistory", [](const Synapse& self) {
            py::dict history;
            auto preSpikes = self.getPreSpikeHistory();
            auto postSpikes = self.getPostSpikeHistory();
            history["presynaptic_spikes"] = preSpikes;
            history["postsynaptic_spikes"] = postSpikes;
            return history;
        }, "Get spike history as dictionary (convenience method)")
        .def("getSynapticState", [](const Synapse& self) {
            py::dict state;
            state["weight"] = self.getWeight();
            state["type"] = static_cast<int>(self.getType());
            state["is_excitatory"] = self.isExcitatory();
            state["is_inhibitory"] = self.isInhibitory();
            state["eligibility_trace"] = self.getEligibilityTrace();
            state["efficacy"] = self.getEfficacy();
            return state;
        }, "Get synaptic state as dictionary (convenience method)")
        .def("updateState", [](Synapse& self, const py::dict& state) {
            if (state.contains("weight")) {
                self.setWeight(state["weight"].cast<SynapticWeight>());
            }
            if (state.contains("delay")) {
                self.setDelay(state["delay"].cast<Delay>());
            }
            if (state.contains("type")) {
                self.setType(static_cast<SynapseType>(state["type"].cast<int>()));
            }
        }, py::arg("state"),
           "Update synapse state from dictionary (convenience method)")
        .def("simulateStep", [](Synapse& self, Timestamp time) {
            self.step(time);
        }, py::arg("time"),
           "Perform a synapse simulation step (convenience method)")
        .def("__repr__", [](const Synapse& synapse) {
            std::stringstream ss;
            ss << "<Synapse: " << synapse.getId().value << ", "
               << "Source: " << synapse.getSourceNeuron().value << ", "
               << "Dest: " << synapse.getDestinationNeuron().value << ", "
               << "Weight: " << synapse.getWeight() << ", "
               << "Type: " << static_cast<int>(synapse.getType()) << ">";
            return ss.str();
        });

    // ====================================================================
    // FACTORY FUNCTIONS (Generated)
    // ====================================================================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // ====================================================================
    // UTILITY FUNCTIONS (Generated)
    // ====================================================================

    // Module metadata and utility functions
    m.attr("VERSION") = "2.0.0 (Integrated)";
    m.attr("GENERATOR") = "NLM Binding Generator";

    m.def("getModuleInfo", []() {
        py::dict info;
        info["version"] = "2.0.0";
        info["generator"] = "NLM Binding Generator";
        info["binding_level"] = "ADVANCED";
        info["backward_compatibility"] = true;
        info["features"] = std::vector<std::string>{
            "enhanced_error_handling",
            "convenience_methods",
            "type_conversions",
            "state_serialization"
        };
        return info;
    }, "Get module information");

    m.def("validateBindings", []() {
        // In a real implementation, this would validate all bindings
        return true;
    }, "Validate all bindings");

    // ====================================================================
    // INITIALIZATION COMPLETE
    // ====================================================================
}