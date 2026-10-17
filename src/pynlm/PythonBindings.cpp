// Enhanced Python bindings for NLM (Neural Learning Machine) C++ project using pybind11
// Provides comprehensive bindings for core neural simulation framework
// Location: src/pynlm/PythonBindings.cpp
// Created: 2026-10-17

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all the NLM headers that we need to bind
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "src/core/Logger/Logger.hpp"

#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralRegion.hpp"
#include "src/brain/NeuralPopulation.hpp"

#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"

#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Reward.hpp"

#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"

#include "src/environment/Environment.hpp"
#include "src/environment/Observation.hpp"
#include "src/world/SimpleWorld.hpp"

#include "src/dynamics/SpikeSystem.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"

#include "src/development/DevelopmentSystem.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"

namespace py = pybind11;
namespace nlm = ::nlm;

// Initialize logging system when module is imported
void initialize_logging() {
    auto& logger = nlm::Logger::getInstance();
    logger.setLevel(nlm::LogLevel::INFO);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides bindings for core neural simulation, memory, learning, and cognition systems.
        Supports the easy_usage.md examples and API.
    )pbdoc";

    // Initialize logging when module is imported
    m.def("initialize_logging", &initialize_logging, "Initialize logging system");

    // Register exception translations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // === Core Types and Configuration ===

    // Neuron and Synapse IDs
    py::class_<nlm::NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value)
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value)
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value)
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__ne__", &nlm::RegionId::operator!=)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value)
        .def("index", &nlm::PopulationId::index)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__ne__", &nlm::PopulationId::operator!=);

    // Enums
    py::enum_<nlm::NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    py::enum_<nlm::SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();

    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    py::enum_<nlm::FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();

    // Config class with JSON/YAML support
    py::class_<nlm::Config>(m, "Config", R"pbdoc(Configuration class for NLM system with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // === Brain System ===

    // Brain class - main interface for neural computation
    py::class_<nlm::Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class implementing spiking neural computation)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step), py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step), py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &nlm::Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &nlm::Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &nlm::Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &nlm::Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset, "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("add_region", &nlm::Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("get_region", &nlm::Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &nlm::Brain::getRegionCount, "Get the number of regions")
        .def("get_region_ids", &nlm::Brain::getRegionIds, "Get all region IDs")
        .def("get_regions", &nlm::Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("get_total_neuron_count", &nlm::Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("get_total_synapse_count", &nlm::Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("get_active_neuron_count", &nlm::Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("get_firing_neuron_count", &nlm::Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("get_average_firing_rate", &nlm::Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &nlm::Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_developmental_stage", &nlm::Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("set_developmental_stage", &nlm::Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("get_config", &nlm::Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("log_status", &nlm::Brain::logStatus, "Log brain status")
        // Memory systems access
        .def("get_working_memory", &nlm::Brain::getWorkingMemory, py::return_value_policy::reference_internal, "Get working memory")
        .def("get_episodic_memory", &nlm::Brain::getEpisodicMemory, py::return_value_policy::reference_internal, "Get episodic memory")
        .def("get_associative_memory", &nlm::Brain::getAssociativeMemory, py::return_value_policy::reference_internal, "Get associative memory")
        // Prediction system
        .def("get_prediction_system", &nlm::Brain::getPredictionSystem, py::return_value_policy::reference_internal, "Get prediction system")
        .def("get_planner", &nlm::Brain::getPlanner, py::return_value_policy::reference_internal, "Get neural planner")
        .def("get_concept_formation", &nlm::Brain::getConceptFormation, py::return_value_policy::reference_internal, "Get concept formation")
        .def("get_attention", &nlm::Brain::getAttention, py::return_value_policy::reference_internal, "Get attentional selection")
        // Development system
        .def("get_development_system", &nlm::Brain::getDevelopmentSystem, py::return_value_policy::reference_internal, "Get development system")
        // Neuromodulation systems
        .def("get_dopamine", &nlm::Brain::getDopamine, py::return_value_policy::reference_internal, "Get dopamine neuromodulator")
        .def("get_curiosity", &nlm::Brain::getCuriosity, py::return_value_policy::reference_internal, "Get curiosity neuromodulator")
        .def("get_novelty", &nlm::Brain::getNovelty, py::return_value_policy::reference_internal, "Get novelty neuromodulator")
        .def("get_prediction_error", &nlm::Brain::getPredictionErrorSignal, py::return_value_policy::reference_internal, "Get prediction error signal")
        // Other systems
        .def("get_spike_system", &nlm::Brain::getSpikeSystem, py::return_value_policy::reference_internal, "Get spike system")
        .def("get_stdp", &nlm::Brain::getSTDP, py::return_value_policy::reference_internal, "Get STDP plasticity")
        .def("get_hebbian", &nlm::Brain::getHebbian, py::return_value_policy::reference_internal, "Get Hebbian plasticity")
        .def("get_structural_plasticity", &nlm::Brain::getStructuralPlasticity, py::return_value_policy::reference_internal, "Get structural plasticity")
        .def("get_random_generator", &nlm::Brain::getRandomGenerator, py::return_value_policy::reference_internal, "Get random generator")
        .def("get_logger", []() -> nlm::Logger* { return &nlm::Logger::getInstance(); }, py::return_value_policy::reference_internal, "Get logger instance");

    // Neuron class
    py::class_<nlm::Neuron>(m, "Neuron", R"pbdoc(Neuron class representing a single neuron with LIF dynamics)pbdoc")
        .def(py::init<nlm::NeuronId>(), py::arg("id"))
        .def("get_id", &nlm::Neuron::getId)
        .def("get_type", &nlm::Neuron::getType)
        .def("set_type", &nlm::Neuron::setType, py::arg("type"))
        .def("get_state", &nlm::Neuron::getState, py::return_value_policy::reference_internal)
        .def("is_firing", &nlm::Neuron::isFiring)
        .def("is_refractory", &nlm::Neuron::isRefractory)
        .def("get_firing_rate", &nlm::Neuron::getFiringRate)
        .def("set_firing_rate", &nlm::Neuron::setFiringRate, py::arg("rate"))
        .def("check_threshold", &nlm::Neuron::checkThreshold)
        .def("step_lif", &nlm::Neuron::stepLIF, py::arg("current_time"), py::arg("dt"))
        .def("record_spike", &nlm::Neuron::recordSpike, py::arg("timestamp"))
        .def("get_spike_history", &nlm::Neuron::getSpikeHistory)
        .def("reset", &nlm::Neuron::reset)
        .def("initialize_random", &nlm::Neuron::initializeRandom, py::arg("rng"));

    // Synapse class
    py::class_<nlm::Synapse>(m, "Synapse", R"pbdoc(Synapse representing a connection between neurons with transmission delays)pbdoc")
        .def(py::init<nlm::SynapseId, nlm::NeuronId, nlm::NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("get_id", &nlm::Synapse::getId)
        .def("get_source_neuron", &nlm::Synapse::getSourceNeuron)
        .def("get_destination_neuron", &nlm::Synapse::getDestinationNeuron)
        .def("get_weight", &nlm::Synapse::getWeight)
        .def("set_weight", &nlm::Synapse::setWeight, py::arg("weight"))
        .def("add_to_weight", &nlm::Synapse::addToWeight, py::arg("delta"))
        .def("get_delay", &nlm::Synapse::getDelay)
        .def("set_delay", &nlm::Synapse::setDelay, py::arg("delay"))
        .def("get_type", &nlm::Synapse::getType)
        .def("set_type", &nlm::Synapse::setType, py::arg("type"))
        .def("is_excitatory", &nlm::Synapse::isExcitatory)
        .def("is_inhibitory", &nlm::Synapse::isInhibitory)
        .def("record_pre_spike", &nlm::Synapse::recordPreSpike, py::arg("timestamp"))
        .def("record_post_spike", &nlm::Synapse::recordPostSpike, py::arg("timestamp"))
        .def("get_pre_spike_history", &nlm::Synapse::getPreSpikeHistory)
        .def("get_post_spike_history", &nlm::Synapse::getPostSpikeHistory)
        .def("get_plasticity_flags", &nlm::Synapse::getPlasticityFlags, py::return_value_policy::reference_internal)
        .def("enable_plasticity", &nlm::Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"))
        .def("get_eligibility_trace", &nlm::Synapse::getEligibilityTrace)
        .def("set_eligibility_trace", &nlm::Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decay_eligibility_trace", &nlm::Synapse::decayEligibilityTrace, py::arg("decay_rate"))
        .def("get_efficacy", &nlm::Synapse::getEfficacy)
        .def("set_efficacy", &nlm::Synapse::setEfficacy, py::arg("efficacy"))
        .def("reset", &nlm::Synapse::reset)
        .def("initialize_random", &nlm::Synapse::initializeRandom, py::arg("rng"));

    // NeuralPopulation class
    py::class_<nlm::NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population of neurons with shared properties)pbdoc")
        .def(py::init<nlm::PopulationId, size_t, nlm::NeuronType>(), py::arg("id"), py::arg("size"), py::arg("type") = nlm::NeuronType::Internal)
        .def("get_id", &nlm::NeuralPopulation::getId)
        .def("get_type", &nlm::NeuralPopulation::getType)
        .def("get_neuron_count", &nlm::NeuralPopulation::getNeuronCount)
        .def("get_neuron", &nlm::NeuralPopulation::getNeuron, py::return_value_policy::reference_internal)
        .def("add_neuron", &nlm::NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("reset", &nlm::NeuralPopulation::reset)
        .def("step", &nlm::NeuralPopulation::step, py::arg("current_time"));

    // NeuralRegion class
    py::class_<nlm::NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region containing multiple populations and local connectivity)pbdoc")
        .def(py::init<nlm::RegionId>(), py::arg("id"))
        .def(py::init<nlm::RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("get_id", &nlm::NeuralRegion::getId)
        .def("get_name", &nlm::NeuralRegion::getName)
        .def("set_name", &nlm::NeuralRegion::setName, py::arg("name"))
        .def("add_population", &nlm::NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = nlm::NeuronType::Internal)
        .def("get_population", &nlm::NeuralRegion::getPopulation, py::return_value_policy::reference_internal)
        .def("get_population_count", &nlm::NeuralRegion::getPopulationCount)
        .def("get_total_neuron_count", &nlm::NeuralRegion::getTotalNeuronCount)
        .def("add_synapse", &nlm::NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("get_synapse", &nlm::NeuralRegion::getSynapse, py::return_value_policy::reference_internal)
        .def("get_synapse_count", &nlm::NeuralRegion::getSynapseCount)
        .def("step", &nlm::NeuralRegion::step, py::arg("current_time"))
        .def("reset", &nlm::NeuralRegion::reset)
        .def("initialize_random_connectivity", &nlm::NeuralRegion::initializeRandomConnectivity,
              py::arg("rng"), py::arg("connection_probability"), py::arg("mean_weight"), py::arg("weight_variance"))
        .def("get_all_neurons", &nlm::NeuralRegion::getAllNeurons)
        .def("get_average_synaptic_weight", &nlm::NeuralRegion::getAverageSynapticWeight)
        .def("get_synaptic_density", &nlm::NeuralRegion::getSynapticDensity);

    // === Memory Systems ===

    // NeuralWorkingMemory
    py::class_<nlm::NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory using neural dynamics and persistent activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &nlm::NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("store_to_neuron", &nlm::NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &nlm::NeuralWorkingMemory::retrieve)
        .def("contains", &nlm::NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("get_neuron_activation", &nlm::NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &nlm::NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &nlm::NeuralWorkingMemory::clear)
        .def("get_active_traces", &nlm::NeuralWorkingMemory::getActiveTraces)
        .def("get_capacity", &nlm::NeuralWorkingMemory::getCapacity)
        .def("set_capacity", &nlm::NeuralWorkingMemory::setCapacity, py::arg("cap"))
        .def("get_decay_rate", &nlm::NeuralWorkingMemory::getDecayRate)
        .def("set_decay_rate", &nlm::NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("get_memory_neurons", &nlm::NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthen_memory", &nlm::NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("run_competition", &nlm::NeuralWorkingMemory::runCompetition)
        .def("is_winning", &nlm::NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("get_memory_activity", &nlm::NeuralWorkingMemory::getMemoryActivity);

    // NeuralEpisodicMemory
    py::class_<nlm::NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory storing experience sequences)pbdoc")
        .def(py::init<>())
        .def("store_event", &nlm::NeuralEpisodicMemory::storeEvent, py::arg("timestamp"), py::arg("input_pattern"), py::arg("action"))
        .def("retrieve_sequence", &nlm::NeuralEpisodicMemory::retrieveSequence, py::arg("start_time"), py::arg("duration"))
        .def("get_recent_events", &nlm::NeuralEpisodicMemory::getRecentEvents, py::arg("num_events"))
        .def("clear", &nlm::NeuralEpisodicMemory::clear)
        .def("get_event_count", &nlm::NeuralEpisodicMemory::getEventCount)
        .def("get_capacity", &nlm::NeuralEpisodicMemory::getCapacity)
        .def("set_capacity", &nlm::NeuralEpisodicMemory::setCapacity, py::arg("capacity"))
        .def("update", &nlm::NeuralEpisodicMemory::update, py::arg("dt"))
        .def("get_memory_usage", &nlm::NeuralEpisodicMemory::getMemoryUsage);

    // NeuralAssociativeMemory
    py::class_<nlm::NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory for pattern associations and completion)pbdoc")
        .def(py::init<>())
        .def("associate_patterns", &nlm::NeuralAssociativeMemory::associatePatterns,
              py::arg("pattern_a"), py::arg("pattern_b"), py::arg("strength") = 1.0f)
        .def("recall_pattern", &nlm::NeuralAssociativeMemory::recallPattern, py::arg("input_pattern"))
        .def("get_associations", &nlm::NeuralAssociativeMemory::getAssociations, py::arg("pattern"))
        .def("clear", &nlm::NeuralAssociativeMemory::clear)
        .def("get_association_count", &nlm::NeuralAssociativeMemory::getAssociationCount)
        .def("get_pattern_count", &nlm::NeuralAssociativeMemory::getPatternCount)
        .def("update", &nlm::NeuralAssociativeMemory::update, py::arg("dt"))
        .def("get_connection_strength", &nlm::NeuralAssociativeMemory::getConnectionStrength,
              py::arg("pattern_a"), py::arg("pattern_b"));

    // === Neuromodulation ===

    // Neuromodulator base class
    py::class_<nlm::Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("get_name", &nlm::Neuromodulator::getName, "Get modulator name")
        .def("get_level", &nlm::Neuromodulator::getLevel, "Get current concentration/level")
        .def("set_level", &nlm::Neuromodulator::setLevel, py::arg("level"), "Set current concentration/level")
        .def("get_plasticity_factor", &nlm::Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &nlm::Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Dopamine
    py::class_<nlm::Dopamine, nlm::Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signal_reward", &nlm::Dopamine::signalReward, py::arg("reward"))
        .def("signal_reward_prediction_error", &nlm::Dopamine::signalRewardPredictionError, py::arg("error"));

    // Curiosity
    py::class_<nlm::Curiosity, nlm::Neuromodulator>(m, "Curiosity", R"pbdoc(Exploration motivation and novelty seeking)pbdoc")
        .def(py::init<>())
        .def("update_curiosity", &nlm::Curiosity::updateCuriosity, py::arg("sensory_input"), py::arg("novelty"))
        .def("get_exploration_drive", &nlm::Curiosity::getExplorationDrive);

    // Novelty
    py::class_<nlm::Novelty, nlm::Neuromodulator>(m, "Novelty", R"pbdoc(Novelty detection and surprise signaling)pbdoc")
        .def(py::init<>())
        .def("update_novelty", &nlm::Novelty::updateNovelty, py::arg("input_pattern"))
        .def("get_novelty_score", &nlm::Novelty::getNoveltyScore);

    // PredictionError
    py::class_<nlm::PredictionError, nlm::Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("update_prediction", &nlm::PredictionError::updatePrediction,
              py::arg("expected"), py::arg("actual"))
        .def("get_prediction_error", &nlm::PredictionError::getPredictionError);

    // === Agent System ===

    // AgentBrain
    py::class_<nlm::AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &nlm::AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("get_motor_output_size", &nlm::AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("process_sensory_input", &nlm::AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &nlm::AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &nlm::AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward"), "Apply reward-based neuromodulation")
        .def("update_development", &nlm::AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &nlm::AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("get_neuromodulation_level", &nlm::AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("get_curiosity_level", &nlm::AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &nlm::AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &nlm::AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &nlm::AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enable_reward_modulation", &nlm::AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enable_structural_plasticity", &nlm::AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enable_development", &nlm::AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enable_curiosity", &nlm::AgentBrain::enableCuriosity, py::arg("enable"))
        .def("is_reward_modulation_enabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &nlm::AgentBrain::isCuriosityEnabled);

    // AgentBody
    py::class_<nlm::AgentBody>(m, "AgentBody", R"pbdoc(Agent body state and position)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("velocity_x", &nlm::AgentBody::velocityX)
        .def_readwrite("velocity_y", &nlm::AgentBody::velocityY)
        .def_readwrite("angular_velocity", &nlm::AgentBody::angularVelocity)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age)
        .def_readwrite("is_moving", &nlm::AgentBody::isMoving)
        .def_readwrite("is_turning", &nlm::AgentBody::isTurning)
        .def_readwrite("last_action_time", &nlm::AgentBody::lastActionTime)
        .def("reset", &nlm::AgentBody::reset);

    // SensoryPercept
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data from world)pbdoc")
        .def(py::init<>())
        .def("get_vision", &nlm::SensoryPercept::getVision, py::return_value_policy::reference_internal)
        .def("set_vision", &nlm::SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &nlm::SensoryPercept::getVisionWidth)
        .def("get_vision_height", &nlm::SensoryPercept::getVisionHeight)
        .def("get_touch", &nlm::SensoryPercept::getTouch)
        .def("set_touch", &nlm::SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &nlm::SensoryPercept::getInternal)
        .def("set_internal", &nlm::SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &nlm::SensoryPercept::getProprioception)
        .def("set_proprioception", &nlm::SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &nlm::SensoryPercept::getAudio)
        .def("set_audio", &nlm::SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &nlm::SensoryPercept::getAllSignals)
        .def("get_timestamp", &nlm::SensoryPercept::getTimestamp)
        .def("set_timestamp", &nlm::SensoryPercept::setTimestamp, py::arg("timestamp"));

    // === Environment ===

    // Environment
    py::class_<nlm::Environment>(m, "Environment", R"pbdoc(Base class for environments)pbdoc")
        .def("reset", &nlm::Environment::reset)
        .def("step", &nlm::Environment::step, py::arg("action"))
        .def("get_state", &nlm::Environment::getState, py::return_value_policy::reference_internal)
        .def("get_last_reward", &nlm::Environment::getLastReward)
        .def("is_done", &nlm::Environment::isDone)
        .def("get_name", &nlm::Environment::getName)
        .def("get_action_space_size", &nlm::Environment::getActionSpaceSize)
        .def("get_observation_space_size", &nlm::Environment::getObservationSpaceSize);

    // SimpleWorld
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &nlm::SimpleWorld::reset)
        .def("set_agent_start", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &nlm::SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &nlm::SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal)
        .def("get_agent_body", &nlm::SimpleWorld::getAgentBody, py::return_value_policy::reference_internal)
        .def("add_object", &nlm::SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &nlm::SimpleWorld::getWidth)
        .def("get_height", &nlm::SimpleWorld::getHeight)
        .def("get_max_energy", &nlm::SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &nlm::SimpleWorld::getSimulationTime)
        .def("set_random_seed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &nlm::SimpleWorld::getRandomSeed);

    // WorldObject
    py::class_<nlm::WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active);

    // === Helper Classes ===

    // RandomGenerator
    py::class_<nlm::RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def("seed", &nlm::RandomGenerator::seed, py::arg("seed"))
        .def("get_seed", &nlm::RandomGenerator::getSeed)
        .def("uniform_int", static_cast<uint32_t (nlm::RandomGenerator::*)(uint32_t, uint32_t)>(&nlm::RandomGenerator::uniformInt),
             py::arg("min"), py::arg("max"))
        .def("uniform_int", static_cast<uint64_t (nlm::RandomGenerator::*)(uint64_t, uint64_t)>(&nlm::RandomGenerator::uniformInt),
             py::arg("min"), py::arg("max"))
        .def("uniform_real", static_cast<double (nlm::RandomGenerator::*)(double, double)>(&nlm::RandomGenerator::uniformReal),
             py::arg("min"), py::arg("max"))
        .def("uniform_real", static_cast<float (nlm::RandomGenerator::*)(float, float)>(&nlm::RandomGenerator::uniformReal),
             py::arg("min"), py::arg("max"))
        .def("normal", static_cast<double (nlm::RandomGenerator::*)(double, double)>(&nlm::RandomGenerator::normal),
             py::arg("mean"), py::arg("stddev"))
        .def("normal", static_cast<float (nlm::RandomGenerator::*)(float, float)>(&nlm::RandomGenerator::normal),
             py::arg("mean"), py::arg("stddev"))
        .def("bernoulli", &nlm::RandomGenerator::bernoulli, py::arg("p") = 0.5)
        .def("poisson", &nlm::RandomGenerator::poisson, py::arg("lambda"))
        .def("exponential", &nlm::RandomGenerator::exponential, py::arg("lambda"))
        .def("random_bool", &nlm::RandomGenerator::randomBool)
        .def("random_bit", &nlm::RandomGenerator::randomBit)
        .def("shuffle", &nlm::RandomGenerator::shuffle, py::arg("container"))
        .def("pick_random", &nlm::RandomGenerator::pickRandom, py::arg("container"))
        .def("pick_random_index", &nlm::RandomGenerator::pickRandomIndex, py::arg("n"))
        .def("fill_bytes", &nlm::RandomGenerator::fillBytes, py::arg("data"), py::arg("size"));

    // SimulationClock
    py::class_<nlm::SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for managing time in neural simulation)pbdoc")
        .def(py::init<>(), "Default constructor with 1ms timestep")
        .def(py::init<nlm::TimestepDuration>(), py::arg("timestep"), "Constructor with specified timestep")
        .def("reset", &nlm::SimulationClock::reset)
        .def("advance", &nlm::SimulationClock::advance)
        .def("get_step", &nlm::SimulationClock::getStep)
        .def("set_step", &nlm::SimulationClock::setStep, py::arg("step"))
        .def("get_time", &nlm::SimulationClock::getTime)
        .def("set_time", &nlm::SimulationClock::setTime, py::arg("time"))
        .def("get_timestep", &nlm::SimulationClock::getTimestep)
        .def("set_timestep", &nlm::SimulationClock::setTimestep, py::arg("dt"))
        .def("is_first_step", &nlm::SimulationClock::isFirstStep)
        .def("is_last_step", &nlm::SimulationClock::isLastStep, py::arg("total_steps"))
        .def("step_to_time", &nlm::SimulationClock::stepToTime, py::arg("step"))
        .def("time_to_step", &nlm::SimulationClock::timeToStep, py::arg("time"))
        .def("advance_by", &nlm::SimulationClock::advanceBy, py::arg("steps"))
        .def("time_at_future_step", &nlm::SimulationClock::timeAtFutureStep, py::arg("steps_ahead"))
        .def("get_real_time_elapsed", &nlm::SimulationClock::getRealTimeElapsed)
        .def("set_real_time_start", &nlm::SimulationClock::setRealTimeStart);

    // Logger
    py::class_<nlm::Logger>(m, "Logger", R"pbdoc(Logging system for NLM)")
        .def_static("get_instance", &nlm::Logger::getInstance, py::return_value_policy::reference_internal)
        .def("set_level", &nlm::Logger::setLevel, py::arg("level"))
        .def("get_level", &nlm::Logger::getLevel)
        .def("log", &nlm::Logger::log, py::arg("message"), py::arg("level") = nlm::LogLevel::INFO)
        .def("is_enabled", &nlm::Logger::isEnabled, py::arg("level"))
        .def("enable_logging", &nlm::Logger::enableLogging, py::arg("enable"));

    // === Plasticity Rules ===

    // STDP
    py::class_<nlm::STDP>(m, "STDP", R"pbdoc(Spike-Timing Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("update", &nlm::STDP::update, py::arg("pre_spike"), py::arg("post_spike"))
        .def("get_trace_pre", &nlm::STDP::getTracePre, py::arg("neuron_id"))
        .def("get_trace_post", &nlm::STDP::getTracePost, py::arg("neuron_id"))
        .def("reset", &nlm::STDP::reset)
        .def("set_learning_rate", &nlm::STDP::setLearningRate, py::arg("rate"))
        .def("get_learning_rate", &nlm::STDP::getLearningRate)
        .def("set_time_window", &nlm::STDP::setTimeWindow, py::arg("pre_window"), py::arg("post_window"));

    // Hebbian
    py::class_<nlm::Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("update", &nlm::Hebbian::update, py::arg("pre_spike"), py::arg("post_spike"))
        .def("get_synapse_weight", &nlm::Hebbian::getSynapseWeight, py::arg("pre_id"), py::arg("post_id"))
        .def("set_learning_rate", &nlm::Hebbian::setLearningRate, py::arg("rate"))
        .def("get_learning_rate", &nlm::Hebbian::getLearningRate)
        .def("reset", &nlm::Hebbian::reset);

    // StructuralPlasticity
    py::class_<nlm::StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for forming new connections)pbdoc")
        .def(py::init<>())
        .def("update", &nlm::StructuralPlasticity::update, py::arg("pre_spike"), py::arg("post_spike"))
        .def("add_synapse", &nlm::StructuralPlasticity::addSynapse, py::arg("pre_id"), py::arg("post_id"), py::arg("weight") = 1.0f)
        .def("remove_synapse", &nlm::StructuralPlasticity::removeSynapse, py::arg("synapse_id"))
        .def("get_synapse_count", &nlm::StructuralPlasticity::getSynapseCount)
        .def("set_growth_rate", &nlm::StructuralPlasticity::setGrowthRate, py::arg("rate"))
        .def("get_growth_rate", &nlm::StructuralPlasticity::getGrowthRate)
        .def("set_pruning_rate", &nlm::StructuralPlasticity::setPruningRate, py::arg("rate"))
        .def("get_pruning_rate", &nlm::StructuralPlasticity::getPruningRate);

    // === Development System ===

    // DevelopmentSystem
    py::class_<nlm::DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Brain development over time)pbdoc")
        .def(py::init<>())
        .def("update", &nlm::DevelopmentSystem::update, py::arg("dt"))
        .def("get_developmental_stage", &nlm::DevelopmentSystem::getDevelopmentalStage)
        .def("set_developmental_stage", &nlm::DevelopmentSystem::setDevelopmentalStage, py::arg("stage"))
        .def("get_plasticity_modifier", &nlm::DevelopmentSystem::getPlasticityModifier)
        .def("set_plasticity_modifier", &nlm::DevelopmentSystem::setPlasticityModifier, py::arg("modifier"))
        .def("is_critical_period", &nlm::DevelopmentSystem::isCriticalPeriod)
        .def("enable_critical_period", &nlm::DevelopmentSystem::enableCriticalPeriod, py::arg("enable"))
        .def("get_developmental_age", &nlm::DevelopmentSystem::getDevelopmentalAge)
        .def("get_maturity_level", &nlm::DevelopmentSystem::getMaturityLevel);

    // === Cognitive Systems ===

    // NeuralPlanner
    py::class_<nlm::NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("plan_action", &nlm::NeuralPlanner::planAction, py::arg("sensory_input"))
        .def("get_action_preferences", &nlm::NeuralPlanner::getActionPreferences)
        .def("get_selected_action", &nlm::NeuralPlanner::getSelectedAction)
        .def("update", &nlm::NeuralPlanner::update, py::arg("dt"))
        .def("reset", &nlm::NeuralPlanner::reset)
        .def("set_planning_horizon", &nlm::NeuralPlanner::setPlanningHorizon, py::arg("horizon"))
        .def("get_planning_horizon", &nlm::NeuralPlanner::getPlanningHorizon)
        .def("enable_referential_planning", &nlm::NeuralPlanner::enableReferentialPlanning, py::arg("enable"));

    // ConceptFormation
    py::class_<nlm::ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("process_input", &nlm::ConceptFormation::processInput, py::arg("input_pattern"))
        .def("get_concepts", &nlm::ConceptFormation::getConcepts)
        .def("get_concept_similarity", &nlm::ConceptFormation::getConceptSimilarity, py::arg("concept_a"), py::arg("concept_b"))
        .def("update", &nlm::ConceptFormation::update, py::arg("dt"))
        .def("reset", &nlm::ConceptFormation::reset)
        .def("set_concept_count", &nlm::ConceptFormation::setConceptCount, py::arg("count"))
        .def("get_concept_count", &nlm::ConceptFormation::getConceptCount)
        .def("get_active_concepts", &nlm::ConceptFormation::getActiveConcepts)
        .def("get_concept_activity", &nlm::ConceptFormation::getConceptActivity, py::arg("concept_id"));

    // === Dynamics Systems ===

    // SpikeSystem
    py::class_<nlm::SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike generation and delivery system)pbdoc")
        .def(py::init<>())
        .def("get_neuron_spikes", &nlm::SpikeSystem::getNeuronSpikes, py::arg("neuron_id"))
        .def("get_all_spikes", &nlm::SpikeSystem::getAllSpikes)
        .def("get_spike_count", &nlm::SpikeSystem::getSpikeCount)
        .def("get_total_spike_count", &nlm::SpikeSystem::getTotalSpikeCount)
        .def("reset", &nlm::SpikeSystem::reset)
        .def("set_refractory_period", &nlm::SpikeSystem::setRefractoryPeriod, py::arg("period"))
        .def("get_refractory_period", &nlm::SpikeSystem::getRefractoryPeriod)
        .def("set_threshold", &nlm::SpikeSystem::setThreshold, py::arg("threshold"))
        .def("get_threshold", &nlm::SpikeSystem::getThreshold);

    // === Global Functions ===

    // Create default configuration
    m.def("create_default_config", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    // Create brain with configuration
    m.def("create_brain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Create simple world
    m.def("create_simple_world", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    // Create agent brain interface
    m.def("create_agent_brain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // Expose core types for type checking
    py::class_<nlm::NeuronState>(m, "NeuronState")
        .def(py::init<>())  // Default constructor
        .def_readwrite("membrane_potential", &nlm::NeuronState::membranePotential)
        .def_readwrite("resting_potential", &nlm::NeuronState::restingPotential)
        .def_readwrite("threshold", &nlm::NeuronState::threshold)
        .def_readwrite("reset_potential", &nlm::NeuronState::resetPotential)
        .def_readwrite("firing_state", &nlm::NeuronState::firingState)
        .def_readwrite("firing_rate", &nlm::NeuronState::firingRate)
        .def_readwrite("refractory_remaining", &nlm::NeuronState::refractoryRemaining)
        .def_readwrite("last_spike_time", &nlm::NeuronState::lastSpikeTime);

    // Error types
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("DEBUG", nlm::LogLevel::DEBUG)
        .value("INFO", nlm::LogLevel::INFO)
        .value("WARNING", nlm::LogLevel::WARNING)
        .value("ERROR", nlm::LogLevel::ERROR)
        .value("CRITICAL", nlm::LogLevel::CRITICAL)
        .export_values();

    // Type conversion helpers
    py::class_<nlm::SensoryInput>(m, "SensoryInput", py::abstract())  // Abstract base class
        .def("get_type", &nlm::SensoryInput::getType)
        .def("get_data", &nlm::SensoryInput::getData)
        .def("get_dimensions", &nlm::SensoryInput::getDimensions)
        .def("get_timestamp", &nlm::SensoryInput::getTimestamp);

    // Vision sensory input
    py::class_<nlm::Vision, nlm::SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("set_data", [](nlm::Vision& self, const std::vector<float>& data) { self.setData(data); }, py::arg("data"))
        .def("get_width", &nlm::Vision::getWidth)
        .def("get_height", &nlm::Vision::getHeight)
        .def("get_channels", &nlm::Vision::getChannels);

    // Audio sensory input
    py::class_<nlm::Audio, nlm::SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](nlm::Audio& self, const std::vector<float>& data) { self.setData(data); }, py::arg("data"))
        .def("set_sample_rate", &nlm::Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &nlm::Audio::getSampleRate)
        .def("get_num_samples", &nlm::Audio::getNumSamples);

    // Internal signals
    py::class_<nlm::InternalSignals, nlm::SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("add_signal", &nlm::InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &nlm::InternalSignals::clearSignals);

    // Motor action
    py::class_<nlm::Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &nlm::Action::getType)
        .def("set_type", &nlm::Action::setType, py::arg("type"))
        .def("get_parameters", &nlm::Action::getParameters)
        .def("set_parameters", &nlm::Action::setParameters, py::arg("params"))
        .def("get_name", &nlm::Action::getName)
        .def("clone", &nlm::Action::clone);

    // Action result
    py::class_<nlm::ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message);

    // Neuron IDs
    py::class_<nlm::NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value)
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // Synapse IDs
    py::class_<nlm::SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value)
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Region IDs
    py::class_<nlm::RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value)
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__ne__", &nlm::RegionId::operator!=)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // Population IDs
    py::class_<nlm::PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value)
        .def("index", &nlm::PopulationId::index)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__ne__", &nlm::PopulationId::operator!=);

    // Add AttentionalSelection
    py::class_<nlm::AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection for focus)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::AttentionalSelection::initialize, py::arg("brain"))
        .def("process_competition", &nlm::AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("global_inhibition") = 0.5f)
        .def("focus_on_region", &nlm::AttentionalSelection::focusOnRegion, py::arg("region"))
        .def("release_attention", &nlm::AttentionalSelection::releaseAttention)
        .def("get_attended_regions", &nlm::AttentionalSelection::getAttendedRegions)
        .def("set_inhibition_strength", &nlm::AttentionalSelection::setInhibitionStrength, py::arg("strength"))
        .def("set_excitation_strength", &nlm::AttentionalSelection::setExcitationStrength, py::arg("strength"))
        .def("set_competition_threshold", &nlm::AttentionalSelection::setCompetitionThreshold, py::arg("threshold"))
        .def("get_inhibition_for", &nlm::AttentionalSelection::getInhibitionFor, py::arg("neuron"))
        .def("get_excitation_for", &nlm::AttentionalSelection::getExcitationFor, py::arg("neuron"))
        .def("update", &nlm::AttentionalSelection::update, py::arg("dt"))
        .def("is_attended", &nlm::AttentionalSelection::isAttended, py::arg("neuron"))
        .def("apply_top_down_bias", &nlm::AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("bias_strength"))
        .def("apply_bottom_up_salience", &nlm::AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salience_strength"))
        .def("reset", &nlm::AttentionalSelection::reset);

    // Add ActionType and MotorCommand enums
    py::enum_<nlm::ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();

    py::enum_<nlm::MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // Add global constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
}
