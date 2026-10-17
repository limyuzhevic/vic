# python/bindings.cpp - Enhanced Python bindings with convenience methods and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

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

// Utility functions for Python convenience

/**
 * @brief Create a default configuration with typical NLM parameters
 * 
 * @return Shared pointer to a default configuration
 * 
 * This function creates a configuration with typical parameters for running
 * NLM simulations, including neuron count, region count, and plasticity settings.
 */
std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set typical NLM simulation parameters
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", static_cast<uint64_t>(42), ConfigSource::Default);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("working_memory_capacity", static_cast<size_t>(100), ConfigSource::Default);
    config->set("episodic_memory_max_episodes", static_cast<size_t>(1000), ConfigSource::Default);
    
    return config;
}

/**
 * @brief Run a simple demonstration of NLM brain simulation
 * 
 * @param config Configuration for the brain simulation
 * @param steps Number of simulation steps to run
 * @return Dictionary with simulation results including neuron counts,
 *         spike statistics, and learning metrics
 */
py::dict runSimpleDemo(const std::shared_ptr<Config>& config, size_t steps = 1000) {
    py::dict results;
    
    try {
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results["error"] = "Failed to initialize brain";
            return results;
        }
        
        // Run simulation steps
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           ", Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                           ", Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        // Collect results
        results["neuron_count"] = brain->getTotalNeuronCount();
        results["synapse_count"] = brain->getTotalSynapseCount();
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        results["avg_firing_rate"] = brain->getAverageFiringRate();
        results["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        results["success"] = true;
        results["message"] = "Simulation completed successfully";
        
    } catch (const std::exception& e) {
        results["error"] = std::string("Exception: ") + e.what();
        results["success"] = false;
    }
    
    return results;
}

/**
 * @brief Create a neural population with specified characteristics
 * 
 * @param brain Pointer to the brain that will contain this population
 * @param size Number of neurons in the population
 * @param neuron_type Type of neurons to create
 * @param region_name Optional name for the region containing the population
 * @return Pair of region ID and population ID
 */
std::pair<RegionId, PopulationId> createNeuralPopulation(
    std::shared_ptr<Brain> brain,
    size_t size,
    NeuronType neuron_type,
    const std::string& region_name = "DefaultRegion") {
    
    // Add a region if none exist
    RegionId region_id;
    if (brain->getRegionCount() == 0) {
        region_id = brain->addRegion(region_name);
    } else {
        // Use existing region
        auto region_ids = brain->getRegionIds();
        region_id = region_ids[0];
    }
    
    // Add population to region
    PopulationId population_id = brain->getRegion(region_id)->addPopulation(size, neuron_type);
    
    return std::make_pair(region_id, population_id);
}

/**
 * @brief Apply a stimulus pattern to a brain for learning demonstration
 * 
 * @param brain Pointer to the brain to stimulate
 * @param pattern Vector of current values to inject
 * @param duration Number of steps to apply the pattern
 */
void applyStimulusPattern(
    std::shared_ptr<Brain> brain,
    const std::vector<float>& pattern,
    size_t duration) {
    
    if (!brain || pattern.empty()) {
        return;
    }
    
    for (size_t step = 0; step < duration; ++step) {
        // Apply current pattern to sensory neurons
        size_t pattern_index = step % pattern.size();
        float current = pattern[pattern_index];
        
        // Inject into sensory neurons
        brain->injectCurrentToNeurons(NeuronType::Sensory, current);
        
        // Run simulation step
        brain->step(static_cast<nlm::SimulationStep>(step));
    }
}

/**
 * @brief Get comprehensive brain statistics as a Python dictionary
 * 
 * @param brain Pointer to the brain to analyze
 * @return Dictionary containing detailed statistics about the brain
 */
py::dict getBrainStatistics(const std::shared_ptr<Brain>& brain) {
    py::dict stats;
    
    if (!brain) {
        stats["error"] = "Brain pointer is null";
        return stats;
    }
    
    stats["neuron_count"] = brain->getTotalNeuronCount();
    stats["synapse_count"] = brain->getTotalSynapseCount();
    stats["active_neurons"] = brain->getActiveNeuronCount();
    stats["firing_neurons"] = brain->getFiringNeuronCount();
    stats["total_spikes"] = brain->getTotalSpikeCount();
    stats["avg_firing_rate"] = brain->getAverageFiringRate();
    stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
    stats["region_count"] = brain->getRegionCount();
    
    // Region details
    auto region_ids = brain->getRegionIds();
    py::list region_list;
    
    for (auto rid : region_ids) {
        py::dict region_info;
        auto* region = brain->getRegion(rid);
        if (region) {
            region_info["id"] = rid.index();
            region_info["neuron_count"] = region->getTotalNeuronCount();
            region_info["synapse_count"] = region->getSynapseCount();
            region_info["active_neurons"] = region->getActiveNeuronCount();
            region_info["avg_weight"] = region->getAverageSynapticWeight();
            region_list.append(region_info);
        }
    }
    
    stats["regions"] = region_list;
    stats["success"] = true;
    
    return stats;
}

/**
 * @brief Create a simple world for NLM agent simulation
 * 
 * @param width World width in arbitrary units
 * @param height World height in arbitrary units
 * @return Shared pointer to SimpleWorld
 */
