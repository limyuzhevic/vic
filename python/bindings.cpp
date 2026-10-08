#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>

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

// Type aliases for cleaner Python API
typedef std::variant<float, int, bool, std::string> ConfigValue;
typedef std::map<std::string, ConfigValue> ConfigMap;

// Forward declarations
class PythonBrainHelper;

// Advanced configuration class for expert users
class AdvancedConfig {
public:
    AdvancedConfig() {
        // Set up advanced configuration with sensible defaults
        config->set("neuron_count", 1000, ConfigSource::User);
        config->set("region_count", 1, ConfigSource::User);
        config->set("connection_probability", 0.1f, ConfigSource::User);
        config->set("simulation_timestep", 0.001, ConfigSource::User);
        
        // Advanced settings for expert users
        config->set("enable_stdp", true, ConfigSource::User);
        config->set("enable_hebbian", true, ConfigSource::User);
        config->set("enable_reward_modulation", true, ConfigSource::User);
        config->set("enable_structural_plasticity", true, ConfigSource::User);
        config->set("enable_development", true, ConfigSource::User);
        config->set("enable_curiosity", true, ConfigSource::User);
        
        // Performance settings
        config->set("performance_mode", "balanced", ConfigSource::User);
        config->set("max_memory_usage", "1GB", ConfigSource::User);
        
        // Advanced plasticity parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::User);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::User);
        config->set("stdp_tau", 20.0f, ConfigSource::User);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::User);
        config->set("pruning_rate", 0.00001f, ConfigSource::User);
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Convenience methods for common configuration patterns
    void setupStandardResearchConfig() {
        config->set("neuron_count", 500, ConfigSource::User);
        config->set("region_count", 2, ConfigSource::User);
        config->set("connection_probability", 0.15f, ConfigSource::User);
        config->set("enable_reward_modulation", true, ConfigSource::User);
        config->set("enable_curiosity", true, ConfigSource::User);
    }
    
    void setupHighPerformanceConfig() {
        config->set("neuron_count", 2000, ConfigSource::User);
        config->set("region_count", 1, ConfigSource::User);
        config->set("connection_probability", 0.05f, ConfigSource::User);
        config->set("simulation_timestep", 0.002, ConfigSource::User);
        config->set("enable_structural_plasticity", false, ConfigSource::User); // Skip for performance
    }
    
    void setupDevelopmentalConfig() {
        config->set("enable_development", true, ConfigSource::User);
        config->set("developmental_stage", "critical_period", ConfigSource::User);
        config->set("plasticity_modifier", 1.5f, ConfigSource::User);
    }
    
private:
    std::shared_ptr<Config> config = std::make_shared<Config>();
};