std::shared_ptr<SimpleWorld> createWorld(float width, float height) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(static_cast<size_t>(width * 10), static_cast<size_t>(height * 10), 80, 60);
    return world;
}

/**
 * @brief Create an agent brain interface
 * 
 * @param brain Pointer to the underlying neural brain
 * @return Shared pointer to AgentBrain
 */
std::shared_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
    return std::make_shared<AgentBrain>(brain);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive Python access to the NLM neural simulation
        system, including:
        
        **Core Components:**
        - Brain: Central neural simulation engine
        - Config: Configuration management
        - Neuron: Individual neurons with LIF dynamics
        - Synapse: Synaptic connections with plasticity
        - NeuralRegion/NeuralPopulation: Organizational hierarchy
        
        **Integrated Systems:**
        - Memory: Working, episodic, and associative memory systems
        - Neuromodulation: Dopamine, curiosity, novelty detection
        - Cognition: Planning, attention, concept formation
        - Development: Synaptogenesis, pruning, maturation
        
        **Environment and Interaction:**
        - SimpleWorld: Simulated environment for agents
        - AgentBrain: Interface between brain and environment
        - SensoryInput: Perceptual inputs
        - Action: Motor outputs
        
        **Advanced Features:**
        - Convenience functions for quick setup
        - Simulation utilities and monitoring
        - Performance optimization tools
        - Example demonstrations
        
        The module is designed to be both easy to use for beginners and
        extensible for advanced users who need custom neural architectures.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // === Basic Types ===
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
        .def("__ne__", &SynapseId::operator!=);

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index);

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index);

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

    // === Core Classes ===

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

    // === Brain Class ===

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

    // === Convenience Functions ===

    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration with typical NLM parameters");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("runSimpleDemo", &runSimpleDemo,
          py::arg("config"), py::arg("steps") = 1000,
          "Run a simple NLM simulation demonstration");

    m.def("createNeuralPopulation", &createNeuralPopulation,
          py::arg("brain"), py::arg("size"), py::arg("neuron_type"),
          py::arg("region_name") = "DefaultRegion",
          "Create a neural population with specified characteristics");

    m.def("applyStimulusPattern", &applyStimulusPattern,
          py::arg("brain"), py::arg("pattern"), py::arg("duration"),
          "Apply a stimulus pattern to a brain for learning demonstration");

    m.def("getBrainStatistics", &getBrainStatistics,
          py::arg("brain"),
          "Get comprehensive brain statistics as a Python dictionary");

    // === Agent and World Classes ===

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

    // === Creation Utilities ===

    m.def("createSimpleWorld", &createSimpleWorld,
          py::arg("width"), py::arg("height"),
          "Create a simple 2D world for NLM simulation");

    m.def("createAgentBrain", &createAgentBrain,
          py::arg("brain"),
          "Create a new agent brain interface");

    // === Global Constants ===

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === Example Functions ===

    m.def("example_basic_brain", []() {
        py::dict example;
        example["description"] = "Basic NLM brain setup";
        example["code"] = R"python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = pynlm.getBrainStatistics(brain)
print(f"Neurons: {stats['neuron_count']}")
print(f"Spikes: {stats['total_spikes']}")
print(f"Activity: {stats['avg_firing_rate']:.3f} Hz"
        );
        return example;
    }, "Example: Basic NLM brain simulation");

    m.def("example_learning_simulation", []() {
        py::dict example;
        example["description"] = "Learning simulation with stimulus patterns";
        example["code"] = R"python
import pynlm
import random

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Apply learning stimulus pattern
pattern = [10.0, 20.0, 15.0, 5.0, 25.0]  # Current injection pattern
pynlm.applyStimulusPattern(brain, pattern, 500)

# Check learning results
stats = pynlm.getBrainStatistics(brain)
print(f"Learning completed!")
print(f"Final activity: {stats['avg_firing_rate']:.3f} Hz")
        );
        return example;
    }, "Example: Learning simulation with stimulus patterns");

    m.def("example_multi_region_brain", []() {
        py::dict example;
        example["description"] = "Multi-region brain with different neuron types";
        example["code"] = R"python
import pynlm

# Create multi-region brain
config = pynlm.createDefaultConfig()
config.set("region_count", 4)  # 4 regions
brain = pynlm.createBrain(config)
brain.initialize()

# Create different populations
region1_pop1 = pynlm.createNeuralPopulation(brain, 100, pynlm.NeuronType.Sensory, "SensoryCortex")
region1_pop2 = pynlm.createNeuralPopulation(brain, 200, pynlm.NeuronType.Internal, "AssociationCortex")
region2_pop1 = pynlm.createNeuralPopulation(brain, 50, pynlm.NeuronType.Motor, "MotorCortex")
region3_pop1 = pynlm.createNeuralPopulation(brain, 150, pynlm.NeuronType.Internal, "VisualCortex")

# Run simulation with regional connectivity
for step in range(1000):
    brain.step(step)

# Analyze by region
stats = pynlm.getBrainStatistics(brain)
print(f"Multi-region brain simulation complete!")
print(f"Total neurons: {stats['neuron_count']}")
print(f"Regions: {stats['region_count']}")
        );
        return example;
    }, "Example: Multi-region brain simulation");

} // namespace nlm