// Enhanced brain simulation runner for expert users
class AdvancedSimulationRunner {
public:
    static void runEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agent,
                          size_t numSteps,
                          const std::function<void(size_t, float, const SensoryPercept&)>& callback = nullptr,
                          bool recordMetrics = true) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid brain, world, or agent for episode simulation");
        }
        
        // Reset systems for new episode
        brain->reset();
        world->reset();
        agent->reset();
        
        if (recordMetrics) {
            brain->getPredictionSystem()->resetMetrics();
            agent->enableRewardModulation(true);
        }
        
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            double currentTime = world->getSimulationTime();
            world->update(0.1);
            
            // Get sensory percept
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process input through agent
            agent->processSensoryInput(percept);
            
            // Brain computation
            brain->step(static_cast<SimulationStep>(step));
            
            // Decode motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, currentTime);
            
            // Get reward from world
            auto result = world->getActionResult();
            if (result) {
                agent->applyRewardModulation(result->reward, agent->getPredictionError());
            }
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Callback for custom monitoring
            if (callback) {
                callback(step, result ? result->reward : 0.0f, percept);
            }
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                             ", Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                             ", Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        if (recordMetrics) {
            brain->getPredictionSystem()->finalizeMetrics();
        }
        
        NLM_LOG_INFO("Episode completed: " + std::to_string(numSteps) + " steps");
    }
    
    static void runBatchSimulation(size_t numEpisodes, size_t stepsPerEpisode,
                                   std::shared_ptr<Config> config = nullptr,
                                   const std::function<void(size_t, size_t)>& progressCallback = nullptr) {
        if (!config) {
            config = std::make_shared<Config>();
        }
        
        std::vector<std::map<std::string, float>> episodeMetrics;
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            if (progressCallback) {
                progressCallback(episode, numEpisodes);
            }
            
            // Create new brain for each episode
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Run episode
            runEpisode(brain, world, agent, stepsPerEpisode,
                      [&, episode](size_t step, float reward, const SensoryPercept&) {
                          // Collect metrics
                          std::map<std::string, float> metrics;
                          metrics["episode"] = static_cast<float>(episode);
                          metrics["step"] = static_cast<float>(step);
                          metrics["reward"] = reward;
                          metrics["firing_neurons"] = static_cast<float>(brain->getFiringNeuronCount());
                          metrics["total_spikes"] = static_cast<float>(brain->getTotalSpikeCount());
                          metrics["avg_firing_rate"] = brain->getAverageFiringRate();
                          metrics["development_stage"] = static_cast<float>(agent->getDevelopmentalStage());
                          metrics["dopamine_level"] = agent->getNeuromodulationLevel();
                          metrics["curiosity_level"] = agent->getCuriosityLevel();
                          
                          episodeMetrics.push_back(metrics);
                      });
            
            // Log episode summary
            NLM_LOG_INFO("Episode " + std::to_string(episode) + " completed. " +
                         "Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                         ", Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
        
        NLM_LOG_INFO("Batch simulation completed: " + std::to_string(numEpisodes) + " episodes");
    }
};

// Visualization helper for expert users
class VisualizationHelper {
public:
    static void saveNeuralConnectivityMap(std::shared_ptr<Brain> brain, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "# Neural Connectivity Map" << std::endl;
        file << "# Generated by NLM Python Bindings" << std::endl;
        file << std::endl;
        
        file << "Nodes:" << std::endl;
        for (const auto& region : brain->getRegions()) {
            file << "  Region " << region->getId().index() << ": " << region->getName() << ", "
                 << region->getTotalNeuronCount() << " neurons, "
                 << region->getSynapseCount() << " synapses" << std::endl;
        }
        
        file << std::endl << "Synapses:" << std::endl;
        size_t synapseCount = 0;
        for (const auto& region : brain->getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                file << "  Synapse " << synapseCount << ": "
                     << "neuron_" << syn->getSourceNeuron().index() << " -> "
                     << "neuron_" << syn->getDestinationNeuron().index() << ", "
                     << "weight: " << syn->getWeight() << ", "
                     << "delay: " << syn->getDelay() << " steps" << std::endl;
                synapseCount++;
            }
        }
        
        file << std::endl << "Statistics:" << std::endl;
        file << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        file << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        file.close();
        NLM_LOG_INFO("Neural connectivity map saved to: " + filename);
    }
    
    static std::string generateNeuralTopologyReport(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "=== Neural Topology Report ===" << std::endl;
        ss << "Brain Configuration:" << std::endl;
        ss << "  Regions: " << brain->getRegionCount() << std::endl;
        ss << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        ss << std::endl << "Regional Breakdown:" << std::endl;
        for (const auto& region : brain->getRegions()) {
            ss << "  " << region->getName() << ":" << std::endl;
            ss << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
            ss << "    Synapses: " << region->getSynapseCount() << std::endl;
            ss << "    Average weight: " << region->getAverageSynapticWeight() << std::endl;
            ss << "    Populations: " << region->getPopulationCount() << std::endl;
        }
        
        ss << std::endl << "Network Properties:" << std::endl;
        ss << "  Connection density: " << (brain->getTotalSynapseCount() / 
                                         static_cast<double>(brain->getTotalNeuronCount() * 
                                                           brain->getTotalNeuronCount())) * 100.0f << " %" << std::endl;
        ss << "  Sparseness: " << (1.0f - 
                               static_cast<float>(brain->getTotalSynapseCount()) /
                               static_cast<float>(brain->getTotalNeuronCount() * 
                                                 brain->getTotalNeuronCount())) * 100.0f << " %" << std::endl;
        
        return ss.str();
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double simulationTime;
        size_t stepsCompleted;
        size_t neuronsCreated;
        size_t synapsesCreated;
        size_t totalSpikes;
        double memoryUsage;
        double cpuUsage;
    };
    
    static PerformanceMetrics collectMetrics() {
        PerformanceMetrics metrics;
        // Collect real-time performance data
        // This would integrate with system monitoring tools
        return metrics;
    }
    
    static void profileSimulation(std::shared_ptr<Brain> brain, size_t steps,
                                  const std::string& profileFile = "profile.txt") {
        // Profile simulation performance
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::ofstream file(profileFile);
        if (file.is_open()) {
            file << "Simulation Profile" << std::endl;
            file << "===================" << std::endl;
            file << "Steps completed: " << steps << std::endl;
            file << "Total time: " << duration.count() << " ms" << std::endl;
            file << "Average time per step: " << (duration.count() / static_cast<double>(steps)) << " ms" << std::endl;
            file << "Speed: " << (steps / (duration.count() / 1000.0)) << " steps/second" << std::endl;
            file << std::endl;
            file << "Brain Statistics:" << std::endl;
            file << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
            file << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
            file << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            file << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            
            file.close();
            NLM_LOG_INFO("Profile saved to: " + profileFile);
        }
    }
};

// Python-specific helper functions
class PythonBrainHelper {
public:
    static py::tuple runExperiment(const std::string& experimentType,
                                   std::shared_ptr<AdvancedConfig> config,
                                   size_t numSteps = 1000) {
        auto brain = std::make_shared<Brain>(config->getConfig());
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable features based on experiment type
        if (experimentType == "plasticity") {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        } else if (experimentType == "development") {
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
        }
        
        // Run experiment
        AdvancedSimulationRunner::runEpisode(brain, world, agent, numSteps);
        
        // Collect results
        py::dict results;
        results["final_spikes"] = brain->getTotalSpikeCount();
        results["final_firing_neurons"] = brain->getFiringNeuronCount();
        results["avg_firing_rate"] = brain->getAverageFiringRate();
        results["development_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        results["dopamine_level"] = agent->getNeuromodulationLevel();
        results["curiosity_level"] = agent->getCuriosityLevel();
        
        return py::make_tuple(brain, world, agent, results);
    }
    
    static py::list getAvailableExperiments() {
        py::list experiments;
        experiments.append("plasticity");
        experiments.append("development");
        experiments.append("novelty");
        experiments.append("social_learning");
        experiments.append("curiosity_driven");
        return experiments;
    }
    
    static std::string getVersionInfo() {
        return "NLM Python Bindings v1.0.0 - Enhanced Edition";
    }
    
    static py::dict getSystemInfo() {
        py::dict info;
        info["version"] = "1.0.0";
        info["build_type"] = "Release";
        info["python_bindings"] = true;
        info["memory_management"] = "smart_pointers";
        info["performance_mode"] = "optimized";
        return info;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        
        An enhanced Python interface for the NLM C++ neural simulation framework.
        
        This edition provides:
        - Advanced brain configuration and simulation control
        - Expert-level simulation management with batch processing
        - Visualization and analysis tools
        - Performance monitoring and profiling
        - Enhanced error handling and validation
        - Experimental utilities for research
        
        Key Features:
        - Phase-based brain simulation with expert control
        - Advanced configuration management
        - Visualization and analysis tools
        - Performance monitoring and profiling
        - Batch simulation for research experiments
        - Neural topology analysis
        - Memory management utilities
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic types and enums
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

    // Configuration classes
    py::class_<Config>(m, "Config", R"pbdoc(Base configuration class for NLM system)pbdoc")
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
             "Get a summary string of the configuration");

    py::class_<AdvancedConfig>(m, "AdvancedConfig", R"pbdoc(Advanced configuration for expert users)pbdoc")
        .def(py::init<>())
        .def("getConfig", &AdvancedConfig::getConfig,
             "Get the underlying Config object")
        .def("setupStandardResearchConfig", &AdvancedConfig::setupStandardResearchConfig,
             "Setup configuration for standard research experiments")
        .def("setupHighPerformanceConfig", &AdvancedConfig::setupHighPerformanceConfig,
             "Setup configuration for high-performance simulations")
        .def("setupDevelopmentalConfig", &AdvancedConfig::setupDevelopmentalConfig,
             "Setup configuration for developmental studies");

    // Core simulation classes
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

    // World and sensory classes
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

    // Simulation utilities
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createAdvancedConfig", []() -> std::shared_ptr<AdvancedConfig> {
        return std::make_shared<AdvancedConfig>();
    }, "Create an advanced configuration for expert users");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced simulation utilities
    m.def("runEpisode", &AdvancedSimulationRunner::runEpisode,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
          py::arg("callback") = nullptr, py::arg("recordMetrics") = true,
          "Run a complete simulation episode with advanced features");

    m.def("runBatchSimulation", &AdvancedSimulationRunner::runBatchSimulation,
          py::arg("numEpisodes"), py::arg("stepsPerEpisode"), py::arg("config") = nullptr,
          py::arg("progressCallback") = nullptr,
          "Run multiple simulation episodes in batch");

    // Visualization and analysis utilities
    m.def("saveNeuralConnectivityMap", &VisualizationHelper::saveNeuralConnectivityMap,
          py::arg("brain"), py::arg("filename"),
          "Save neural connectivity map to file");

    m.def("generateNeuralTopologyReport", &VisualizationHelper::generateNeuralTopologyReport,
          py::arg("brain"),
          "Generate detailed neural topology analysis report");

    // Performance monitoring
    m.def("profileSimulation", &PerformanceMonitor::profileSimulation,
          py::arg("brain"), py::arg("steps"), py::arg("profileFile") = "profile.txt",
          "Profile simulation performance and save results");

    // Experiment utilities
    m.def("runExperiment", &PythonBrainHelper::runExperiment,
          py::arg("experimentType"), py::arg("config"), py::arg("numSteps") = 1000,
          "Run a predefined neuroscience experiment");

    m.def("getAvailableExperiments", &PythonBrainHelper::getAvailableExperiments,
          "Get list of available neuroscience experiments");

    // System information and utilities
    m.def("getVersionInfo", &PythonBrainHelper::getVersionInfo,
          "Get version and build information");

    m.def("getSystemInfo", &PythonBrainHelper::getSystemInfo,
          "Get detailed system information");

    // Constants and configuration
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Documentation and examples
    m.def("showExamples", []() {
        std::cout << "=== NLM Python Examples ===\n";
        std::cout << "\n1. Basic Brain Simulation:" << std::endl;
        std::cout << "```python" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << """brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()""" << std::endl;
        std::cout << "for i in range(100):" << std::endl;
        std::cout << "    brain.step(i)" << std::endl;
        std::cout << "print(f\"Total spikes: {brain.getTotalSpikeCount()}\")" << std::endl;
        std::cout << "```" << std::endl;
        
        std::cout << "\n2. Advanced Agent Simulation:" << std::endl;
        std::cout << "```python" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << """config = pynlm.createAdvancedConfig()
agent_config.setupStandardResearchConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)""" << std::endl;
        std::cout << """for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())""" << std::endl;
        std::cout << "```" << std::endl;
        
        std::cout << "\n3. Batch Experiment:" << std::endl;
        std::cout << "```python" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << "pynlm.runBatchSimulation(10, 1000, progressCallback=lambda e, t: print(f\"Episode {e}/{t}\"))" << std::endl;
        std::cout << "```" << std::endl;
        
        std::cout << "\n4. Neural Analysis:" << std::endl;
        std::cout << "```python" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << "pynlm.saveNeuralConnectivityMap(brain, 'connectivity.json')" << std::endl;
        std::cout << "report = pynlm.generateNeuralTopologyReport(brain)
print(report)" << std::endl;
        std::cout << "```" << std::endl;
        
    }, "Show example code and usage patterns");

    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Pybind11 module initialization
    py::class_<std::map<std::string, ConfigValue>>(m, "ConfigMap")
        .def(py::init<>());

    // Utility functions
    m.def("currentTime", []() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return duration.count();
    }, "Get current simulation time in milliseconds");
}

} // namespace nlm
